bool UserQuest::isDoingJobChangeQuest() const
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] != 0)
        {
            Quest* quest = G_CDataManager()->find_quest(m_quest[i]);
            if (quest != 0 && (quest->m_jobChangeQuest == 1 || quest->m_jobChangeQuest == 2))
            {
                return true;
            }
        }
    }
    return false;
}