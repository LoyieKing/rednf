void CDeathTower::CPlayData::addPlayTime(unsigned int time)
{
    m_lastPlayTime = m_playTime;
    m_playTime += time;
}