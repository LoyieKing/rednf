int SkillSlot::insert_skill(int skillIdx, int group, bool isActive,
                            ENUM_SKILL_TREE_KIND kind, int level)
{
    if (m_parent == 0)
    {
        return -1;
    }
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return -1;
    }
    int slot = get_skillslot_no(0, group, kind, isActive);
    if (slot != -1)
    {
        buf[slot].m_index = (unsigned char)skillIdx;
        buf[slot].m_level = (char)level;
    }
    return slot;
}