void CGuildServerProxy::SendRequestGuildSecede(unsigned int guildKey,
                                               unsigned int requesterCharNo,
                                               int nameLen, char* name)
{
    Packet_Guild_Request_Guild_Secede pkt;
    pkt.m_guildKey = guildKey;
    pkt.m_requesterCharNo = requesterCharNo;
    pkt.m_nameLen = nameLen;
    if (nameLen > 0)
    {
        memcpy(pkt.m_name, name, nameLen);
    }
    SendPacket((char*)&pkt, pkt.packetSize);
}