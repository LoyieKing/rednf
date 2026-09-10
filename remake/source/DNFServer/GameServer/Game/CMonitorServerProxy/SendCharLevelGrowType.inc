void CMonitorServerProxy::SendCharLevelGrowType(unsigned int dbid, unsigned int growType,
                                                short guildMemFlag, short level)
{
    Packet_Monitor_Char_Changable_Info pkt;
    pkt.m_dbid = dbid;
    pkt.m_padE = (char)growType;
    pkt.m_guildMemFlag = guildMemFlag;
    pkt.m_growthType = (char)level;
    SendPacket((char*)&pkt, pkt.packetSize);
