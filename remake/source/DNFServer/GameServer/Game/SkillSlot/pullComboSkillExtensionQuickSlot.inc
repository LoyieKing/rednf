void SkillSlot::pullComboSkillExtensionQuickSlot(ENUM_SKILL_TREE_KIND kind)
{
    if (m_parent == 0)
    {
        return;
    }
    _Mastered_skill* buf = get_skillslot_buf(kind);
    if (buf == 0)
    {
        return;
    }
    for (int i = 0xc6; i < 0xcc; ++i)
    {
        if (buf[i].m_index == 0)
        {
            continue;
        }
        if (moveComboSkillExtensionQuickSlot(
                buf[i].m_index, (int)(char)buf[i].m_level, kind))
        {
            buf[i].m_index = 0;
            buf[i].m_level = 0;
        }
        else
        {
            LogManager::logFormat(
                1, "skill_slot.cpp",
                __PRETTY_FUNCTION__,
                0xcd7,
                "pullComboSkillExtensionQuickSlot Error slot(%d) index(%d) level(%d)kind(%d) characNo(%d)",
                i, (unsigned char)buf[i].m_index,
                (int)(char)buf[i].m_level, kind,
                m_parent->get_charac_no(-1));
        }
    }
