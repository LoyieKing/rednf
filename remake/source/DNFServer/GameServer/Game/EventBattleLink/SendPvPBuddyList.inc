void CCommunityServerProxy::SendPvPBuddyList(unsigned int accId, unsigned int characNo)
{
    Packet_Request_PvP_Buddy_Conn_List packet;
    packet.m_accId = accId;         // -0x14
    packet.m_characNo = characNo;   // -0x10
    SendTcpPacket(reinterpret_cast<char*>(&packet), packet.packetSize);  // 0x12
