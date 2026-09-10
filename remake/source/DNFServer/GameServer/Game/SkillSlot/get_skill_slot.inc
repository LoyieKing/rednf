void SkillSlot::get_skill_slot(char* out1, char* out2, int size) const
{
    if (0x198 < (unsigned)size)
    {
        size = 0x198;
    }
    memcpy(out1, m_skillSlot, size);
    memcpy(out2, m_skillSlot2, size);
