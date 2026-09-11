int CMonsterManager::insert_monster(int level, int monsterIdx)
{
    // ORIG 直接以 value_type 就地构造 pair<const int,int>(int&,int&)（单 ctor、
    // 无 make_pair 中转、无临时对象拷贝）→ 用显式 std::pair<const int,int> 形。
    m_levelMonsterList.insert(std::pair<const int, int>(level, monsterIdx));
    return 1;
}