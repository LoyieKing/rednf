void CDungeonClearTracer::Trace(const char* name)
{
    if (IsKilledBoss())
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x108, 0)(
            "FUNC(%s) Map ID(%d), Boss(%d/%d), Monster(%d/%d) Cleared(%d), "
            "End Point(%d)\n",
            name, m_mapId, m_killedBossCount, m_maxBossCount,
            m_killedMonsterCount, m_maxMonsterCount, m_clearedDungeon,
            m_endPoint);
    }
    Reset();
