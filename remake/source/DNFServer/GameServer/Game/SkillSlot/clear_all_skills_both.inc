int SkillSlot::clear_all_skills_both()
{
    m_parent = 0;
    m_remainSP1 = 0;
    memset(m_skillSlot, 0, 0x198);
    m_remainSP2 = 0;
    memset(m_skillSlot2, 0, 0x198);
    m_isLoadStrikerSkill = 0;
    return 1;
