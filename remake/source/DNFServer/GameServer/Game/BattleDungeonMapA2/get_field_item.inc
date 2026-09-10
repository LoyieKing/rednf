bool CBattle_Field::get_field_item(int uid, map_item& out)
{
    std::map<int, map_item>::iterator it;
    it = GetCurrentMapInfo()->m_itemMap.find(uid);
    if (it == GetCurrentMapInfo()->m_itemMap.end())
        return false;
    out = it->second;
    return true;
