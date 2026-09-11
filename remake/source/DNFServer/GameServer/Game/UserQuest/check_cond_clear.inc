bool UserQuest::check_cond_clear(int questIdx) const
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_user->getGmQuestFlag())
        {
            if (m_quest[i] == questIdx)
            {
                return 1;
            }
        }
        else if (m_quest[i] == questIdx && m_trigger[i] == 0)
        {
            return 1;
        }
    }
    return 0;
}