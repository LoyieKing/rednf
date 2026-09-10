void stMapMonsterKillChecker_t::setUncontinuallyMobDieCnt(unsigned int cnt)
{
    m_dieCnt = cnt;   // 字段已 unsigned（WarField.h），同型赋值保 ORIG store 展开序
