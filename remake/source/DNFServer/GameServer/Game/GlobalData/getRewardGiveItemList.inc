DimensionActivationRewardData::getRewardGiveItemList(int job, int growType)
{
    std::pair<int, int> key;
    key.first = job;
    key.second = growType;
    std::map<std::pair<int, int>,
             std::vector<std::vector<EventRewardItemInfo> > >::iterator it =
        m_map24.find(key);
    if (it != m_map24.end())
    {
        return &it->second;
    }
    return 0;
}