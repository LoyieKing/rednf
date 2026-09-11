void CMonsterManager::destroy()
{
    CMonster* monster = 0;
    __gnu_cxx::hash_map<int, CMonster*>::iterator it;
    it = m_monsterMap.begin();
    while (it != m_monsterMap.end())
    {
        monster = it->second;
        it++;
        delete monster;
    }
    m_monsterMap.clear();
}