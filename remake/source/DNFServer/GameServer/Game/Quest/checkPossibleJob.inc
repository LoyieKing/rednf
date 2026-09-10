bool Quest::checkPossibleJob(ENUM_CHARACTERJOB job) const
{
    std::vector<stQuestTargetCharacter_t>::const_iterator it =
        m_targetCharacters.begin();
    std::vector<stQuestTargetCharacter_t>::const_iterator end =
        m_targetCharacters.end();
    while (it != end)
    {
        if ((it->m_job == job) || (it->m_job == 0xb))
        {
            return 1;
        }
        ++it;
    }
    return 0;
