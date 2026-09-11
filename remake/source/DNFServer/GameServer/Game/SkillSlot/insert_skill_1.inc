int SkillSlot::insert_skill(_Mastered_skill* buf, int skillIdx, int group,
                            bool isActive, int level)
{
    if (m_parent == 0)
    {
        return -1;
    }
    if (buf == 0)
    {
        return -1;
    }
    int slot = get_skillslot_no(buf, 0, group, isActive);
    if (slot != -1)
    {
        buf[slot].m_index = (unsigned char)skillIdx;
        buf[slot].m_level = (char)level;
    }
    return slot;
}