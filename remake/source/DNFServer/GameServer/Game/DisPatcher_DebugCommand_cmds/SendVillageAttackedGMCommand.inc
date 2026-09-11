void SendVillageAttackedGMCommand(CUser* user, int state)
{
    Packet_VillageAttackedGMCommand pkt;                         // ORIG 0x820bb75
    pkt.m_uid = user->GetUID();                                  // ORIG 0x820bb80（-0x18）
    pkt.m_characNo =
        static_cast<unsigned int>(user->get_charac_no(-1));      // ORIG 0x820bb96（get_charac_no(0xffffffff)）
    pkt.m_field16 = static_cast<unsigned int>(state);            // ORIG 0x820bba1（-0xc）
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(
        user->GetServerGroup())->SendTcpPacket(                  // ORIG 0x820bbbf（&pkt 跨调用物化 ebx）
        reinterpret_cast<char*>(&pkt), 0x1a);                    // ORIG 0x820bbd3
}