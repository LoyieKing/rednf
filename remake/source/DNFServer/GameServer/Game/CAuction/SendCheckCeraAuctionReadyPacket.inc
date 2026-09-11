void CCeraAuctionServerProxy::SendCheckCeraAuctionReadyPacket()
{
    PCK_AUCTION_CHECK_AUCTION_READY_GP packet;
    SendPacket((nsl::PACKET_HEADER*)&packet);
}