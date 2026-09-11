void CMonitorServerProxy::SendRequestOnTimeEventReward(unsigned int a,
                                                       unsigned int b,
                                                       unsigned int c)
{
    Packet_GTM_OntimeEvent_RequestReward pkt;
    pkt.m_fieldA = a;
    pkt.m_fieldB = b;
    pkt.m_fieldC = c;
    SendPacket((char*)&pkt, pkt.packetSize);
}