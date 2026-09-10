int SkillSlot::FindSkill(int skillIdx) const
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
    for (int i = 0; i < 204; ++i)
    {
        if ((unsigned char)buf[i].m_index == skillIdx)
        {
            return (int)(char)m_skillSlot[i].m_level;
        }
    }
    return 0;
