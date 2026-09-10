void CGuildServerProxy::SendPowerWarPoint(Packet_Guild_Change_Power_War_Point& pkt)
{
    if (GlobalData::s_event_manager->GetRepeatEvent(0x1e)->IsEventing(0))
    {
        SendTcpPacket((char*)&pkt, pkt.packetSize);
    }
