bool CKillMonsterInfo::RegisterBossKill(unsigned int mobIdx, unsigned char diff)
{
    return RegisterMonsterKill(m_bossKills, mobIdx, diff, false);
}