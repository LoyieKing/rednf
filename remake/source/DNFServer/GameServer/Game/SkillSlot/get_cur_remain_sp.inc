int* SkillSlot::get_cur_remain_sp(ENUM_SKILL_TREE_KIND kind) const
{
    if (m_parent == 0)
    {
        return 0;
    }
    if (kind == -1 || kind == 0)
    {
        return const_cast<int*>(&m_remainSP1);
    }
    if (kind == 1)
    {
        return const_cast<int*>(&m_remainSP2);
    }
    if (kind == 2)
    {
        return const_cast<int*>(&m_remainSFP1);
    }
    if (kind == 3)
    {
        return const_cast<int*>(&m_remainSFP2);
    }
    return 0;
}