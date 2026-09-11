// ORIG T 0x0826d8a2（报告 getRequireQuickPartyMinCount.md）：
// type∈[0,1]、diff∈[0,3] 越界返回 -1；命中查 m_minCounts[type][diff]
// （寻址 +0x1a0 + type*0x10 + diff*4）。
int CQuickPartyRewardManager::getRequireQuickPartyMinCount(int type,
                                                           int dungeonDiff)
{
    if (type < 0 || type > 1)
        return -1;
    if (dungeonDiff < 0 || dungeonDiff > 3)
        return -1;
    return m_minCounts[type][dungeonDiff];
}