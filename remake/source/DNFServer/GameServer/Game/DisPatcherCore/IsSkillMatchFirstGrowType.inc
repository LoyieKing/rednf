bool CHackLog_InvalidUsingSkill::IsSkillMatchFirstGrowType(const CSkill* skill, char type)
{
    for (int i = 0; i <= 5; ++i)
    {
        if (skill->m_fitGrowType[i] == (int)type)
        {
            return true;
        }
    }
    return false;
