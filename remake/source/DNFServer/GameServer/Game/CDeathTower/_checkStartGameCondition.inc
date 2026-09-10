int CDeathTower::_checkStartGameCondition(unsigned char& flag)
{
    Inven_Item items[4];
    int params[4];
    memset(items, 0, sizeof(items));
    memset(params, 0, sizeof(params));

    // [b28d-solo-CDeathTower] ORIG 0x0846392d/0x0846396b：bool callee 负极性早返
    // （P292：call; xor $1; test %al; je）；P0：getDungeon 结果内联直入实参槽。
    if (!m_party->CheckEnterAncientDungeon(m_dungeonMgr.getDungeon(), flag,
                                           items, params))
    {
        return 0x11;
    }
    if (!m_party->UseAncientDungeonItems(m_dungeonMgr.getDungeon(), items,
                                         params))
    {
        return 1;
    }
    return 0;
