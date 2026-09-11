// [b28d-solo-SkillSlot] int→bool（头声明同步）：ORIG check_skill_list 调用点
// 0x08604d2a `call; xor $0x1,%eax; test %al,%al; je` = P292/P248 档①。
bool SkillSlot::check_skill_mastered(int skillIdx, int level, int group) const
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
    int slot = get_skillslot_no(
        skillIdx, group,
        (ENUM_SKILL_TREE_KIND)((CUserCharacInfo*)m_parent)
            ->GetCurCharacSkillTreeIndex(),
        1);
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