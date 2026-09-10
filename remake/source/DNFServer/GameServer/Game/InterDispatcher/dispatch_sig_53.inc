int Inter_MonitorSetSubGuildMasterReply::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)                        // test eax; sete
        return 0;

    Inter_MonitorSetSubGuildMasterReplyPktView* pPkt = reinterpret_cast<Inter_MonitorSetSubGuildMasterReplyPktView*>(data);
    if (pPkt->m_12 !=        // msg+0x12 characNo
        user->get_charac_no(-1))
        return 0;

    PacketGuard packet;
    packet.put_header(1, 0x81);
    if (pPkt->m_a == 0)
    {
        packet.put_byte(1);
        packet.put_byte(pPkt->m_a);
        const int msgLen = (int)strlen(pPkt->m_17);
        packet.put_int(msgLen);
        packet.put_str(pPkt->m_17, msgLen);
        packet.put_byte(
            (int)pPkt->m_16);

        const unsigned char newGrade =                 // msg+0x16
            pPkt->m_16;
        const unsigned char oldGrade =                 // msg+0x35
            pPkt->m_35;
        const unsigned int grade =                     // msg+0x36
            pPkt->m_36;
        const unsigned int characNo =                  // msg+0x12
            pPkt->m_12;
        cUserHistoryLog* hist = &user->m_historyLog;
        hist->GuildChangeGrade(
            (unsigned int)user->get_charac_guildkey(),
            static_cast<const char*>(user->GetGuildDBInfo()),
            characNo, grade, oldGrade, newGrade);
    }
    else
    {
        packet.put_byte(0);
        packet.put_byte(pPkt->m_a);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
