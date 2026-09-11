int SkillSlot::check_skill_mastered(_Mastered_skill* buf, int skillIdx,
                                    int level, int group) const
{
    if (m_parent == 0)
    {
        return 0;
    }
    if (buf == 0)
    {
        return 0;
    }
    int slot = get_skillslot_no(buf, skillIdx, group, 1);
    if (slot < 0)
    {
        return 0;
    }
    if ((unsigned char)buf[slot].m_index == skillIdx &&
        level <= buf[slot].m_level)
    {
        return 1;
    }
    return 0;
}