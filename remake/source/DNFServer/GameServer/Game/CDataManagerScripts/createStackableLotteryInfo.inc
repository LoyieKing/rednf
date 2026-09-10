void createStackableLotteryInfo(LotteryInfo* info, std::vector<int>* source)
{
    info->clear();
    std::vector<int>::iterator it = source->begin();
    info->m_defaultIdx = *it;
    it++;
    info->m_defaultCount = *it;
    it++;
    while (it != source->end())
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        if (it == source->end())
            break;
        p.second = *it;
        it++;
        if (it == source->end())
            break;
        info->m_counts.push_back(*it);
        it++;
        info->m_items.push_back(p);
    }
