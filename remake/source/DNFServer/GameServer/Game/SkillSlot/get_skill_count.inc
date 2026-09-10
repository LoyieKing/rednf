int SkillSlot::get_skill_count() const
{
    if (m_parent == 0)
    {
        return 0;
    }
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return 0;
    }
    int count = 0;
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index)
        {
            count = count + 1;
        }
    }
    return count;
