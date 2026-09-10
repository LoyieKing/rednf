stTempRewardInfo CNPCRelationEventManager::GetItemKind(
    unsigned short idx, unsigned int itemId, unsigned int count)
{
    stTempRewardInfo result;
    std::map<unsigned short, stNPCRelationEventInfo*>::iterator it =
        m_npcEventMap.find(idx);
    if (it == m_npcEventMap.end())
        return result;   // ORIG 0x0890d1d2 `mov %ebx,(%esp); call C1Ev` ⇒ NRVO 直构 sret

    stNPCRelationEventInfo* node = it->second;
    // ORIG 0x0890d1c0：不绑定 vector 引用/迭代器，每轮 size()/at() 现取
    // &node->m_favor[kind].m_items（m_items 在 favor[kind]+0，步长 0x14），
    // at() 结果缓存一次后读 m_itemId(+0)/m_count(+4)；ROUND=(int)(long double)
    // count/(long double)m_count（x87 fildll/fdivrp/fldcw-PC64/fistpl 最近舍入）。
    for (int kind = 0; kind < 3; ++kind)
    {
        for (unsigned int col = 0;
             col < node->m_favor[kind].m_items.size(); ++col)
        {
            stNPCEventItemInfo item = node->m_favor[kind].m_items.at(col);
            // ORIG 0x0890d277：`mov -0x24(%ebp),%eax; cmp 0x14(%ebp),%eax; jne`
            // （形参在 mem 源、字段入 eax）⇒ 相等比较按 ORIG 操作数序书写。
            if (itemId == (unsigned int)item.m_itemId
                && (unsigned int)item.m_count <= count)
            {
                if (item.m_count == 0)
                    return result;
                result.m_kind = kind;
                result.m_count =
                    (int)((long double)count / (long double)item.m_count);
                return result;
            }
        }
    }
    return result;
