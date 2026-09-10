bool CMonsterManager::insert_monster(int level, CMonster* monster)
{
    // [b28d-32] ORIG 0x0834fdee：迭代器先默认构造（C1Ev）再 find 赋值
    // （P50 复活 + find 结果 8 字节拷贝到 it 槽）；insert 用显式
    // pair<const int,CMonster*>(int&,CMonster*&) 直构，无 make_pair 中转。
    __gnu_cxx::hash_map<int, CMonster*>::iterator it;
    it = m_monsterMap.find(level);
    if (it == m_monsterMap.end())
    {
        m_monsterMap.insert(std::pair<const int, CMonster*>(level, monster));
        return true;
    }
    return false;
