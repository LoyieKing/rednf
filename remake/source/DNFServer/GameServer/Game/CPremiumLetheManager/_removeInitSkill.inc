void CPremiumLetheManager::_removeInitSkill(CUser* user,
                                            _Mastered_skill* skills)
{
    const _Mastered_skill* pSkill;  // getCurCharacSkillR ?? const?????? pSkill
    if (user->GetCurCharacSkillTreeIndex() == -1 ||
        user->GetCurCharacSkillTreeIndex() == 0)
    {
        pSkill = ((const SkillSlot*)user->getCurCharacSkillR())->m_skillSlot;
    }
    else
    {
        pSkill = ((const SkillSlot*)user->getCurCharacSkillR())->m_skillSlot2;
    }
    for (int i = 0; i <= 0xcb; ++i)
    {
        for (int j = 0; j <= 0xcb; ++j)
        {
            if (pSkill[i].m_index != 0 &&
                pSkill[i].m_index == skills[j].m_index)
            {
                skills[j].m_level =
                    (char)(skills[j].m_level - pSkill[i].m_level);
                if (skills[j].m_level < 1)
                {
                    skills[j].m_index = 0;
                    skills[j].m_level = 0;
                }
            }
        }
    }
}