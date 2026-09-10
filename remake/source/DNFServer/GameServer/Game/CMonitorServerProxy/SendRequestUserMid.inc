void CMonitorServerProxy::SendRequestUserMid(unsigned int dbid, unsigned int gmId,
                                             char* name, int len)
{
    Packet_GM_Request_Mid pkt;
    pkt.m_gmId = gmId;
    pkt.m_dbid = dbid;
    memcpy(pkt.m_name, name, len);
    SendPacket((char*)&pkt, pkt.packetSize);
