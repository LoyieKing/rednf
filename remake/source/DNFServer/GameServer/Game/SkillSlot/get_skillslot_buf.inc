_Mastered_skill* SkillSlot::get_skillslot_buf(ENUM_SKILL_TREE_KIND kind) const
{
    if (kind == -1 || kind == 0)
    {
        return const_cast<_Mastered_skill*>(m_skillSlot);
    }
    if (kind == 1)
    {
        return const_cast<_Mastered_skill*>(m_skillSlot2);
    }
    return 0;
}