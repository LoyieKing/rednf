void CCeraAuctionServerProxy::SendRegistPacket()
{
    PCK_AUCTION_REGIST_GP packet;
    int envField = G_CEnvironment()->m_serverEnvir.m_gcNo;
    SendPacket((nsl::PACKET_HEADER*)&packet);
