int Inter_LoadBreakAwayPreventMessage::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user == 0)
        return 0;

    // ORIG：包基址在 user 判空后物化入 -0x14(%ebp)
    Inter_LoadBreakAwayPreventMessagePktView* pPkt = reinterpret_cast<Inter_LoadBreakAwayPreventMessagePktView*>(data);
    const int state = pPkt->m_4;      // msg+0x4
    const unsigned int questTime =                             // msg+0x0
        pPkt->m_0;
    if (state == 0 || state == 2)
    {   // 活动开启
        PacketGuard packet;
        packet.put_header(0, 0x111);
        packet.put_byte(state);
        packet.finalize(true);
        user->Send(packet);

        PacketGuard openPacket;
        openPacket.put_header(0, 0x136);
        openPacket.put_int(0x15180);
        openPacket.put_byte(0);
        openPacket.finalize(true);
        user->Send(openPacket);

        user->setBreakAwayDungeonClearCnt(3);
        user->setBreakAwayQuestTime(questTime);
    }
    else
    {   // 仅状态通知
        PacketGuard packet;
        packet.put_header(0, 0x111);
        packet.put_byte(state);
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
