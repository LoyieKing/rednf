void CCommunityServerProxy::SendLogout(unsigned int accId, unsigned int characNo)
{
    Packet_Community_Logout pkt;
    pkt.m_id = accId;
    pkt.m_characNo = characNo;
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
