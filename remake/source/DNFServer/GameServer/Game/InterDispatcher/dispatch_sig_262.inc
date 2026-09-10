int Inter_ApproveJoinGuild::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    Inter_ApproveJoinGuildPktView* pPkt = reinterpret_cast<Inter_ApproveJoinGuildPktView*>(data);
    if (user->get_state() <= 2)
        return 0;

    register int charIdMsg =                           // recv charId @ msg+0xe
        pPkt->m_e;
    if (charIdMsg != user->getCurCharacNo())
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x15f);
    if (pPkt->m_a != 0)
    {
        packet.put_byte(0);
        packet.put_byte((int)pPkt->m_a);
        packet.put_int(pPkt->m_16);
    }
    else
    {
        packet.put_byte(1);
        packet.put_int(pPkt->m_16);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
