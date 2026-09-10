void CGuildDebug::execute()
{
    CUser* user = GetUser();
    Packet_Guild_Debug packet;
    packet.m_charNo = static_cast<unsigned int>(user->get_charac_no(-1));
    packet.m_guildKey =
        static_cast<unsigned int>(user->get_charac_guildkey());
    packet.m_mode = m_param1;
    packet.m_field16 = m_param2;
    packet.m_field1a = m_f1;  // +0x10

    // ORIG 0x084ac7cc：size 读包内 packetSize（双段 movzwl），&packet 与 size 先物化
    // %ebx/%esi 跨 GetServerProxy call 保活（P52 链式 receiver + R2L 实参独占）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&packet), packet.packetSize);
