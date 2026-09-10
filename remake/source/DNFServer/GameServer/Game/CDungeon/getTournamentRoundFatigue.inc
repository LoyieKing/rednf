int CDungeon::getTournamentRoundFatigue() const
{
    // ORIG T 0x08365238：返回 m_tournamentRoundFatigue（+0x8a4）；0 兜底返回 1。
    if (m_tournamentRoundFatigue == 0)
        return 1;
    return m_tournamentRoundFatigue;
