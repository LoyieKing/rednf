void CMonitorServerProxy::SendGuildWarStart(unsigned char channel)
{
    Packet_Notice_Guild_War_Start pkt;
    pkt.m_channel = channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}