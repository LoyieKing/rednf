void SkillSlot::set_remain_sp_at_index(int value, ENUM_SKILL_TREE_KIND kind)
{
    if (kind == -1 || kind == 0)
    {
        m_remainSP1 = value;
    }
    else if (kind == 1)
    {
        m_remainSP2 = value;
    }
}