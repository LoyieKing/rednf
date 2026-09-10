void CAuctionServerProxy::SendPacket(nsl::PACKET_HEADER* packet)
{
    SendTcpPacket((char*)packet, (int)packet->getSize());
