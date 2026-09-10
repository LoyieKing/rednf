bool CAuctionServerProxy::Parsing(int recvLen)
{
    return GetTcpHandler()->Parsing_AuctionServerPacket(recvLen);
