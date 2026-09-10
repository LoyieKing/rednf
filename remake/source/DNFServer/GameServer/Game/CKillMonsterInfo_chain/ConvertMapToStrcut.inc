int CKillMonsterInfo::ConvertMapToStrcut(
    const std::map<unsigned int, unsigned char>& inMap,
    stMonsterInfo_t* pInfo, unsigned int count) const
{
    if (inMap.empty())
        return 1;
    if (inMap.size() > count)
        return 0;
    std::map<unsigned int, unsigned char>::const_iterator it;
    it = inMap.begin();
    unsigned int i = 0;
    while (it != inMap.end() && i < count)
    {
        pInfo[i].mobIdx = it->first;
        pInfo[i].diff = it->second;
        it++;
        i++;
    }
    return 1;
