int CSkill::get_guildSkillNeedGSP(int count) const
{
    if ((unsigned)m_guildSkillNeedGSP.size() < (unsigned)count)
    {
        return -1;
    }
    return m_guildSkillNeedGSP.at(count - 1);
