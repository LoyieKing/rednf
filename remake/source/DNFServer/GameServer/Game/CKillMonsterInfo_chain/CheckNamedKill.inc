bool CKillMonsterInfo::CheckNamedKill(unsigned int mobIdx, unsigned char diff)
{
    return CheckMonsterKill(m_namedKills, mobIdx, diff);
}