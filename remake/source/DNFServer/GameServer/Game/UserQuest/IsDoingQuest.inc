bool UserQuest::IsDoingQuest(int questIdx) const
{
    for (int i = 0; i <= 19; ++i)
    {
        if (m_quest[i] == questIdx && m_trigger[i] != 0)
        {
            return 1;
        }
    }
    return 0;
