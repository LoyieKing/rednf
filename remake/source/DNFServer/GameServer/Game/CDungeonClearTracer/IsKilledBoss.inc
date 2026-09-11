bool CDungeonClearTracer::IsKilledBoss()
{
    if (m_killedBossCount > 0 && m_killedBossCount >= m_maxBossCount)
    {
        return true;
    }
    return false;
}