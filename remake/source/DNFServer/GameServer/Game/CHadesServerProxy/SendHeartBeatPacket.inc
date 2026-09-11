void CHadesServerProxy::SendHeartBeatPacket(unsigned short udpPort, int channelNo)
{
    Packet_Hades_Udp_HeartBeat packet;
    packet.m_channelNo = (unsigned short)channelNo;
    packet.m_udpPort = udpPort;
    SendHadesUdpPacket((char*)&packet);
}