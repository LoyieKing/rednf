// ORIG _ZN21CStatisticServerProxy17SendFileStatisticEPKcS1_（0x084720b4，T）：
// Packet_File_Statistic 头 + 双缓冲（文件名 m_fileName / 内容 m_fileData），
// packetSize（+2 ushort）作发送长度。
void CStatisticServerProxy::SendFileStatistic(const char* file, const char* key)
{
    Packet_File_Statistic pkt;
    strcpy(pkt.m_fileName, key);
    strcpy(pkt.m_fileData, file);
    SendPacket((char*)&pkt, pkt.packetSize);
}