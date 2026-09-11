void CMonitorServerProxy::SendNotifyNewMail(unsigned int charNo)
{
    Packet_Monitor_Notify_New_Mail pkt;
    pkt.m_charNo = charNo;
    SendPacket((char*)&pkt, pkt.packetSize);
}