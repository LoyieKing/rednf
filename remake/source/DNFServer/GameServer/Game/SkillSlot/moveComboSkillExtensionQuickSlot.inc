bool SkillSlot::moveComboSkillExtensionQuickSlot(int skillIdx, int level,
                                                ENUM_SKILL_TREE_KIND kind)
{
    if (m_parent == 0)
    {
        return 0;
    }
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return 0;
    }
    CSkill* skill = G_CDataManager()->find_skill(
        ((CUserCharacInfo*)m_parent)->get_charac_job(), skillIdx);
    if (skill == 0)
    {
        return 0;
    }
    int group = skill->get_group();
    int slot = get_skillslot_no(skillIdx, group, kind, 0);
    if (slot != -1)
    {
        if (buf[slot].m_level < level)
        {
            buf[slot].m_level = (char)level;
        }
    }
    else
    {
        slot = insert_skill(skillIdx, group, 0, kind, 1);
        if (slot >= 6 && slot <= 0xc5)
        {
            buf[slot].m_level = (char)level;
        }
        else
        {
            LogManager::logFormat(
                1, "skill_slot.cpp",
                __PRETTY_FUNCTION__,
                0xcf9,
                "moveComboSkillExtensionQuickSlot Error slot(%d) index(%d) level(%d)kind(%d) characNo(%d)",
                slot, (unsigned char)buf[slot].m_index,
                (int)(char)buf[slot].m_level, kind,
                m_parent->get_charac_no(-1));
        }
    }
    return 1;
