void upgradeAbilityByDifficultyOfDungeon(
    int diff, ActiveStaticInfo* info, int level,
    const std::vector<std::vector<float> >& table)
{
    (void)level;  // ORIG 本体未读取 0x10(%ebp)
    // ORIG 0x088b813a：empty 守卫为 `call empty; xor $1; test; je <body>`（P292
    // bool 取反形 ⇒ 源码 `if (!table.empty())`）；乘算全内联（无 helper call），
    // BONUS 表全局直接索引（无 bonus 指针局部），行访问 at()/operator[] 照抄。
    if (!table.empty())
    {
        info->m_hpBase = static_cast<int>(
            static_cast<float>(info->m_hpBase) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x14] *
            table.at(4).at(diff));
        info->m_attr17Base[13] += static_cast<int>(
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x0f] *
            table.at(3).at(diff));
        info->m_statBase6 += static_cast<int>(
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x32] *
            table.at(10).at(diff));
        for (int i = 0; i <= 0x10; ++i)
        {
            if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) !=
                0x12)
                info->m_attr17Base[i] += static_cast<int>(
                    DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x23] *
                    table.at(7).at(diff));
        }
        info->m_statBase4 = static_cast<int>(
            static_cast<float>(info->m_statBase4) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x19] *
            table.at(5).at(diff));
        if (info->m_statBase3 != 0)
            info->m_statBase3 += static_cast<int>(
                DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x1e] * table[6][diff]);
        info->m_statBase11 = static_cast<int>(
            static_cast<float>(info->m_statBase11) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28] *
            table.at(8).at(diff));
        info->m_statBase13 = static_cast<int>(
            static_cast<float>(info->m_statBase13) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28] *
            table.at(8).at(diff));
        info->m_statBase12 = static_cast<int>(
            static_cast<float>(info->m_statBase12) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d] *
            table.at(9).at(diff));
        info->m_statBase14 = static_cast<int>(
            static_cast<float>(info->m_statBase14) *
            DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d] *
            table.at(9).at(diff));
        return;
    }
    // ORIG 0x088b813a 尾部 `call _Z35upgradeAbilityByDifficultyOfDungeoniP16ActiveStaticInfoi`
    // ⇒ 表空时回落到 3 参版（0x088b85cf），非内联展开。
    upgradeAbilityByDifficultyOfDungeon(diff, info, level);
}