bool CBattle_Field::GetFieldMob(int uid, map_monster& out)
{
    std::map<int, map_monster>::iterator it;
    it = GetCurrentMapInfo()->m_monsterMap.find(uid);
    if (it == GetCurrentMapInfo()->m_monsterMap.end())
        return false;
    {
        map_monster tmp(it->second);
        out = tmp;
    }
    return true;
