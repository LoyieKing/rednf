// ---------------------------------------------------------------------------
// upgradeAbilityByDifficultyOfDungeon — ORIG 0x88b813a（4 参版本；level 参数
// 在本重载中未使用）。table 为副本难度表，行访问除 +0x118 一处使用 operator[]
// 外均为 at()（保留 ORIG 的越界抛出语义）；BONUS 扁平索引见各注释。
// ---------------------------------------------------------------------------
// b28d-solo-MonsterAbilityLeafC3 verdict=AE ORIG 0x088b813a orig/ours=330/330。
// 真语义修复：attr17Base[13]/statBase6/attr17Base[i]/statBase3 四项原先多乘了字段
// 自身（ORIG 仅 m_hpBase/statBase4/statBase11..14 乘字段）；表空时须回落调用 3 参版
// （ORIG 尾 call _Z35upgradeAbilityByDifficultyOfDungeoniP16ActiveStaticInfoi）。
// 形：内联乘算（删 scaleByBonus helper）、BONUS 全局直接索引、`if (!table.empty())`
// 取反守卫（P292）、循环 `!= 0x12` 正向形（setne/je）、行访问 at()/operator[] 照抄。
void upgradeAbilityByDifficultyOfDungeon(int diff, ActiveStaticInfo* info,
