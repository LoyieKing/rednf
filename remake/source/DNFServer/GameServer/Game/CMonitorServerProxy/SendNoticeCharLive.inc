void CMonitorServerProxy::SendNoticeCharLive(unsigned int charNo)
{
    Packet_Monitor_Notice_Charac_Live_On_Ten_Min pkt;
    pkt.m_charNo = charNo;
    SendPacket((char*)&pkt, pkt.packetSize);
}