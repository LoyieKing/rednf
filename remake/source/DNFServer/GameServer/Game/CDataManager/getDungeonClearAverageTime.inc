unsigned int CDataManager::getDungeonClearAverageTime(int dungeonIdx, int partyCount,
                                                      ENUM_DUNGEON_DIFFICULTY difficulty)
{
    // ORIG 0x083639d2：result(-0xc) 入口初始化在 key ctor 前；case 0..3 走
    // -0xc 赋值汇聚（0x08363ac2 mov -0xc），end/default 两路直接 mov$0 返回
    // （0x08363a77/0x08363abb）。
    unsigned int result = 0;
    stClearTimeAverageMapKey key;                                // -0x18
    key.m_a = dungeonIdx;
    key.m_b = partyCount;

    // ORIG 仅发射非 const find/end（_ZNSt3map...4findERS5_ / 3endEv，无
    // _ZNK 重载）且 it 无 const_iterator 转换调用 ⇒ 原码经非 const 引用
    // 内联 cast 访问（ORIG 每处 `lea 0x5108(%eax)` 直发，无引用槽物化，
    // 0x083639f9/0x08363a1b），iterator 直接比较。
    typedef std::map<stClearTimeAverageMapKey, stClearTimeAverage> RankMap;
    RankMap::iterator it =
        const_cast<RankMap&>(m_rankSystem.m_dungeonRank.m_map0).find(key);
    // ORIG 0x08363a45 == 调用的 this = end 迭代器（-0x10）、实参 = it
    // （-0x1c）⇒ 原码操作数序为 `end() == it`。
    if (const_cast<RankMap&>(m_rankSystem.m_dungeonRank.m_map0).end() == it)
    {
        return 0;
    }

    // [b28d-lane16] 本地 ENUM_DUNGEON_DIFFICULTY 只声明了 _0=0（值域 0..0），
    // 直接 switch(difficulty) 会被前端裁掉 case 2/3（编译警告「case 标号值比该
    // 类型的最大值还要大」，ORIG 4 次 ptEv 只剩 2 次）；cast 到 int 后值域完整，
    // 4 个 case 逐条发射（mangled 不含 switch 类型，零 ABI 影响）。
    switch ((int)difficulty)
    {
    case 0: result = (unsigned int)it->second.m_v[0]; break;
    case 1: result = (unsigned int)it->second.m_v[1]; break;
    case 2: result = (unsigned int)it->second.m_v[2]; break;
    case 3: result = (unsigned int)it->second.m_v[3]; break;
    default: return 0;
    }
    return result;
