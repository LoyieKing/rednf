void CAuctionServerProxy::SendRegistPacket()
{
    PCK_AUCTION_REGIST_GA packet;
#if !remake_verify
    packet.gc_no = G_CEnvironment()->m_serverEnvir.m_gcNo;   // DNF-FIX: ORIG 该值入包体
#else
    int envField = G_CEnvironment()->m_serverEnvir.m_gcNo;
#endif
    SendPacket((nsl::PACKET_HEADER*)&packet);
}