void GameWorld::UpdateMiniCraneSeed()
{
    // Packet_MiniCraneSeed：PacketHeader(0x27f8, 0xe) + m_data(+0x0a)。内联 ctor
    // 产出 ORIG 弱 W 符号 _ZN20Packet_MiniCraneSeedC1Ev（@0x086d1d18）。
    int group = G_CEnvironment()->get_server_group();
    Packet_MiniCraneSeed packet;
    GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)group)
        ->SendTcpPacket((char*)&packet, 0xe);
}