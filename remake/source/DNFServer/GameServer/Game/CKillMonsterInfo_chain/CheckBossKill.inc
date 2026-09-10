bool CKillMonsterInfo::CheckBossKill(unsigned int mobIdx, unsigned char diff)
{
    return CheckMonsterKill(m_bossKills, mobIdx, diff);
