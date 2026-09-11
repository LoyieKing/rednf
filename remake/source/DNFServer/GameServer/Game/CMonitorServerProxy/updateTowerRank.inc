void CMonitorServerProxy::updateTowerRank(unsigned int charNo, unsigned int job,
                                          unsigned int floor, unsigned int score)
{
    Packet_Request_Charac_Tower_Update_Ranking pkt;
    pkt.m_charNo = charNo;
    pkt.m_job = job;
    pkt.m_floor = floor;
    pkt.m_score = score;
    SendPacket((char*)&pkt, pkt.packetSize);
}