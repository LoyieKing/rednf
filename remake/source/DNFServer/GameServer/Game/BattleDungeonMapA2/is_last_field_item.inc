bool CBattle_Field::is_last_field_item(int uid)
{
    std::map<int, map_item>::iterator it;
    it = GetLastMapInfo()->m_itemMap.find(uid);
    if (it == GetLastMapInfo()->m_itemMap.end())
        return false;
    return true;
