std::string& QuestScript::getName(ENUM_CHARACTERJOB job)
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobNames[job])
    {
        return *m_jobNames[job];
    }
    return m_name;
}