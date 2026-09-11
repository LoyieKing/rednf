int SkillSlot::insert_skill_with_slot_index(int slot, int skillIdx,
                                            bool isActive, int level)
{
    if (slot < 0)
    {
        return -4;
    }
    if (m_parent == 0)
    {
        return -1;
    }
    _Mastered_skill* buf = get_skillslot_buf(
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex());
    if (buf == 0)
    {
        return -1;
    }
    buf[slot].m_index = (unsigned char)slot;
    buf[slot].m_level = 1;
    return slot;
}