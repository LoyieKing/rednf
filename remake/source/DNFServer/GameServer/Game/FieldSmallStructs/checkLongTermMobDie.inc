int stMapMonsterKillChecker_t::checkLongTermMobDie(unsigned int tick,
                                                   unsigned int limit)
{
    if (m_field10 != 0 && tick - m_field10 >= limit)
        ++m_dieCnt;
    return m_dieCnt;
