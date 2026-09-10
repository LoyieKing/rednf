unsigned long CAvatarItemMgr::RegistNotAssignedAvatarItem(int itemIdx, int tick,
                                                          const char* ipgAgency, bool b,
                                                          const stAvatarEmblemInfo_t& emblem)
{
    int tempKey = _AssignTempKey();
    Avatar_Item* item = m_AvatarItemPool.Acquire();
    // [b28d-38] ORIG 0x82f9xxx：SetData 三分支各物理内联一份（P165 多份独立块），
    // 无 expireDate 具名局部（各分支参数直接算）。
    if (itemIdx == 0 || itemIdx == 0x16d)
    {
        item->SetData(0, tick, ipgAgency);
    }
    else if (!b)
    {
        item->SetData(itemIdx * 0x15180 + tick, tick, ipgAgency);
    }
    else
    {
        item->SetData(tick + itemIdx, tick, ipgAgency);
    }
    item->setEmblemSocketData(emblem);
    item->setColor(0, 0);   // ORIG 真名 setColor(short,short)
    // ORIG 0x82f95d3: lea 0x18(%eax),%ecx → 插入 m_waitAssignItems(+0x18)
    m_waitAssignItems.insert(std::make_pair(tempKey, item));
    return (unsigned long)tempKey;
