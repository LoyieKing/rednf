int stMapMonsterKillChecker_t::dieMonster(unsigned int tick, unsigned int limit)
{
    // ORIG 0x830e74e..0x830e78a 逐条核对：m_lastMobDieTime=tick 仅在超限
    // 重置分支内（0x830e77d），cnt++ 分支 fall 后直 return；<= 极性使 4.4
    // 复现 cnt++ fall / 重置块后置的块序。
    if (tick - m_lastMobDieTime <= limit)
    {
        m_uncontinuallyMobDieCnt++;
    }
    else
    {
        m_uncontinuallyMobDieCnt = 0;
        m_lastMobDieTime = tick;
    }
    return m_uncontinuallyMobDieCnt;
