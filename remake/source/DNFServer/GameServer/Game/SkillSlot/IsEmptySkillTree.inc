char SkillSlot::IsEmptySkillTree(ENUM_SKILL_TREE_KIND kind) const
{
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return 0;
    }
    char ret = 1;
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index)
        {
            ret = 0;
            break;
        }
    }
    return ret;
}