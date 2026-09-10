int SkillSlot::get_remain_sp_at_index(ENUM_SKILL_TREE_KIND kind) const
{
    if (kind == -1 || kind == 0)
    {
        return m_remainSP1;
    }
    if (kind == 1)
    {
        return m_remainSP2;
    }
    LogManager::logFormat(1, "skill_slot.cpp",
                          __PRETTY_FUNCTION__,
                          0x68, "Wrong Skill Tree Kind : %d", kind);
    return 0;
