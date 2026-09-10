int SkillSlot::FindSkill(int skillIdx, ENUM_SKILL_TREE_KIND kind) const
{
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return 0;
    }
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index == skillIdx)
        {
            return buf[i].m_level;
        }
    }
    return 0;
