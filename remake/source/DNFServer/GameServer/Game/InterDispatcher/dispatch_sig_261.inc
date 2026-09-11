int Inter_JoinGuildInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() <= 2)                        // setle：signed <= 2
        return 0;

    // ORIG 0x084e3af5：null 岛后单槽 -0x10=recv（pPkt 经 cast 内联复用同槽，技巧36）。
    char* recv = data;                                 // -0x10(%ebp) 缓存消息指针

    PacketGuard packet;
    packet.put_header(0, 0x131);
    packet.put_byte(
        (int)reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_0);   // movzbl+movsbl
    if (*recv != '\0')
    {
        packet.put_int(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_4);
        int strLen = strlen(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_26);
        packet.put_int(strLen);
        packet.put_str(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_26, strLen);
        strLen = strlen(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_8);
        packet.put_int(strLen);
        packet.put_str(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_8, strLen);
        packet.put_int(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_40);
        packet.put_int(
            reinterpret_cast<Inter_JoinGuildInfoPktView*>(recv)->m_44);
    }
    packet.finalize(true);
    user->Send(packet);
    return 0;
}