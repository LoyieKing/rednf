void SkillSlot::debugCheckGrowTypeSkill(int level, int job, int firstGrow,
                                        int secondGrow)
{
    for (int i = 0; i <= 203; ++i)
    {
        if (m_skillSlot[i].m_index != 0)
        {
            CSkill* skill = NULL;
            skill = G_CDataManager()->find_skill(job, m_skillSlot[i].m_index);
            if (skill == 0)
            {
                LogManager::logFormat(
                    1, "skill_slot.cpp",
                    __PRETTY_FUNCTION__,
                    0x5a4, "G_CDataManager()->find_skill( %d, %d )", job,
                    (unsigned char)m_skillSlot[i].m_index);
                return;
            }
            if (!skill->can_learn(level, firstGrow, secondGrow,
                                  (int)(char)m_skillSlot[i].m_level))
            {
                m_skillSlot[i].m_index = 0;
                m_skillSlot[i].m_level = 0;
            }
        }
        if (m_skillSlot2[i].m_index != 0)
        {
            CSkill* skill = NULL;
            skill = G_CDataManager()->find_skill(job, m_skillSlot2[i].m_index);
            if (skill == 0)
            {
                LogManager::logFormat(
                    1, "skill_slot.cpp",
                    __PRETTY_FUNCTION__,
                    0x5b7, "G_CDataManager()->find_skill( %d, %d )", job,
                    (unsigned char)m_skillSlot2[i].m_index);
                return;
            }
            if (!skill->can_learn(level, firstGrow, secondGrow,
                                  (int)(char)m_skillSlot2[i].m_level))
            {
                m_skillSlot2[i].m_index = 0;
                m_skillSlot2[i].m_level = 0;
            }
        }
    }
