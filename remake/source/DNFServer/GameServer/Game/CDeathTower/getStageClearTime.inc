int CDeathTower::CStage::getStageClearTime()
{
    return m_finishTick - m_startTick;
}