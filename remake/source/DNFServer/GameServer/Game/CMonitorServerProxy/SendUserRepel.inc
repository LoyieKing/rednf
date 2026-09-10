void CMonitorServerProxy::SendUserRepel(unsigned int dbid, unsigned int fieldA,
                                        char* name, int len)
{
    Packet_Monitor_User_Repel_ByCharName pkt;
    pkt.m_fieldA = fieldA;
    pkt.m_dbid = dbid;
    memcpy(pkt.m_name, name, len);
    SendPacket((char*)&pkt, pkt.packetSize);
