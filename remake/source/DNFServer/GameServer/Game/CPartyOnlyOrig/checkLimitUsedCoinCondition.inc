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
