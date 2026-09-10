void CCommunityServerProxy::SendResAddPvPBuddy(
    unsigned int accId, unsigned int characNo, char flag, char* name, int nameLen)
{
    Packet_Response_Add_PvP_Buddy pkt;   // ORIG W ctor 0x0846cf90
    pkt.m_accId = accId;
    pkt.m_characNo = characNo;
    pkt.m_flag = flag;
    memcpy(pkt.m_name, name, nameLen);
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
