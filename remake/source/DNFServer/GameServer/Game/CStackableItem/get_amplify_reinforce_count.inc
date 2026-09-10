int CStackableItem::get_amplify_reinforce_count(int count)
{
    std::map<int, int>::iterator it = m_amplifyMap.begin();
    std::map<int, int>::iterator end = m_amplifyMap.end();
    int sum = 0;
    int bestKey = 0;
    int bestCount = 0;
    while (it != end)
    {
        sum += it->second;
        if (count < sum)
        {
            return it->first;
        }
        if (bestCount < it->second)
        {
            bestCount = it->second;
            bestKey = it->first;
        }
        ++it;
    }
    return bestKey;
