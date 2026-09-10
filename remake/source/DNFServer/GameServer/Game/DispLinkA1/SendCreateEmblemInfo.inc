void Dispatcher_DisJointAvatar::SendCreateEmblemInfo(
    std::vector<std::pair<int, int> > const& results)
{
    Packet_Emblem_Create_Statistic packet;
    if (results.size() >= 6)
        return;

    std::vector<std::pair<int, int> >::const_iterator it = results.begin();
    std::vector<std::pair<int, int> >::const_iterator end = results.end();
    // [b28d-solo-DispLinkA1 P239] ORIG 发射序：begin→end→count=0。
    int count = 0;
    while (it != end) {
        // [b28d-solo-DispLinkA1 P0] ORIG 具名槽 -0x14/-0x10（各一次 operator->）。
        const int itemIdx = it->first;
        const int value = it->second;
        // [b28d-solo-DispLinkA1 N] ORIG 实参先升格 %ebx 跨 receiver call
        // （G_CDataManager）保活；P226 语句表达式/裸形均不翻（4.4 恒先 call 后装载）。
        const CItem* item = G_CDataManager()->find_item(itemIdx);
        if (item == 0)
            return;
        packet.m_grades[count] = item->get_grade();
        packet.m_values[count] = value;
        ++count;
        if (count > 5)
            break;
        it++;   // [b28d-solo-DispLinkA1 P28] ORIG 调 operator++(int)（sret -0x1c）
    }
    // [b28d-solo-DispLinkA1 P196/P218] ORIG 为负条件（setne/jne）。
    if (count != (int)results.size())
        return;
    packet.m_count = count;
    // [b28d-solo-DispLinkA1 P167] ORIG 直调 GetServerProxy(0)（无匿名 helper）。
    GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
        ->SendPacket(reinterpret_cast<char*>(&packet), 0x36);
