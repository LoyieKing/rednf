int StrikerScript::isCommonSkill(short firstGrowType, int skillIndex)
{
    std::vector<StrikerSkill>* vec =
        getStrikerSkillVector(firstGrowType, 0);
    if (vec == 0)
        return 0;
    for (std::vector<StrikerSkill>::iterator it = vec->begin();
         it != vec->end(); it++)
    {
        if (it->m_field00 == skillIndex)
            return 1;
    }
    return 0;
