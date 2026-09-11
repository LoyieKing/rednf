bool Quest::checkMatchedGrowType(ENUM_CHARACTERJOB job, int firstGrow,
                                 int secondGrow) const
{
    std::vector<stQuestTargetCharacter_t>::const_iterator it =
        m_targetCharacters.begin();
    std::vector<stQuestTargetCharacter_t>::const_iterator end =
        m_targetCharacters.end();
    while (it != end)
    {
        if (((it->m_job == job) || (it->m_job == 0xb)) &&
            ((it->m_firstGrow == firstGrow) || (it->m_firstGrow == -1)) &&
            ((it->m_secondGrow == secondGrow) || (it->m_secondGrow == -1)))
        {
            return 1;
        }
        ++it;
    }
    return 0;
}