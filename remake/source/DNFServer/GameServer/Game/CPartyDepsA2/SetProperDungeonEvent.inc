// ============================================================================
// SetProperDungeonEvent —— ORIG T 0x085bb62a（报告 CParty/SetProperDungeonEvent.md）
// [b28d-solo-CPartyDepsA2 判词·AE] max/min 结果就地写回 minLevel/upperBound
// （P0：4.1 槽复用即源码复用）+ 先声明后赋值、声明序按 ORIG 槽序
// standardLevel/cap/zero/minLevel/upperBound（P198/P244），lower/upper 具名局部删除。
// ============================================================================
void CParty::SetProperDungeonEvent(CUser* user, int param)
{
    // [b28d-63] ORIG 0x085bb62a：两 bool 取反守卫（xor$1/test/jne）+ 两个
    // jl/jg 各落独立 nop 岛（0x736/0x739/0x73c/0x73f）⇒ P223 多岛形；
    // dungeon 取 **this->m_battleField.m_pDungeon**（ORIG `0x8(%ebp)+0xcac`），
    // user 经 CUser 基类接口直调（ORIG `0xc(%ebp)` 三次）；param 未被 ORIG 使用。
    user->ClearProperDungeonUser();

    if (!G_CDataManager()->m_conditionDungeon.isEventDungeon(
            m_battleField.m_pDungeon->get_index()))
        goto END1;
    if (!G_CDataManager()->m_conditionDungeon.isApplyLevel(
            user->get_charac_level()))
        goto END2;
    {
        // [b28d-solo-CPartyDepsA2] ORIG 槽序（浅→深）standardLevel -0x10、
        // cap -0x14、zero -0x18、minLevel -0x1c、upperBound -0x20、level -0xc；
        // max/min 结果就地写回 minLevel/upperBound 槽 ⇒ 先声明后赋值 +
        // 声明序按 ORIG 槽序（P198 孪生/P244 具名局部判据）。
        int standardLevel;
        int cap;
        int zero;
        int minLevel;
        int upperBound;
        int level;

        standardLevel = m_battleField.m_pDungeon->get_standard_level();
        minLevel = m_battleField.m_pDungeon->get_min_level();
        zero = 0;
        minLevel = std::max(minLevel, zero);
        upperBound = standardLevel + 1;
        cap = 0x46;
        upperBound = std::min(upperBound, cap);
        level = user->get_charac_level();
        if (level < minLevel)
            goto END3;
        if (level > upperBound)
            goto END4;
        user->SetProperDungeonUser();
    }
END1:;
END2:;
END3:;
END4:;
}