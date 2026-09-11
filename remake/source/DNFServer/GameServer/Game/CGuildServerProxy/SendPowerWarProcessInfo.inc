void CGuildServerProxy::SendPowerWarProcessInfo(unsigned int channel)
{
    Packet_Guild_Power_War_Process_Info pkt;
    pkt.m_a = channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}