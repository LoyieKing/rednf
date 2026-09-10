double GuildParameterScript::getExpWeight(int guildLevel)
{
    for (int i = (int)m_vecE4.size() - 1; i >= 0; --i)
    {
        if (m_vecE4[i].first <= guildLevel)
            return m_vecE4[i].second;
    }
    return m_vecE4[0].second;
