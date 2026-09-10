unsigned int CDeathTower::stMapMonsterKillChecker_t::dieMonster(unsigned int tick, unsigned int limit)
{
    // ORIG 0x08469c96：
    //   无符号比较 (tick - m_lastMobDieTime) <= limit → ++m_dieCnt；
    //   否则 m_dieCnt = 0; m_lastMobDieTime = tick；返回 m_dieCnt。
    if ((unsigned int)(tick - m_lastMobDieTime) <= limit)
    {
        ++m_dieCnt;
    }
    else
    {
        m_dieCnt = 0;
        m_lastMobDieTime = tick;
    }
    return m_dieCnt;
