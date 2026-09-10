void CAvatarDisjointRandomReward::SendCreateEmblemStatistic(
    std::vector<std::pair<int, int> >& rewards)
{
    // [b28c-L0 N6/P30] ORIG 两个独立数组基址（grade @-0x40 直接索引 n；
    // value @-0x4c+`lea 0x8(%ebx)`），我方单数组 m_slots 恒 `lea 1/lea 6`
    // 折同一地址。值索引已按 ORIG 槽语义修为 m_slots[n+6]（越界尾随照抄 P171）。
    Packet_Emblem_Create_Statistic packet;
    if (rewards.size() > 5)
        return;

    int n = 0;
    for (std::vector<std::pair<int, int> >::iterator it = rewards.begin();
         it != rewards.end(); ++it)
    {
        CItem* citem = G_CDataManager()->find_item(it->first);
        if (citem == 0)
            return;
        if (citem->GetItemType() == 0x14)
        {
            packet.m_grades[n] = citem->get_grade();
            packet.m_values[n] = it->second;
            ++n;
            if (n > 5)
                break;
        }
    }
    if (n <= 0)
        return;
    packet.m_slots[0] = n;
    GlobalData::s_statistic_proxy_mgr
        ->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x36);
