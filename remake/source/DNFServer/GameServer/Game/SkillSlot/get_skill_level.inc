int SkillSlot::get_skill_level(int slot) const
{
    if (m_parent == 0)
    {
        return -1;
    }
    if (((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() == -1 ||
        ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex() == 0)
    {
        return m_skillSlot[slot].m_level;
    }
    return m_skillSlot2[slot].m_level;
}