int ServerParameterScript::GetPriceAverage(int idx)
{
    if (m_bloodReward794.m_vec0.empty())
    {
        return 0;
    }
    std::vector<std::pair<int, int> >::iterator it =
        m_bloodReward794.m_vec0.begin();
    std::vector<std::pair<int, int> >::iterator e =
        m_bloodReward794.m_vec0.end();
    for (; it != e; ++it)
    {
        if ((*it).first >= idx)
        {
            return (*it).second;
        }
    }
    --it;
    return (*it).second;
