bool DisPatcher_DebugCommand::_debugCommandPowerwarPointUp(CUser* user,
                                                           MSG_DEBUG_COMMAND& msg)
{
    if (!user->isGMUser())
        return false;

    Packet_Guild_Change_Power_War_Point pkt;                    // ctor(0x6a4,0x2d)
    pkt.m_winPoint = (unsigned char)msg.m_arg;                  // pkt+0x2b ← msg[0xe] 低字节
    pkt.m_losePoint = 0;                                        // pkt+0x2c = 0
    pkt.m_powerSide = user->getPowerSide();                     // pkt+0x0a ← getPowerSide
    pkt.m_characNo[0] = user->get_charac_no(-1);                // pkt+0x0b dword ← charac_no(-1)
    GlobalData::s_guild_proxy_mgr->GetServerProxy(
        static_cast<ENUM_SERVER_GROUP>(0))->SendPowerWarPoint(pkt);  // +0x7a
    return true;
