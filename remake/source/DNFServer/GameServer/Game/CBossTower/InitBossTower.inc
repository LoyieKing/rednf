bool CBossTower::InitBossTower(CParty* party, int dungeonIdx)
{
    m_dungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (party == 0 || m_dungeon == 0)
        return 0;

    m_party = party;
    m_stageMax = m_dungeon->getRandomTowerMaxStage();
    m_prepareFinishFlag = 0;
    return 1;
