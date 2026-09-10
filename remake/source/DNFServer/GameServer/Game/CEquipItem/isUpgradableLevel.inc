bool CEquipItem::isUpgradableLevel(ItemUpgradeType::T type, int level)
{
    std::map<ItemUpgradeType::T, std::pair<int, int> >::iterator it =
        m_upgradeLevel.find(type);
    if (it != m_upgradeLevel.end())
    {
        if (level < it->second.first || it->second.second < level)
        {
            return 0;
        }
    }
    return 1;
