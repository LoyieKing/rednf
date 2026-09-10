__gnu_cxx::hash_map<int, CSkill*>* CSkillList::getSkillList(int jobType)
{
    if (jobType > 10)
    {
        return 0;
    }
    return &m_skillList[jobType];
