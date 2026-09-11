void CDeathTower::reset()
{
    m_dungeonMgr.reset();
    m_stage.reset();
    m_playData.reset();
    m_party = 0;
    m_towerState = TOWER_STATE_NONE;
}