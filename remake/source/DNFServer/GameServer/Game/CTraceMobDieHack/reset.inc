void CTraceMobDieHack::reset()
{
    m_map1.clear();
    m_traceBeginTime = 0;
    m_mobKillers.clear();
    m_hackLogs.clear();
    m_bTracing = false;
    m_dungeonDiff = 5;
    for (int i = 0; i <= 3; ++i)
    {
        m_records[i].m_user = 0;
        m_records[i].m_damage = 0;
        m_records[i].m_count = 0;
    }
}