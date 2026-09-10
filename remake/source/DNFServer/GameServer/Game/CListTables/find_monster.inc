CMonster* CMonsterManager::find_monster(int idx)
{
    __gnu_cxx::hash_map<int, CMonster*>::iterator it;
    it = m_monsterMap.find(idx);
    if (it == m_monsterMap.end())
    {
        return 0;
    }
    return it->second;
