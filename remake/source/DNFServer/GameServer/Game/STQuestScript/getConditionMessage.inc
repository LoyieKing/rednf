std::string& QuestScript::getConditionMessage(ENUM_CHARACTERJOB job)
{
    if (m_questIdx != 0 && job >= 0 && job <= 10 && m_jobConditions[job])
    {
        return *m_jobConditions[job];
    }
    return m_conditionDefault;
