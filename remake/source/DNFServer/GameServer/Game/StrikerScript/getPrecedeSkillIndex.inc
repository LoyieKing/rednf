int StrikerScript::getPrecedeSkillIndex(short firstGrowType, short secondGrowType,
                                        int skillIndex)
{
    std::vector<StrikerSkill>* vec = 0;
    vec = getStrikerSkillVector(firstGrowType, secondGrowType);
    if (vec == 0)
        return 0;
    for (unsigned int i = 0; i < vec->size(); ++i)
    {
        if ((*vec)[i].m_field00 == skillIndex)
            return (*vec)[i].m_field08;
    }
    return 0;
