void CMonsterDropTest::processDungeonDrop(CParty* party, DropStat& drop,
                                          GradeStat& grade)
{
    switch (static_cast<unsigned int>(m_mode))
    {
    case 0:
        singleMapMonsterKill(party, drop, grade);
        break;
    case 2:
    case 3:
        passedMapMonsterKill(party, drop, grade);
        break;
    }
