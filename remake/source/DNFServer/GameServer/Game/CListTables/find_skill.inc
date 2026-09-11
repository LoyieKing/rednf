CSkill* CSkillList::find_skill(int jobType, int skillIdx)
{
    if (jobType > 10)
    {
        return 0;
    }
    __gnu_cxx::hash_map<int, CSkill*>::iterator it;
    it = m_skillList[jobType].find(skillIdx);
    if (it == m_skillList[jobType].end())
    {
        return 0;
    }
    return it->second;
}