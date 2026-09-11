// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
// upgradeAbilityByDifficultyOfDungeon — ORIG 0x88b8c37（2 参版本）：
// 仅按 DIFFICULTY_OF_DUNGEON_BONUS 全局表（扁平 [d+off]）提升，无行乘。
// ---------------------------------------------------------------------------
void upgradeAbilityByDifficultyOfDungeon(int diff, ActiveStaticInfo* info)
{
    // ORIG 0x88b8c37：全局表直接索引（无 bonus 指针变量）。
    // +0x04 m_hpBase = trunc(hpBase*BONUS[d+0x14])
    info->m_hpBase = static_cast<int>(
        static_cast<float>(info->m_hpBase) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x14]);
    // +0xac attr17Base[13] += BONUS[d+0x0f]
    info->m_attr17Base[13] += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x0f]);
    // +0x130 statBase6 += BONUS[d+0x32]
    info->m_statBase6 += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x32]);
    // +0x78+i*4 attr17Base[i] +=（i=0..16；容错映射 0x12 跳过；
    // ORIG 0x88b8c37 用 setne+je 形式，即 if (x != 0x12) 正写）
    for (int i = 0; i <= 0x10; ++i)
    {
        if (getToleranceActiveStatus(static_cast<ENUM_ACTIVEPROPERTY>(i)) != 0x12)
            info->m_attr17Base[i] +=
                static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x23]);
    }
    // +0x120 statBase4 = trunc(statBase4*BONUS[d+0x19])
    info->m_statBase4 = static_cast<int>(
        static_cast<float>(info->m_statBase4) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x19]);
    // +0x118 statBase3 += （非零守卫）
    if (info->m_statBase3 != 0)
        info->m_statBase3 += static_cast<int>(DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x1e]);
    // +0x158/+0x168 statBase11/13 = trunc(*BONUS[d+0x28])
    info->m_statBase11 = static_cast<int>(
        static_cast<float>(info->m_statBase11) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28]);
    info->m_statBase13 = static_cast<int>(
        static_cast<float>(info->m_statBase13) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x28]);
    // +0x160/+0x170 statBase12/14 = trunc(*BONUS[d+0x2d])
    info->m_statBase12 = static_cast<int>(
        static_cast<float>(info->m_statBase12) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d]);
    info->m_statBase14 = static_cast<int>(
        static_cast<float>(info->m_statBase14) * DIFFICULTY_OF_DUNGEON_BONUS[0][diff + 0x2d]);
}