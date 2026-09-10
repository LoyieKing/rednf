std::string& QuestScript::getSolveMessage(ENUM_CHARACTERJOB job)
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobSolves[job])
    {
        return *m_jobSolves[job];
    }
    return m_solveDefault;
