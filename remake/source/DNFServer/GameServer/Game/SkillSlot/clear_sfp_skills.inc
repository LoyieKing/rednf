bool SkillSlot::clear_sfp_skills(char job, ENUM_SKILL_TREE_KIND kind)
{
    CSkill* skill = NULL;
    for (int i = 0; i < 204; ++i)
    {
        if (kind == -1 || kind == 0)
        {
            skill = G_CDataManager()->find_skill(job, m_skillSlot[i].m_index);
            if (skill == 0)
            {
                continue;
            }
            if (skill->IsSpecialSkill())
            {
                m_skillSlot[i].m_index = 0;
                m_skillSlot[i].m_level = 0;
            }
        }
        else if (kind == 1)
        {
            skill = G_CDataManager()->find_skill(job, m_skillSlot2[i].m_index);
            if (skill == 0)
            {
                continue;
            }
            if (skill->IsSpecialSkill())
            {
                m_skillSlot2[i].m_index = 0;
                m_skillSlot2[i].m_level = 0;
            }
        }
        else
        {
            LogManager::logFormat(
                1, "skill_slot.cpp",
                __PRETTY_FUNCTION__,
                0x49c, "SkillSlot::clear_sfp_skills error %d", kind);
        }
    }
    return 1;
