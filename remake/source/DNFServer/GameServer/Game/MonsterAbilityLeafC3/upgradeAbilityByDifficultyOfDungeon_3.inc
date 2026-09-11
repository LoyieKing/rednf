void upgradeAbilityByDifficultyOfDungeon(int diff, ActiveStaticInfo* info,
                                         int level)
{
    // ORIG 0x88b85cf：全局表直接索引（无 bonus 指针变量）。
    if (MONSTER_DIFFICULTY_BONUS[diff].size() >= static_cast<unsigned int>(level))
    {
        std::vector<float>& row =
            MONSTER_DIFFICULTY_BONUS[diff].at(level - 1);   // ORIG 非 const at

        info->m_hpBase = static_cast<int>(
            static_cast<float>(info->m_hpBase) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x14] * row.at(4));
        info->m_attr17Base[13] += static_cast<int>(
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x0f] * row.at(3));
        info->m_statBase6 += static_cast<int>(
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x32] * row.at(10));
        for (int i = 0; i <= 0x10; ++i)
        {
            if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
                info->m_attr17Base[i] += static_cast<int>(
                    DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x23] * row.at(7));
        }
        info->m_statBase4 = static_cast<int>(
            static_cast<float>(info->m_statBase4) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x19] * row.at(5));
        if (info->m_statBase3 != 0)
            info->m_statBase3 += static_cast<int>(
                DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x1e] * row.at(6));
        info->m_statBase11 = static_cast<int>(
            static_cast<float>(info->m_statBase11) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28] * row.at(8));
        info->m_statBase13 = static_cast<int>(
            static_cast<float>(info->m_statBase13) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28] * row.at(8));
        info->m_statBase12 = static_cast<int>(
            static_cast<float>(info->m_statBase12) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d] * row.at(9));
        info->m_statBase14 = static_cast<int>(
            static_cast<float>(info->m_statBase14) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d] * row.at(9));
        return;
    }

    // ORIG 0x88b85cf：size<level 时 ORIG 内联展开 2 参版逻辑（非调用）。
    info->m_hpBase = static_cast<int>(
        static_cast<float>(info->m_hpBase) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x14]);
    info->m_attr17Base[13] += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x0f]);
    info->m_statBase6 += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x32]);
    for (int i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
            info->m_attr17Base[i] +=
                static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x23]);
    }
    info->m_statBase4 = static_cast<int>(
        static_cast<float>(info->m_statBase4) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x19]);
    if (info->m_statBase3 != 0)
        info->m_statBase3 += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x1e]);
    info->m_statBase11 = static_cast<int>(
        static_cast<float>(info->m_statBase11) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28]);
    info->m_statBase13 = static_cast<int>(
        static_cast<float>(info->m_statBase13) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28]);
    info->m_statBase12 = static_cast<int>(
        static_cast<float>(info->m_statBase12) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d]);
    info->m_statBase14 = static_cast<int>(
        static_cast<float>(info->m_statBase14) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d]);
}