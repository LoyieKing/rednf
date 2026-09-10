void SkillSlot::set_skill_slot_at_index(char* in, int size,
                                        ENUM_SKILL_TREE_KIND kind)
{
    if (0x198 < (unsigned)size)
    {
        size = 0x198;
    }
    switch (kind)
    {
    case -1:
    case 0:
        memcpy(m_skillSlot, in, size);
        break;
    case 1:
        memcpy(m_skillSlot2, in, size);
        break;
    default:
        break;
    }
