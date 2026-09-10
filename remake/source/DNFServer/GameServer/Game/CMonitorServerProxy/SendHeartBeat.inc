void CMonitorServerProxy::SendHeartBeat(int channel)
{
    Packet_Monitor_UDP_HeartBeat pkt;
    pkt.m_channel = (unsigned char)channel;
    SendPacket((char*)&pkt, pkt.packetSize);
