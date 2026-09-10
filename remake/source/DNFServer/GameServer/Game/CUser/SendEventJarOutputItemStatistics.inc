void CUser::SendEventJarOutputItemStatistics(
    const std::vector<Inven_Item>& items)
{
    if ((int)items.size() != 2)
        return;
    unsigned short jarIdx = 0;
    if (items[0].m_addInfo != 0x28be7b)
    {
        if (items[1].m_addInfo != 0x28be7b)
            return;
        jarIdx = 1;
    }
    int itemCode = items[1 - (int)jarIdx].m_addInfo;
    int itemCount = items[(int)jarIdx].m_addInfo2;
    Packet_Event_UpgradeJar_Output_Statistic_GTS pkt;
    pkt.m_itemCode = itemCode;
    pkt.m_itemCount = itemCount;
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy(
            (ENUM_SERVER_GROUP)0);
    if (proxy != 0)
        proxy->SendPacket((char*)&pkt, 0x12);
