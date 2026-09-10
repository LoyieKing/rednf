bool WarField::GetFieldMob(int idx, map_monster& out)
{
    std::map<int, map_monster>::iterator it;
    it = m_mapInfo.m_monsterMap.find(idx);
    if (it == m_mapInfo.m_monsterMap.end())
    {
        return 0;
    }
    {
        map_monster mob(it->second);
        out = mob;
    }
    return 1;
