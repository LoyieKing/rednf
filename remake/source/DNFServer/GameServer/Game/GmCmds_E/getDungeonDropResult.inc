void CMonsterDropTest::getDungeonDropResult(std::list<map_item>& items, DropStat& drop,
                                            GradeStat& grade)
{
    switch (m_mode)
    {
    case 2:
    case 0:
        makeGeneralItemDropList(items, drop);
        makeUpgradeDropList(items, grade);
        break;
    case 3:
        makeGoldItemDropList(items, drop);
        break;
    }
