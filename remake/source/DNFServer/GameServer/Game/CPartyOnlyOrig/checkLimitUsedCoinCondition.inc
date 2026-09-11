// ORIG T 0x085be56c（checkLimitUsedCoinCondition.md）：party/dungeon 双层
// 硬币限额校验；超限返回 0x16、找不到副本返回 0x13、通过返回 0。
// 形参直写回 0xc(%ebp)（ORIG +0x34 `mov %eax,0xc(%ebp)`：find_dungeon 结果
// 直接覆写形参槽，无本地别名副本）；diff 以内联实参入 getLimitCoinDiff。
// b28d-solo-CPartyOnlyOrig 判词：**AE** @0x085be56c（clean 口径 ndiff=0，102:102）。
// 修法：P225 拆嵌套 if 复现 4.1 每条件 `setne/setge %al; test %al,%al; je`
// 物化（合并 `&&` 在 4.4 恒折 jl/jne 直跳，探针 p1..p4）+ Main 代落
// CDungeon::getLimitCoinDiff int→bool（ORIG `test %al`）+ 本 TU
// CheckHellCoinLimint int→bool（ORIG `call; xor $1; test %al`）。
int CParty::checkLimitUsedCoinCondition(CDungeon const* dungeon)
{
    if (dungeon == 0)
    {
        int dungeonIdx = reinterpret_cast<CBattle_Field*>(&m_battleField)
                             ->get_dungeon_index();
        dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
        if (dungeon == 0)
        {
            return 0x13;
        }
    }
    // b28d-solo-CPartyOnlyOrig：P225 拆 && —— ORIG 每条件 `setne/setge %al;
    // test %al,%al; je` 物化；4.4 对合并 `&&` 恒折 jl/jne 直跳（探针 p1/p4），
    // 拆嵌套 if 后逐条同形（探针 p2/p1v）。
    if (GetPartyMemberCoinLimit() != 0)
    {
        if (GetUsedCoinCount() >= GetPartyMemberCoinLimit())
        {
            return 0x16;
        }
    }

    int limitCoin = 0;
    if (dungeon->getLimitCoinDiff(
            reinterpret_cast<CBattle_Field*>(&m_battleField)
                ->get_dungeon_diff(),
            limitCoin))
    {
        if (limitCoin != -1)
        {
            if (GetUsedCoinCount() >= limitCoin)
            {
                return 0x16;
            }
        }
    }
    if (dungeon->m_87c != -1)
    {
        if (GetUsedCoinCount() >= dungeon->m_87c)
        {
            return 0x16;
        }
    }
    if (dungeon->m_898 != -1)
    {
        if (CheckHellCoinLimint(dungeon->m_898) != 1)
        {
            return 0x16;
        }
    }
    return 0;
}