int UserQuest::finish_quest(int questIdx)
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx)
        {
            if (m_trigger[i] == 0)
            {
                m_quest[i] = 0;
                m_trigger[i] = 0;

                Quest* quest = G_CDataManager()->find_quest(questIdx);
                if (quest != 0 && !quest->isRepeatableQuest())
                {
                    m_cleared.setClearedQuest(questIdx);
                }
                if (quest->m_type == 8)
                {
                    m_authenData.m_urgentQuest = -1;
                }
                return 0;
            }
            return 0x16;
        }
    }
    return 0x13;
}