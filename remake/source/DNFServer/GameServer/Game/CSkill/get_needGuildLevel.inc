int CSkill::get_needGuildLevel(int count) const
{
    return m_needGuildLevel + m_needGuildLevelAdd * (count - 1);
