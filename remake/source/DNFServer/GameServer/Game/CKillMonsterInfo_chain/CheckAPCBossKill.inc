bool CKillMonsterInfo::CheckAPCBossKill(unsigned int mobIdx, unsigned char diff)
{
    return CheckMonsterKill(m_apcBossKills, mobIdx, diff);
