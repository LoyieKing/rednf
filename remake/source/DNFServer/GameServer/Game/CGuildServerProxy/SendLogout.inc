void CGuildServerProxy::SendLogout(unsigned int dbid, int channel, char* name,
                                   unsigned int cache0, unsigned int cache4,
                                   unsigned int memberKey, unsigned char foc)
{
    if (dbid == 0)
    {
    }
    else
    {
        Packet_Monitor_UDP_Logout pkt;
        pkt.m_dbid = dbid;
        pkt.m_channel = (unsigned char)channel;
        pkt.m_cache0 = cache0;
        pkt.m_cache4 = cache4;
        pkt.m_memberKey = memberKey;
        pkt.m_foc = foc;
        if (name != 0)
        {
            strcpy(pkt.m_name, name);
        }
        SendTcpPacket((char*)&pkt, pkt.packetSize);
    }
    return;
