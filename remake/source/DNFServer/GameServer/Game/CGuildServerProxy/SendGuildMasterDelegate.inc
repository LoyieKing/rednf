void CGuildServerProxy::SendGuildMasterDelegate(unsigned int guildKey,
                                                unsigned int requesterCharNo,
                                                int len, char* name)
{
    Packet_Guild_Request_Guild_Master_Delegate pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_requesterCharNo = requesterCharNo;
    memcpy(pkt.m_name, name, len);
    SendPacket((char*)&pkt, pkt.packetSize);
