int CSkill::get_guildSkillNeedGold(int count) const
{
    if ((unsigned)m_guildSkillNeedGold.size() < (unsigned)count)
    {
        return -1;
    }
    return m_guildSkillNeedGold.at(count - 1);
