// 组 Packet_VillageAttackedGMCommand 经 monitor proxy 发往本人所属服务器组。
void CVillageAttackedStartCmd::execute()
{
    Packet_VillageAttackedGMCommand pkt;
    pkt.m_uid = GetUser()->GetUID();
    pkt.m_characNo = (unsigned int)GetUser()->get_charac_no(-1);
    pkt.m_vill = (unsigned int)m_villageNo;
    pkt.m_field16 = 0;

    GlobalData::s_monitor_proxy_mgr->GetServerProxy(GetUser()->GetServerGroup())
        ->SendTcpPacket(reinterpret_cast<char*>(&pkt), 0x1a);
}