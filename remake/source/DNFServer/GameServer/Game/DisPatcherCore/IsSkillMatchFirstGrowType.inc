// ORIG 0x082865ca：for i in 0..5: skill->m_fitGrowType[i](+0xcc+4i) == type → true
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
}
