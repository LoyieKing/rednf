void WarField::PickupItem(std::map<int, map_item>::iterator it)
{
    m_mapInfo.m_itemMap.erase(it);
