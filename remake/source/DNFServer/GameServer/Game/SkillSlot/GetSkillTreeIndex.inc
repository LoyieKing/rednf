int SkillSlot::GetSkillTreeIndex() const
{
    if (m_parent == 0)
    {
        return -1;
    }
    return ((CUserCharacInfo*)m_parent)->GetCurCharacSkillTreeIndex();
