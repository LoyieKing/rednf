void SkillSlot::set_remain_sfp_at_index(int value, ENUM_SKILL_TREE_KIND kind)
{
    if (kind == 2)
    {
        m_remainSFP1 = value;
    }
    else if (kind == 3)
    {
        m_remainSFP2 = value;
    }
    else
    {
        LogManager::logFormat(1, "skill_slot.cpp",
                              __PRETTY_FUNCTION__,
                              0x75, "Wrong Skill Tree Kind : %d", kind);
    }
