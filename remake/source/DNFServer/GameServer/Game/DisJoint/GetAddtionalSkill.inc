void DisJoint::GetAddtionalSkill(int job, int level, int& outSkill,
                                 int& outLevel)
{
    DisjointSkillScript entry;
    for (std::vector<DisjointSkillScript>::size_type i = 0;
         i < m_skills.size(); ++i)
    {
        entry = m_skills[i];
        if (entry.m_field00 == job && entry.m_field04 == level)
        {
            outSkill = entry.m_field08;
            outLevel = entry.m_field0c;
            return;
        }
    }
    outSkill = 0;
    outLevel = 0;
