bool AvatarFixedHiddenOptionServer::Init()
{
    for (int i = 0; i < 6; ++i)
    {
        int cumulative = 0;
        int count = (int)m_abilities[i].size();
        m_cumulativeRate[i].reserve((std::vector<int>::size_type)(count - 1));
        for (int k = 1; k < count; ++k)
        {
            cumulative += m_abilities[i][k].m_nRate;
            m_cumulativeRate[i].push_back(cumulative);
        }
    }
    return true;
