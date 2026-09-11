// ORIG execute 0x084ab886：仅 GM 用户生效；组 Packet_Monitor_Event_Start
//（0x44f/0x12：eventType=eventCode，p1/p2 两 ushort），经监控代理按
// 发起者 server group 下发 TCP 包。
void CStartEvent::execute()
{
    CUser* user = GetUser();
    if (!user->isGMUser())
    {
        return;
    }

    Packet_Monitor_Event_Start packet;
    packet.m_field1 = static_cast<unsigned int>(m_eventId);
    packet.m_field2 = m_param1;
    packet.m_field3 = m_param2;
    register Packet_Monitor_Event_Start* pPkt = &packet;
    GlobalData::s_monitor_proxy_mgr->GetServerProxy(user->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(pPkt), 0x12);
}