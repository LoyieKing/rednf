// [b28d-32 AE: const_iterator 对 + 后置 it++ + 具名 result 复现 ORIG，51=51 逐条全等 | ORIG 0x0834ff20]
int CMonsterManager::get_levelrandom_mob(int level)
{
    // [b28d-32] ORIG 0x0834ff20：range 类型为 const_iterator 对 ⇒ 从 non-const
    // equal_range 结果隐式转换构造（_C1ISt17_Rb_tree_iterator...EOS_ 实例）；
    // 迭代器遍历用后置 it++（ORIG 增量段 ppEi，P28）。
    std::pair<std::multimap<int, int>::const_iterator,
              std::multimap<int, int>::const_iterator> range =
        m_levelMonsterList.equal_range(level);
    int count = m_levelMonsterList.count(level);
    int rand = get_rand_int(count);
    std::multimap<int, int>::const_iterator it = range.first;
    for (int i = 0; i < rand; ++i)
    {
        it++;
    }
    // ORIG 0x0834ffc0 尾：`mov 0x4(%eax),%eax; mov %eax,-0x10(%ebp);
    // mov -0x10(%ebp),%eax` ⇒ 具名结果局部 + return（P171 照抄）。
    int result = it->second;
    return result;
}