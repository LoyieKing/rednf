int GenerateRandomUpgrade::generateUpgrade(ENUM_RARITY rarity, int levelDelta)
{
    CDataManager* dm = G_CDataManager();
    FarmingMap* farming = reinterpret_cast<FarmingMap*>(
        reinterpret_cast<char*>(dm) + 0x528);

    FarmingMap::const_iterator it = farming->find(static_cast<int>(rarity));
    if (it == farming->end())
        return 0;
    if (it->second.m_active == 0)              // pair.second + ItemFarmingData+0xc
        return 0;

    // P167/§4.1：ORIG 直接 G_CDataManager()+0x540 取 rates，无 helper
    std::vector<std::pair<int, float> > const* rates =
        reinterpret_cast<std::vector<std::pair<int, float> > const*>(
            reinterpret_cast<char*>(G_CDataManager()) + 0x540);
    float factor = 1.0f;
    if (!rates->empty())
    {
        // P167/§4.1：内联三元+循环查表（无 lookupGradeRatio helper）
        int last = static_cast<int>(rates->size()) - 1;
        if ((*rates)[0].first < levelDelta)
            factor = (*rates)[0].second;
        else if ((*rates)[last].first > levelDelta)
            factor = (*rates)[last].second;
        else
        {
            for (int i = 0; i <= last; ++i)
            {
                if ((*rates)[i].first == levelDelta)
                    factor = (*rates)[i].second;
            }
        }
    }

    ItemFarmingData entry = it->second;         // ORIG ItemFarmingData copy ctor

    int weightOld = entry.m_vecWeight[0].second;
    entry.m_vecWeight[0].second = static_cast<int>(static_cast<float>(weightOld) * factor);
    entry.m_active = entry.m_active - weightOld + entry.m_vecWeight[0].second;

    int roll = m_rand->randInt(static_cast<const unsigned long&>(entry.m_active));

    int acc = 0;
    for (size_t i = 0; i < entry.m_vecWeight.size(); ++i)
    {
        acc += entry.m_vecWeight[i].second;
        if (roll <= acc)
            return entry.m_vecWeight[i].first;
    }
    return 0;                                // ORIG 未命中返回 0
