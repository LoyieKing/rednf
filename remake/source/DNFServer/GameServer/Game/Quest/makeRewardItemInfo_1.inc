void makeRewardItemInfo(const std::vector<int>& src,
                        std::vector<std::pair<int, int> >& out)
{
    std::pair<int, int> p;
    std::vector<int>::const_iterator it = src.begin();
    while (it != src.end())
    {
        p.first = *it;
        it++;
        if (it == src.end())
        {
            cMyTrace(__PRETTY_FUNCTION__, 0x1031, 5)(
                "[%s][%d]_it == data.rewardSelectItems_.end()",
                __PRETTY_FUNCTION__, 0x1031);
            return;
        }
        p.second = *it;
        it++;
        out.push_back(p);
    }
