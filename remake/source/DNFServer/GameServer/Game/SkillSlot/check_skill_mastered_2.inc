// [G17 2026-09-11 类型轴同步] 返回类型 int→bool（与 SkillSlot.h:267 声明一致；
//   ORIG 调用点 0x0860575x 段为 `call; xor $1; test; je` = bool 形）。
bool SkillSlot::check_skill_mastered(_Mastered_skill* buf, int skillIdx,
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