DimensionActivationRewardData::getRewardSelectItemList(int packageKey)
{
    std::map<int, std::vector<std::vector<EventRewardItemInfo> > >::iterator it =
        m_mapC.find(packageKey);
    if (it != m_mapC.end())
    {
        return &it->second;
    }
    return 0;
