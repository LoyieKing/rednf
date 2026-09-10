const char* CAvatarItemMgr::getJewelSocketData(int key) const
{
    Avatar_Item* item = _FindItemAll(key);
    if (item == 0)
        return (const char*)&m_emblemInfo;
    return item->getJewelSocketData();
