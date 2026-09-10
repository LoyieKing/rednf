std::string& QuestScript::getRequestMessage(ENUM_CHARACTERJOB job)
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobRequests[job])
    {
        return *m_jobRequests[job];
    }
    return m_requestDefault;
