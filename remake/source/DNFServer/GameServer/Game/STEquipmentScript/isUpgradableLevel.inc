bool STEquipmentScript::isUpgradableLevel(ItemUpgradeType::T type, int level)  // 0x0898efc8
{
    std::map<ItemUpgradeType::T, std::pair<int, int> >::iterator it =
        m_upgrade6fc.find(type);
    if (it != m_upgrade6fc.end())
    {
        if (level < it->second.first || it->second.second < level)
        {
            return false;
        }
    }
    return true;
