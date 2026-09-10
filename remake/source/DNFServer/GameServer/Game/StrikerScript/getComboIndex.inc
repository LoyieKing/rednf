int StrikerScript::getComboIndex(short firstGrowType, short secondGrowType,
                                 int skillIndex)
{
    std::vector<StrikerSkill>* vec = 0;
    for (int i = 0; i <= 1; ++i)
    {
        if (i == 0)
            vec = getStrikerSkillVector(firstGrowType, secondGrowType);
        else
            vec = getStrikerSkillVector(firstGrowType, 0);
        if (vec == 0)
            return 0;
        for (std::vector<StrikerSkill>::iterator it = vec->begin();
             it != vec->end(); it++)
        {
            if (it->m_field00 == skillIndex)
                return it->m_field04;
        }
    }
    return 0;
