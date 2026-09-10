void CMonitorServerProxy::SendExchangeServerInfo(int channel, int ip, short port)
{
    Packet_Exchange_Server_Info pkt;
    pkt.m_channel = channel;
    pkt.m_ip = ip;
    pkt.m_port = port;
    SendPacket((char*)&pkt, pkt.packetSize);
