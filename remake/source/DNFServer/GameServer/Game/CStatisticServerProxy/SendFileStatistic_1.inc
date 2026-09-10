void CStatisticServerProxy::SendFileStatistic(const char* file, const char* key)
{
    Packet_File_Statistic pkt;
    strcpy(pkt.m_fileName, key);
    strcpy(pkt.m_fileData, file);
    SendPacket((char*)&pkt, pkt.packetSize);
