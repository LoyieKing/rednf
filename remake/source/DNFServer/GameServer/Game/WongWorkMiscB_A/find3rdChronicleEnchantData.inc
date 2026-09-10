STEnchantSystemMapData* CItemUpgrade::find3rdChronicleEnchantData(
    STEnchantSystemMapKey key, int type)
{
    CItem* itemScript = G_CDataManager()->find_item(type);
    if (itemScript == 0)
        return 0;
    typedef std::map<STEnchantSystemMapKey, STEnchantSystemMapData> EnchantMap;
    EnchantMap::iterator it =
        ((CStackableItem*)itemScript)->m_enchantMap.find(key);
    if (it != ((CStackableItem*)itemScript)->m_enchantMap.end())
        return &it->second;
    return 0;
