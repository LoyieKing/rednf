int SkillSlot::clear_all_skills(ENUM_SKILL_TREE_KIND kind)
{
    _Mastered_skill* buf = get_skillslot_buf(kind);
    int* remain = get_cur_remain_sp(kind);
    if (buf == 0 || remain == 0)
    {
        return 0;
    }
    m_parent = 0;
    *remain = 0;
    memset(buf, 0, 0x198);
    return 1;
