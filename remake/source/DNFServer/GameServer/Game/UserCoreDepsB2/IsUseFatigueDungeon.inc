bool CParty::IsUseFatigueDungeon()
{
    // [b28d-D43] P222/P214①：ORIG 两守卫 `je outer_false`（同落尾部）+ 内层三条件
    // 物化 ⇒ 正向包裹形（false 归共享尾）；拆两条独立守卫 4.4 反产各自
    // `jne+mov$0+jmp` 岛（43 条），合并 `||` 产 41 条，本形为最优候选。
    if (m_battleField.m_bloodState != 1 && m_battleField.m_pDungeon != 0)
    {
        if (m_battleField.m_pDungeon->m_bloodDungeonType == 0 &&
            !m_battleField.m_pDungeon->m_villageAttackEventFlag &&
            !m_battleField.m_pDungeon->isNoFatigueDungeon())
            return true;
    }
    return false;
