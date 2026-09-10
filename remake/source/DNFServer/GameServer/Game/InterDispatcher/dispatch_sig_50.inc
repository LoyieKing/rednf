int Inter_MonitorSetGuildKey::dispatch_sig(CUser* user, char* data, int len)
{
    Inter_MonitorSetGuildKeyPktView* pPkt = reinterpret_cast<Inter_MonitorSetGuildKeyPktView*>(data);
    const int local_10 = len;

    if (user->get_state() == 2)                        // test eax,eax; sete
    {
        user->set_charac_guildkey_in_login(            // msg+0x12, msg+0xe
            pPkt->m_12,
            pPkt->m_e);
        return 0;
    }

    if (user->get_state() <= 2)                        // cmp $2; setg → !(state>2)
        return 0x26d4;

    if (user->get_charac_no(-1) != pPkt->m_e)
    {
        LogManager::logFormat(
            1, "InterDispatcher.cpp",
            __PRETTY_FUNCTION__,
            0x26a7,
            "Inter_MonitorSetGuildKey::dispatch_sig() : pUser->get_charac_no(%d) != recv->m_uCharID(%d)",
            user->get_charac_no(-1),
            pPkt->m_e);
        return 0;
    }

    if (pPkt->m_12 == 0)
    {
        user->resetGuildDBInfo();
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 0x97);
        packet.finalize(true);
        user->Send(packet);
        user->send_skill_info();

        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(1);
        packet.put_short(1);
        user->make_basic_info(reinterpret_cast<char*>(&packet), '\x01');
        packet.finalize(true);
        user->Send(packet);
    }
    else
    {
        user->set_charac_guildkey(                     // CUserCharacInfo::set_charac_guildkey
            pPkt->m_12);
    }

    {
        PacketGuard packet;
        packet.clear();
        packet.put_header(0, 2);
        packet.put_byte(2);
        user->make_basic_info(reinterpret_cast<char*>(&packet), '\x02');
        packet.finalize(true);
        user->Send(packet);
    }
    return 0;
