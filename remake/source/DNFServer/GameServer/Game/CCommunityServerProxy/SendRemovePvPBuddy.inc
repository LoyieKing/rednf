// ORIG T 0x0846cc88 _ZN21CCommunityServerProxy18SendRemovePvPBuddyEjjcPci
void CCommunityServerProxy::SendRemovePvPBuddy(
    unsigned int accId, unsigned int characNo, char flag, char* name, int nameLen)
{
    Packet_Request_Remove_PvP_Buddy pkt;   // ORIG W ctor 0x0846cfec
    pkt.m_accId = accId;
    pkt.m_characNo = characNo;
    pkt.m_flag = flag;
    memcpy(pkt.m_name, name, nameLen);
    SendTcpPacket(reinterpret_cast<char*>(&pkt), pkt.packetSize);
}