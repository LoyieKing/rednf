// ORIG T 0x085be9c8（无报告，objdump）：+0xcd8==1 / 无副本(+0xcac) / m_bloodDungeonType!=0 /
// m_villageAttackEventFlag==1 / isNoFatigueDungeon 任一命中 → false。
// [b28d-D43] ORIG 0x085be9c8：前两条件（bloodState==1 / 无副本）je 落
// **outer false**(+0x7b)，后三条件 jne/je 落 **inner false**(+0x6b) 再物化
// `test %al,%al; je outer` ⇒ 源码 = 两条件早返 false + 三条件 return 短路链
// （P0：无槽 = 无具名局部；旧 `bool baseOk` 合写令 4.4 落槽 -0x9 并撑帧）。
// 第三条件 `!m_villageAttackEventFlag`（ORIG xor$1 物化）而非 `!=1`（cmp 形）。
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
}