CMap* CDeathTower::CDungeonMgr::getStageMap(unsigned int stage)
{
    // ORIG 0x84605ee：mapIdx = m_pDungeon->getDeathTowerMapIndex(stage)；
    // map = G_CDataManager()->find_map(mapIdx)；为空返回 0，否则返回 map。
    int mapIdx = m_pDungeon->getDeathTowerMapIndex(stage);
    CMap* map = (CMap*)G_CDataManager()->find_map(mapIdx);
    if (map == 0)
        return 0;
    return map;
}