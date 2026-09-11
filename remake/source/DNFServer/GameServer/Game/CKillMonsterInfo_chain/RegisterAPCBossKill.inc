bool CKillMonsterInfo::RegisterAPCBossKill(unsigned int mobIdx,
                                           unsigned char diff)
{
    return RegisterMonsterKill(m_apcBossKills, mobIdx, diff, true);
}