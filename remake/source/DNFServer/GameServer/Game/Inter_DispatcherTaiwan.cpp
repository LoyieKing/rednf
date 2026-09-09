// df_game_r 还原 —— Inter_DispatcherTaiwan 实现（报告 docs/class_func_reports/
// Inter_DispatcherTaiwan/）。C1 0x084e8bba：基类 ISigDispatcher() + 设 vptr +
// TaiwanInternalDispatcher C1 + init()；dispatch_sig 0x084e8c2e：读 data 前 4
// 字节 code → getDispatcher → 空 cMyTrace(0x6dca,5)+getCurCharacNo 返 0x6dcb
// → 否则虚调槽2 dispatch_sig(data+4, len-4)。
#include "Inter_DispatcherTaiwan.h"

#include "GameTypes.h"                       // cMyTrace
#include "CUser.h"                           // CUserCharacInfo::getCurCharacNo
#include "TaiwanInterfaceInterDispatcher.h"  // dispatch_sig 虚调用需完整类型

Inter_DispatcherTaiwan::Inter_DispatcherTaiwan()
{
    m_disp.init();   // ORIG @0x084e8bed call TaiwanInternalDispatcher::init()
}

int Inter_DispatcherTaiwan::dispatch_sig(CUser* user, char* data, int len)
{
    // b28d-lane12: AE（4.4.6 后端路由后 ndiff=0）。修法：末实参 len-4 →
    // len-sizeof(int)（unsigned 转换阻断 4.4 的 lea 折叠）复现 ORIG 0x084e8cbd
    // `mov 0x14(%ebp),%eax; sub $0x4,%eax`；4.4.6 后端复现 ORIG 0x084e8cba
    // `mov (%eax),%edx` + 0x084e8cdb `call *%edx`（4.4.7 为 N7：间接 call 目标
    // 恒 %eax，ndiff 24→18 NEAR；P230 补：换后端即翻）。探针 /tmp/l12_probe2.cpp
    // g5 逐条全等。
    int code = *(int*)data;   // ORIG mov (%eax),%eax 直读 4 字节（local_14）
    TaiwanInterfaceInterDispatcher* disp =
        m_disp.getDispatcher((TaiwanInternalPack::T)code);   // ORIG call getDispatcher
    if (disp == 0)
    {
        // ORIG：cMyTrace 临时链式（getCurCharacNo 先求值存 ebx 跨构造调用缓存）
        cMyTrace(__PRETTY_FUNCTION__, 0x6dca, 5)
            ("[TaiwanDispatcher] Invalid internal packet code. Code:%d, Charac:%u)",
             code, user->getCurCharacNo());
        return 0x6dcb;   // ORIG mov $0x6dcb,%eax
    }
    return disp->dispatch_sig(user, data + 4, len - sizeof(int));   // ORIG 虚槽2（vptr+8）直调
}
