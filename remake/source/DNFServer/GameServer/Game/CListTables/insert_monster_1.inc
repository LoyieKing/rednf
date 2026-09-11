// [b28d-solo-CListTables N98/N93 NEAR] ORIG 0x0834fdee：54=54 助记符全等，仅
// it/end 槽互换（ORIG it -0x2c/end -0x24 vs 4.4 it -0x24/end -0x2c）。it 为先
// 声明后赋值（P50 复活，ORIG 单次 C1Ev），end 为初始化式直构；全头部声明多出
// 第二个 C1Ev（+5 条），交换声明序又改执行序 ⇒ 4.4 按首次使用序分配不可翻，封顶。
// [round1-L29 2026-09-10 修复] end 改内联 `if (it == m_monsterMap.end())`（ORIG end 无具名槽，
//   it/end 两具名对象槽互换消失）⇒ 54=54 IDENTICAL（此前 NEAR）。
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
}