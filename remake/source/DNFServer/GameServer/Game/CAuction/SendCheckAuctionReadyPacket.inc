void CAuctionServerProxy::SendCheckAuctionReadyPacket()
{
    PCK_AUCTION_CHECK_AUCTION_READY_GA packet;
    SendPacket((nsl::PACKET_HEADER*)&packet);
