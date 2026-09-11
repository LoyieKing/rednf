void CGuildServerProxy::SendReqestPowerWarStartInfo()
{
    Packet_Request_Power_War_Start_Info pkt;
    pkt.m_channel = G_CEnvironment()->get_channel_no();
    SendTcpPacket((char*)&pkt, pkt.packetSize);
}