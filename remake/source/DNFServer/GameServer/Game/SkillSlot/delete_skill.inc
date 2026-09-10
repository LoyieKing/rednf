int SkillSlot::delete_skill(int skillIdx, int job,
                            ENUM_SKILL_TREE_KIND kind)
{
    CSkill* skill = NULL;
    skill = G_CDataManager()->find_skill(skillIdx, job);
    if (skill == 0)
    {
        return -1;
    }
    if (m_parent == 0)
    {
        return -1;
    }
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return -1;
    }
    for (int i = 0; i < 204; ++i)
    {
        if (buf[i].m_index == skill->m_index)
        {
            buf[i].m_index = 0;
            buf[i].m_level = 0;
            break;
        }
    }
    return 0;
