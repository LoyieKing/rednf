int SkillSlot::get_remain_sfp_at_index(ENUM_SKILL_TREE_KIND kind) const
{
    if (kind == 2)
    {
        return m_remainSFP1;
    }
    if (kind == 3)
    {
        return m_remainSFP2;
    }
    LogManager::logFormat(1, "skill_slot.cpp",
                          __PRETTY_FUNCTION__,
                          0x81, "Wrong Skill Tree Kind : %d", kind);
    return 0;
}