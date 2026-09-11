// upgradeAbilityByParty —— ORIG 0x088b8e88：job 0..3 截断；首个满足
// level>=m_a && level==m_b 的 balance 应用后立即返回。
// b28d-solo-MonsterAbilityLeafC3 verdict=DIFF[N85/N121] ORIG 0x088b8e88
// orig/ours=229/229 ndiff=40。已复现：m_grid[row][job] 二维索引（行=偏移/4，复现
// add+scaled 而非指针算术 shl 链，P311/P30）、job 截断、do-while 结构、表全局直接
// 传址。残余：x87 比较 lowering（ORIG fucompp/fnstsw/sahf vs 4.4 fucomip/fstp，
// N85 跨版本）+ 命中 return 的 epilogue 尾岛（N121）。语义等价封顶。
// [D07 试形记录 2026-09-10] ①`-march=i486 -mtune=generic` 手工追加旗标重编本 TU：
// party/rand/ridable 三件 verdict 逐字不变（我方仍 `fucomip %st(1),%st; fstp`）⇒
// 本条 x87 比较 lowering 不受 march 控制，非 P230 补所指的 fnstsw 族，封 N85。
// ②条件极性两形（`level < m_a` / `!(m_a <= level)`）实测同产 `fildl;flds;fucomip;
// fstp;seta`；ORIG 的 `fxch;fucompp;fnstsw %ax;sahf;setae` 无源形可达。
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
        // [laneE1 2026-09-11 P167 重载选错修复] ORIG 该点 call 目标是
        //   `_ZNSt6vectorI19DungeonPartyBalanceSaIS0_EE2atEj`（at(unsigned)），
        //   我方为 `…EEixEj`（operator[]）⇒ 源形须为 `.at(i)`（P175：ORIG 走
        //   size()+at() 索引形）。越界语义随 ORIG 恢复（at 抛 out_of_range）。
        const DungeonPartyBalance& bal = DUNGEON_PARTY_BALANCE_TABLE.at(i);
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
}