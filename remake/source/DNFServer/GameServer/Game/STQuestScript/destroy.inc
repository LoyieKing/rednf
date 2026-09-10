void QuestScript::destroy()
{
    for (int i = 0; i < 0xb; ++i)
    {
        if (m_jobNames[i])
        {
            delete m_jobNames[i];
        }
        m_jobNames[i] = 0;
        if (m_jobRequests[i])
        {
            delete m_jobRequests[i];
        }
        m_jobRequests[i] = 0;
        if (m_jobConditions[i])
        {
            delete m_jobConditions[i];
        }
        m_jobConditions[i] = 0;
        if (m_jobSolves[i])
        {
            delete m_jobSolves[i];
        }
        m_jobSolves[i] = 0;
    }
