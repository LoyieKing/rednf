bool Arad_StatisticsEventItemList::needStatistics(unsigned long itemIndex)
{
    // ORIG 0x0819d09a：std::find(begin, end, itemIndex) != end()；
    // ORIG 尾部为显式 0/1 归一（test/je/mov 形态），以 if 分支返回匹配。
    if (std::find(m_statisticsItemList.begin(), m_statisticsItemList.end(),
                  itemIndex) != m_statisticsItemList.end())
        return true;
    return false;
