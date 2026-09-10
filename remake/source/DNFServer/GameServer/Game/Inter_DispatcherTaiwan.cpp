// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Inter_DispatcherTaiwan/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— Inter_DispatcherTaiwan 实现（报告 docs/class_func_reports/
// Inter_DispatcherTaiwan/）。C1 0x084e8bba：基类 ISigDispatcher() + 设 vptr +
// TaiwanInternalDispatcher C1 + init()；dispatch_sig 0x084e8c2e：读 data 前 4
// 字节 code → getDispatcher → 空 cMyTrace(0x6dca,5)+getCurCharacNo 返 0x6dcb
// → 否则虚调槽2 dispatch_sig(data+4, len-4)。
#include "Inter_DispatcherTaiwan.h"

#include "GameTypes.h"                       // cMyTrace
#include "CUser.h"                           // CUserCharacInfo::getCurCharacNo
#include "TaiwanInterfaceInterDispatcher.h"  // dispatch_sig 虚调用需完整类型

#include "Inter_DispatcherTaiwan/Inter_DispatcherTaiwan.inc"
{
    m_disp.init();   // ORIG @0x084e8bed call TaiwanInternalDispatcher::init()
}

#include "Inter_DispatcherTaiwan/dispatch_sig.inc"
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
