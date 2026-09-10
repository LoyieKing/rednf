int Inter_Antibot_ProcessProtectMsg::dispatch_sig(CUser* /*user*/, char* data, int /*len*/)
{
    Inter_Antibot_ProcessProtectMsgPktView* pPkt = reinterpret_cast<Inter_Antibot_ProcessProtectMsgPktView*>(data);
    CUser* target =
        getUser(pPkt->m_2);   // msg+0x2 uid
    if (target == 0 || target->get_state() == 0)
        return 0;

    short eventType = pPkt->m_0;           // msg+0x0
    // ORIG 0x84e4911/0x84e4915 `movb $0x0,-0x1b; movw $0x0,-0x1a` 前置清零 ⇒ 声明=0
    // + 分支内独立赋值（P197③）。
    char protstatLevel = 0;
    short extra = 0;
    if (eventType == 0x13)
    {
        protstatLevel = 0;
        extra = pPkt->m_6;           // short@0x6
    }
    else if (eventType == 0x14)
    {
        protstatLevel = (char)pPkt->m_8;
        extra = pPkt->m_a;
    }
    else if (eventType == 0x12)
    {
        protstatLevel = (char)pPkt->m_8;
        extra = pPkt->m_a;
    }
    else
    {
        protstatLevel = 0;
        extra = 0;
    }

    // P328⑩/P227：ORIG 0x84e497e..0x84e49aa 先求 extra→%edi、protstatLevel→%esi、uid→%ebx
    // 再 ctor ⇒ 全表达式临时链。
    cMyTrace(__PRETTY_FUNCTION__, 0x5f3b, 0)(
        "[SAFEMODE]uid : %d    protstatLevel : %d    eventType : %d\n",
        (int)pPkt->m_2,
        (int)protstatLevel, (int)(unsigned short)extra);

    if (protstatLevel == 1 || protstatLevel == 8)
    {   // 开启保护通知
        PacketGuard packet;
        packet.put_header(0, 0x1ee);
        packet.put_byte(1);
        packet.finalize(true);
        target->Send(packet);
    }
    if (protstatLevel == 0)
    {   // 关闭保护通知
        PacketGuard packet;
        packet.put_header(0, 0x1ee);
        packet.put_byte(0);
        packet.finalize(true);
        target->Send(packet);
    }
    return 0;
