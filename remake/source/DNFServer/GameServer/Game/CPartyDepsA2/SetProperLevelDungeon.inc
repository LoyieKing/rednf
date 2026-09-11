// ============================================================================
// SetProperLevelDungeon —— ORIG T 0x085bb746（报告 CParty/SetProperLevelDungeon.md）
// [b28d-solo-CPartyDepsA2 判词·AE] 同 SetProperDungeonEvent：max/min 就地写回
// + 先声明后赋值、声明序按 ORIG 槽序（P198/P244）。
// ============================================================================
void CParty::SetProperLevelDungeon(CUser* user)
{
    user->ClearProperLevelDungeonUser();

    // [b28d-63] ORIG 0x085bb781：`xor $1; test; jne <nop 岛>` + 后段两独立
    // `jl/jg` 各落独立岛（0xe1/0xe4）⇒ P223 多岛形：负条件 goto + 尾标签串。
    if (!G_CDataManager()->m_conditionDungeon.isEventDungeon(
            m_battleField.m_pDungeon->get_index()))
        goto END1;
    {
        // [b28d-solo-CPartyDepsA2] 同 SetProperDungeonEvent：ORIG 槽序
        // standardLevel -0x10 / cap -0x14 / zero -0x18 / minLevel -0x1c /
        // upperBound -0x20 / level -0xc，max/min 就地写回 ⇒ 先声明后赋值。
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
            goto END2;
        if (level > upperBound)
            goto END3;
        user->SetProperLevelDungeonUser();
    }
END1:;
END2:;
END3:;
}