void upgradeAbilityByParty(int job, ActiveStaticInfo* info, int level)
{
    if (info == 0)
        return;
    if (job < 0)
        job = 0;
    else if (job > 3)
        job = 3;
    for (std::vector<DungeonPartyBalance>::size_type i = 0;
         i < DUNGEON_PARTY_BALANCE_TABLE.size(); ++i)
    {
        const DungeonPartyBalance& bal = DUNGEON_PARTY_BALANCE_TABLE[i];
        if (static_cast<float>(level) < bal.m_a)
            continue;
        if (static_cast<float>(level) != bal.m_b)
            continue;
        info->m_hpBase = static_cast<int>(static_cast<float>(info->m_hpBase) *
                                          bal.m_grid[4][job]);
        info->m_attr17Base[13] += static_cast<int>(bal.m_grid[3][job]);
        info->m_statBase6 += static_cast<int>(bal.m_grid[10][job]);
        for (int k = 0; k <= 0x10; ++k)
        {
            if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(k)) != 0x12)
                info->m_attr17Base[k] +=
                    static_cast<int>(bal.m_grid[7][job]);
        }
        info->m_statBase4 = static_cast<int>(static_cast<float>(info->m_statBase4) *
                                             bal.m_grid[5][job]);
        if (info->m_statBase3 != 0)
            info->m_statBase3 += static_cast<int>(bal.m_grid[6][job]);
        info->m_statBase11 = static_cast<int>(static_cast<float>(info->m_statBase11) *
                                              bal.m_grid[8][job]);
        info->m_statBase13 = static_cast<int>(static_cast<float>(info->m_statBase13) *
                                              bal.m_grid[8][job]);
        info->m_statBase12 = static_cast<int>(static_cast<float>(info->m_statBase12) *
                                              bal.m_grid[9][job]);
        info->m_statBase14 = static_cast<int>(static_cast<float>(info->m_statBase14) *
                                              bal.m_grid[9][job]);
        return;
    }
