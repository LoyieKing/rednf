void CDeathTower::CPlayData::resetMemberReady()
{
    m_allReady = 0;
    for (int i = 0; i < 4; ++i)
    {
        m_memberReady[i] = 0;
    }
}