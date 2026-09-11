void SkillSlot::initialize_sfp_skill()
{
    if (m_parent == 0)
    {
        return;
    }
    if (((CUserCharacInfo*)m_parent)->get_charac_level() <= 0x31)
    {
        return;
    }
    ((SkillSlot*)((CUserCharacInfo*)m_parent)->getCurCharacSkillW())
        ->clear_sfp_skills(
            (char)((CUserCharacInfo*)m_parent)->get_charac_job(),
            (ENUM_SKILL_TREE_KIND)0);
    ((SkillSlot*)((CUserCharacInfo*)m_parent)->getCurCharacSkillW())
        ->clear_sfp_skills(
            (char)((CUserCharacInfo*)m_parent)->get_charac_job(),
            (ENUM_SKILL_TREE_KIND)1);
    ((SkillSlot*)((CUserCharacInfo*)m_parent)->getCurCharacSkillW())
        ->set_remain_sfp_at_index(
            ((CUserCharacInfo*)m_parent)->get_charac_level() - 0x31,
            (ENUM_SKILL_TREE_KIND)2);
    ((SkillSlot*)((CUserCharacInfo*)m_parent)->getCurCharacSkillW())
        ->set_remain_sfp_at_index(
            ((CUserCharacInfo*)m_parent)->get_charac_level() - 0x31,
            (ENUM_SKILL_TREE_KIND)3);
}