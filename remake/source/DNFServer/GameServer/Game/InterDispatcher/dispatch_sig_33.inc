int Inter_MonitorServerEventStart::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    // [N 判词·2026-09-11] 残差 = 帧 0x5c(ORIG) vs 0x6c + trace 对象槽 -0x48 vs -0x4c（单槽深 4）。
    //   试形（Main「指针局部→整型承载地址」旋钮）：`unsigned int pPktAddr` 承载 `data` + 宏视图
    //   访问，实测 **零变化**（仍 100 条、ndiff 5、帧 0x6c）⇒ 本件多出的 4B 不在 pPkt 指针分区
    //   （pPkt 槽两侧同为 -0x1c）⇒ 落 N126/N82（版本常数帧 + 域分区）。
    Inter_MonitorServerEventStartPktViewI* pPkt = reinterpret_cast<Inter_MonitorServerEventStartPktViewI*>(data);

    PacketGuard packet;                                // ORIG 构造后未使用，分支外统一析构

    if (pPkt->m_eventType == 1)
    {
        SendEventNotiPacket(true,
                            (short)pPkt->m_eventType,
                            (short)pPkt->m_10,
                            reinterpret_cast<CUser*>(0));
        CServerEvent().AddExpRate((int)pPkt->m_expRate);
        CServerEvent().TurnOnEvent(pPkt->m_eventType,
                                   (int)pPkt->m_10);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x53,
                                    (int)pPkt->m_10,
                                    (int)pPkt->m_expRate,
                                    // P248：ORIG 084cf451 `mov 0xa(%eax),%edi` 单段（无显式转换）
                                    pPkt->m_eventType);
        cMyTrace(__PRETTY_FUNCTION__,
            0x2c48, 0)("X-Mas Event Start");
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x2c4b, 5)("Unknown ServerEvent (%d)", pPkt->m_eventType);
    }
    return 0;
}