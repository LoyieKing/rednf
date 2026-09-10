void createLotteryInfo(LotteryInfo* info, std::vector<int>* source)
{
    info->clear();
    std::vector<int>::iterator it = source->begin();
    info->m_defaultIdx = *it;
    it++;
    info->m_defaultCount = 1;
    while (it != source->end())
    {
        std::pair<int, int> p;
        p.first = *it;
        it++;
        if (it == source->end())
            break;
        p.second = *it;
        it++;
        info->m_items.push_back(p);
        info->m_counts.push_back(1);
    }
