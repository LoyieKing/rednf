void CVillageAttackedStartCmd::execute()
{
    Packet_VillageAttackedGMCommand pkt;
    pkt.m_uid = GetUser()->GetUID();
    pkt.m_characNo = (unsigned int)GetUser()->get_charac_no(-1);
    pkt.m_vill = (unsigned int)reinterpret_cast<IntSlotView*>(this)->m_slot;
    pkt.m_field16 = 0;

    GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetUser()->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x1a);
