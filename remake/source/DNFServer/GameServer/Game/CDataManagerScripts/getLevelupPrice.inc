int GuildParameterScript::getLevelupPrice(int level, int memberCnt)
{
    for (int i = (int)m_vecF0.size() - 1; i >= 0; --i)
    {
        if (m_vecF0[i].first <= memberCnt)
            return m_vecF0[i].second[level] * 10000;
    }
    return m_vecF0[0].second[level] * 10000;
