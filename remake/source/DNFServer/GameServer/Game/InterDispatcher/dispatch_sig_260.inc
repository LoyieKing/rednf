int Inter_RequestJoinGuild::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                      // asm cmp $0x2; setle
        return 0;

    Inter_RequestJoinGuildPktView* pPkt = reinterpret_cast<Inter_RequestJoinGuildPktView*>(data);

    if (pPkt->m_0 != 0)          // msg+0x00 错误码
    {
        user->SendCmdErrorPacket(
            (ENUM_CMDPACKET)0x15d,
            static_cast<unsigned char>(
                pPkt->m_0 & 0xff));
        return 0;
    }

    PacketGuard packet;
    packet.put_header(1, 0x15d);
    packet.put_byte(1);
    packet.put_int(pPkt->m_4);
    const int guildNameLen =                         // msg+0x26 公会名
        static_cast<int>(strlen(pPkt->m_26));
    packet.put_int(guildNameLen);
    packet.put_str(pPkt->m_26, guildNameLen);
    const int requesterLen =                         // msg+0x08 申请者名
        static_cast<int>(strlen(pPkt->m_8));
    packet.put_int(requesterLen);
    packet.put_str(pPkt->m_8, requesterLen);
    packet.put_int(pPkt->m_40);
    packet.put_int(pPkt->m_44);
    packet.finalize(true);
    user->Send(packet);
    return 0;
