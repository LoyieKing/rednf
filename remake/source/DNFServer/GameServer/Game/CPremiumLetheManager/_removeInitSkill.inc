void CPremiumLetheManager::_removeInitSkill(CUser* user,
                                            _Mastered_skill* skills)
{
    _Mastered_skill* pSkill;
    if (user->GetCurCharacSkillTreeIndex() == -1 ||
        user->GetCurCharacSkillTreeIndex() == 0)
    {
        pSkill = (_Mastered_skill*)((char*)user->getCurCharacSkillR() + 0x46);
    }
    else
    {
        pSkill = (_Mastered_skill*)((char*)user->getCurCharacSkillR() + 0x1de);
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
