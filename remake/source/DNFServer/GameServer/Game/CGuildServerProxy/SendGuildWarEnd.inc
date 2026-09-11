void CGuildServerProxy::SendGuildWarEnd(unsigned char channel)
{
    Packet_Notice_Guild_War_End pkt;
    pkt.m_channel = channel;
    SendPacket((char*)&pkt, pkt.packetSize);
}