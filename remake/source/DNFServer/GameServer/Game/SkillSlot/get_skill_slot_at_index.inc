void SkillSlot::get_skill_slot_at_index(char* out, int size,
                                        ENUM_SKILL_TREE_KIND kind) const
{
    if (0x198 < (unsigned)size)
    {
        size = 0x198;
    }
    if (kind == -1 || kind == 0)
    {
        memcpy(out, m_skillSlot, size);
    }
    else if (kind == 1)
    {
        memcpy(out, m_skillSlot2, size);
    }
