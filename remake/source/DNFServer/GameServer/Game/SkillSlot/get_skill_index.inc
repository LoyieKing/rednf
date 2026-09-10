unsigned int SkillSlot::get_skill_index(int slot) const
{
    if (m_parent == 0)
    {
        return 0xffffffff;
    }
    if (((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() == -1 ||
        ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() == 0)
    {
        return (unsigned char)m_skillSlot[slot].m_index;
    }
    return (unsigned char)m_skillSlot2[slot].m_index;
