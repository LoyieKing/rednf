int CQuickPartyRewardManager::getRequireQuickPartyMinCount(int type,
                                                           int dungeonDiff)
{
    if (type < 0 || type > 1)
        return -1;
    if (dungeonDiff < 0 || dungeonDiff > 3)
        return -1;
    return m_minCounts[type][dungeonDiff];
