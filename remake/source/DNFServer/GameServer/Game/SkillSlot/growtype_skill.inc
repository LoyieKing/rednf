int SkillSlot::growtype_skill(int job, int skillIdx, int level,
                              ENUM_SKILL_TREE_KIND kind)
{
    CSkill* skill = NULL;
    skill = G_CDataManager()->find_skill(job, skillIdx);
    if (skill == 0)
    {
        LogManager::logFormat(
            1, "skill_slot.cpp",
            __PRETTY_FUNCTION__,
            0x20b, "G_CDataManager()->find_skill( %d, %d )", job, skillIdx);
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
    int group = skill->get_group();
    int slot = 0;
    if (level == 0)
    {
        slot = get_skillslot_no(skillIdx, group, kind, 1);
        if (slot != -1)
        {
            buf[slot].m_index = 0;
            buf[slot].m_level = (char)level;
        }
        if (m_parent != 0)
        {
            (&m_parent->m_historyLog)
                ->SkillDel(((CUserCharacInfo*)m_parent)->get_charac_job(),
                           ((CUserCharacInfo*)m_parent)
                               ->GetCurCharacSkillTreeIndex(),
                           skillIdx, 0, (eSkillDelReason)0);
        }
    }
    else
    {
        slot = get_skillslot_no(skillIdx, group, kind, 1);
        if (slot != -1)
        {
            buf[slot].m_level = (char)level;
        }
        else
        {
            slot = insert_skill(skillIdx, group, skill->is_active_skill(),
                                kind, 1);
            if (slot > -1)
            {
                buf[slot].m_level = (char)level;
            }
        }
        if (m_parent != 0)
        {
            (&m_parent->m_historyLog)
                ->SkillAdd(((CUserCharacInfo*)m_parent)->get_charac_job(),
                           ((CUserCharacInfo*)m_parent)
                               ->GetCurCharacSkillTreeIndex(),
                           skillIdx, level, (eSkillAddReason)1);
        }
    }
    return slot;
}