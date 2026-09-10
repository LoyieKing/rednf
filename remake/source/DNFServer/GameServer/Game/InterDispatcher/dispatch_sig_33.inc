#pragma pack(push, 1)
// P248 同型化：ORIG 084cf439/084cf465 对 +0x10 两处发 `movzwl %ax,%reg`（零扩展，用于
//   int 形参）⇒ 该字段语义 = unsigned short；(short) 形参处 ORIG 084cf3fa 为
//   `movzwl 0x10(%eax),%eax; movswl %ax,%edx`（无符号读 + 符号截断）⇒ 与 unsigned short 自洽。
//   聚合器 Inter_MonitorServerEventStartPktView.m_10 声明为 short（R1 禁改聚合器，
//   故本件自带同布局视图；✅ 建议属主把聚合器该字段改为 unsigned short）。
struct Inter_MonitorServerEventStartPktViewI
{
    char m_pad0[0xa];               // +0x00..未消费
    unsigned int m_a;               // +0x0a  (P248: ORIG 084cf451 `mov 0xa(%eax),%edi` 单段 = 与 InsertTimer 第 6 形参 unsigned int 同型)
    unsigned short m_e;             // +0x0e
    unsigned short m_10;            // +0x10
};
#pragma pack(pop)

int Inter_MonitorServerEventStart::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_MonitorServerEventStartPktViewI* pPkt = reinterpret_cast<Inter_MonitorServerEventStartPktViewI*>(data);

    PacketGuard packet;                                // ORIG 构造后未使用，分支外统一析构

    if (pPkt->m_a == 1)
    {
        SendEventNotiPacket(true,
                            (short)pPkt->m_a,
                            (short)pPkt->m_10,
                            reinterpret_cast<CUser*>(0));
        CServerEvent().AddExpRate((int)pPkt->m_e);
        CServerEvent().TurnOnEvent(pPkt->m_a,
                                   (int)pPkt->m_10);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x53,
                                    (int)pPkt->m_10,
                                    (int)pPkt->m_e,
                                    // P248：ORIG 084cf451 `mov 0xa(%eax),%edi` 单段（无显式转换）
                                    pPkt->m_a);
        cMyTrace(__PRETTY_FUNCTION__,
            0x2c48, 0)("X-Mas Event Start");
    }
    else
    {
        cMyTrace(__PRETTY_FUNCTION__,
            0x2c4b, 5)("Unknown ServerEvent (%d)", pPkt->m_a);
    }
    return 0;
