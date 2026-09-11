bool CKillMonsterInfo::RegisterNamedKill(unsigned int mobIdx, unsigned char diff)
{
    return RegisterMonsterKill(m_namedKills, mobIdx, diff, false);
}