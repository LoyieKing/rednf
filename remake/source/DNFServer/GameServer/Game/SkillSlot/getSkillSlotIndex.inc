int SkillSlot::getSkillSlotIndex(ENUM_SKILL_TREE_KIND kind, int slot) const
{
    if (kind > 1)
    {
        LogManager::logFormat(1, "skill_slot.cpp",
                              __PRETTY_FUNCTION__,
                              0xd1f, "SkillSlot::getSkillSlotIndex kind(%d) error",
                              kind);
        return 0xffffffff;
    }
    if (kind == -1 || kind == 0)
    {
        return (unsigned char)m_skillSlot[slot].m_index;
    }
    return (unsigned char)m_skillSlot2[slot].m_index;
}