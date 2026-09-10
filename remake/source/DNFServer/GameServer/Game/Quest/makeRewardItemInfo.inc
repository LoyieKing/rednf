void makeRewardItemInfo(const std::vector<int>& src,
                        std::vector<RewardItemInfo>& out)
{
    RewardItemInfo item;
    std::vector<int>::const_iterator it = src.begin();
    while (it != src.end())
    {
        item.m_itemIdx = *it;
        it++;
        if (it == src.end())
        {
            // ORIG 0x83522ce：cMyTrace 记录 "it == data.rewardSelectItems_.end()"
            cMyTrace(__PRETTY_FUNCTION__, 0x104d, 5)(
                "[%s][%d]_it == data.rewardSelectItems_.end()",
                __PRETTY_FUNCTION__, 0x104d);
            return;
        }
        item.m_count = *it;
        it++;
        out.push_back(item);
    }
