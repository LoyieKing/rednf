bool PvPSkillTreeParameterScript::getPvPSkillTree(
    int job, int firstGrow, int secondGrow, int grade, bool flag,
    std::vector<std::pair<int, int> >** out) const
{
    PvPSkillTreeKey key(static_cast<unsigned char>(job),
                        static_cast<unsigned char>(firstGrow),
                        static_cast<unsigned char>(secondGrow));

    std::map<PvPSkillTreeKey,
             std::map<int, PvPSkillTreeValue> >::const_iterator treeIt =
        m_map0.find(key);
    if (treeIt != m_map0.end())
    {
        std::map<int, PvPSkillTreeValue>::const_iterator gradeIt =
            treeIt->second.find(grade);
        if (gradeIt != treeIt->second.end())
        {
            if (flag)
                *out = const_cast<std::vector<std::pair<int, int> >*>(
                    &gradeIt->second.m_vec0c);
            else
                *out = const_cast<std::vector<std::pair<int, int> >*>(
                    &gradeIt->second.m_vec00);
            return true;
        }
    }
    return false;
