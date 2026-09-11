bool CDeathTower::CDungeonMgr::initDungeonMgr(int dungeonIdx)
{
    // ORIG 0x084605a2：m_pDungeon = G_CDataManager()->find_dungeon(dungeonIdx)；
    m_pDungeon = G_CDataManager()->find_dungeon(dungeonIdx);
    if (m_pDungeon == 0)
        return false;
    // [b28d-D13] P0 清绑（N96）：ORIG 0x084605e0 mov %eax,%edx 为 4.4 对
    // getDeathTowerMaxStage() 返回值存 int 字段的自然中转，直接赋值即复现。
    m_endStage = m_pDungeon->getDeathTowerMaxStage();
    return true;
}