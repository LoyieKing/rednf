void CUser::StartRecvEventWaiting()
{
    m_mcap.m_recvEventWaiting = false;                // CMCAPManager +0x18（CUser+0x8e408）

    // ORIG 0x0867f1fe：`movl $0x1e,-0xc(%ebp)` 顶部落槽；uid/uidw 跨 call 直落 %esi/%ebx
    // P24 内联实参形尝试（4.4 若直落 callee-saved 即闭合；否则 4.1 regmove 专属 N）。
    int delay = 0x1e;
    G_TimerQueue()->InsertTimer(static_cast<TimerEntry::OBJ_TYPE>(2),
                                GetUID(),
                                static_cast<TIMER_MESSAGE>(0x67),
                                delay,
                                static_cast<int>(static_cast<unsigned short>(get_unique_id())),
                                0u);
