int Inter_MonitorNoticeGuildInfo::dispatch_sig(CUser* user, char* data, int /*len*/)
{
    if (user->get_state() == 0)
        return 0;

    // ORIG 0x084****：state 岛后 pPkt 才物化 -0x1c（技巧13）。
    Inter_MonitorNoticeGuildInfoPktView* pPkt = reinterpret_cast<Inter_MonitorNoticeGuildInfoPktView*>(data);
    if (user->get_charac_no(-1) !=
        pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x2613,
            "Inter_MonitorNoticeGuildInfo::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    user->SetGuildDBInfo(                              // ORIG E 引用（头为 const&）
        *reinterpret_cast<STGuildDBInfoOnly*>(data + 0x16));

    PacketGuard packet;                                // -0x28
    packet.put_header(0, 0x46);

    const int guildNameLen = (int)strlen(pPkt->m_16);   // -0x18
    packet.put_int(guildNameLen);
    packet.put_str(pPkt->m_16, guildNameLen);

    if (user->get_charac_no(-1) ==
        pPkt->m_2d)          // 会长标记
        packet.put_byte(1);
    else
        packet.put_byte(0);

    packet.put_byte((int)pPkt->m_31);
    packet.put_byte((int)((((unsigned char)pPkt->m_32) >> 1) & 1));
    packet.put_short((int)pPkt->m_38);
    packet.put_byte((int)(user->get_guild_create_right() & 0xff));
    packet.put_int(pPkt->m_3a);
    packet.put_byte((int)pPkt->m_3e);
    packet.put_int(pPkt->m_3f);
    packet.put_short((int)pPkt->m_58);

    int memberCountPos = packet.get_index();           // -0x2c（put_byte(int&,int) 回填需左值）
    int memberCount = 0;                               // -0x14
    packet.put_byte(0);
    // ORIG 循环体 `mov -0xSLOT(%ebp),%ecx; lea (%ecx,%eax,1),%eax`：基址走 pPkt 具名槽。
