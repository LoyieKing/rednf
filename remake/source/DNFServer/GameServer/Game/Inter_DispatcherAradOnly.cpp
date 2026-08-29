// df_game_r 还原 —— Inter_DispatcherAradOnly 实现（报告 docs/class_func_reports/
// Inter_DispatcherAradOnly/）。C1 0x084e7eb8：基类 ISigDispatcher() + 设 vptr +
// Arad_InternalDispatcher C1 + Init()；dispatch_sig 0x084e7f2c：读 data 前 4
// 字节 code → GetDispatcher → 空 cMyTrace(0x6ba2,5)+getCurCharacNo 返 0x6ba3
// → 否则虚调槽2 dispatch_sig(data+4, len-4)。
#include "Inter_DispatcherAradOnly.h"

#include "GameTypes.h"  // cMyTrace
#include "CUser.h"      // CUserCharacInfo::getCurCharacNo

Inter_DispatcherAradOnly::Inter_DispatcherAradOnly()
{
    m_disp.Init();   // ORIG @0x084e7eeb call Arad_InternalDispatcher::Init()
}

Inter_DispatcherAradOnly::~Inter_DispatcherAradOnly()
{
}

int Inter_DispatcherAradOnly::dispatch_sig(CUser* user, char* data, int len)
{
    int code = *(int*)data;   // ORIG mov (%eax),%eax 直读 4 字节（local_14）
    ARAD::DISPATCHER::Arad_IInterDispatcher* disp =
        m_disp.GetDispatcher((ARAD::ENUM_INTERNALPACKET_JPN)code);   // ORIG call GetDispatcher
    if (disp == 0)
    {
        // ORIG：cMyTrace 临时链式（getCurCharacNo 先求值存 ebx 跨构造调用缓存）
        cMyTrace(__PRETTY_FUNCTION__, 0x6ba2, 5)
            ("[DISPATCHER_ARAD] Invalid Internal Packet Code. (code:%d, charac:%u)",
             code, user->getCurCharacNo());
        return 0x6ba3;   // ORIG mov $0x6ba3,%eax
    }
    return disp->dispatch_sig(user, data + 4, len - 4);   // ORIG 虚槽2（vptr+8）直调
}
