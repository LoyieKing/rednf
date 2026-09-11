void SkillSlot::set_skill_slot(char* in1, char* in2, int size)
{
    if (0x198 < (unsigned)size)
    {
        size = 0x198;
    }
    memcpy(m_skillSlot, in1, size);
    memcpy(m_skillSlot2, in2, size);
}