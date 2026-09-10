void Dispatcher_CompoundEmblem::SendCreateEmblemInfo(
    const std::vector<std::pair<int, int> >& created)
{
    // b28d-40: N82 封顶——帧 0x74 vs 0x64 + 迭代器/局部槽平移，语义闭合。ORIG 0x08221dde。
    Packet_Emblem_Create_Statistic packet;

    // ORIG 0x08221dfb seta：size()>5 直接返回。
    if (created.size() > 5)
        return;
    // [b29 R20] 判据⑩/P28：ORIG 0x08221f05 `movl $0x0,-0x18`（count=0）出现在
    // begin(-0x20)/end(-0x24) 调用**之后** ⇒ count 声明后移至迭代器之后。
    std::vector<std::pair<int, int> >::const_iterator it = created.begin();
    std::vector<std::pair<int, int> >::const_iterator end = created.end();
    int count = 0;
    while (it != end)
    {
        // [b29 R20] ORIG 0x08221e3f/0x08221e4f 两次 operator->（P157 逐字段重读，
        // 每次落具名 int 槽 -0x14/-0x10）：key=it->first、value=it->second。
        int key = it->first;
        int value = it->second;
        const CItem* item = G_CDataManager()->find_item(key);
        if (item == 0)
            return;                       // ORIG 中途 return
        packet.m_slots[count + 1] = item->get_grade();
        packet.m_slots[count + 6] = value;
        ++count;
        if (count > 5)
            break;
        it++;                             // [b29 R20] P28 增量侧：ORIG 调 ppEi（后置）
    }
    // ORIG 0x08221ef4 setne：count != size 时返回。
    if (count != (int)created.size())
        return;
    packet.m_slots[0] = count;
    CStatisticServerProxy* proxy =
        GlobalData::s_statistic_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0);
    proxy->SendPacket(reinterpret_cast<char*>(&packet), 0x36);
}
