bool CSkillList::get_post_learning_skill(
    int jobType, int skillIdx, std::vector<std::pair<int, int> >& out)
{
    std::map<int, std::vector<std::pair<int, int> > >::iterator it =
        m_postLearningMap[jobType].find(skillIdx);
    if (it != m_postLearningMap[jobType].end())
    {
        out = it->second;
        return true;
    }
    return false;
}