int TrainingQuestScript::getTrainingQuest(int dayIndex, int questCount, int job)
{
    if (questCount == job)
    {
        ++m_applyCount;
    }
    else
    {
        m_applyCount = 0;
    }
    if (questCount == -100)
    {
        if (m_applyCount >= m_sameApplyQuest.size())
        {
            return 0x7fffffff;
        }
        return m_sameApplyQuest.at(m_applyCount);
    }
    if (dayIndex - 1 + questCount < 0)
    {
        return 0x7fffffff;
    }
    if (m_applyCount >= m_questProb[dayIndex - 1 + questCount].size())
    {
        return 0x7fffffff;
    }
    return m_questProb[dayIndex - 1 + questCount].at(m_applyCount);
