int CDeathTower::CStage::killMonster(int idx, map_monster& monster)
{
    // [b28d-solo-CDeathTower] 旧判「N93 封顶」作废——[b28d-PoolTower] 翻正 AE：
    // 第二具名 const_iterator 改**显式转换临时内联** `it == const_iterator(map.end())`
    // 后 4.4 复现 ORIG 槽阶梯（it=-0x1c / find 转换源 -0x18 / itEnd+end 临时 -0x14），
    // 62=62 逐条全等；C1 转换/eq/pt 符号集不变（P167 CLEAR 9/9 维持）。
    // ORIG 0x8461708：find(idx)；命中则 monster = it->second、取
    // m_level(+0x10) 作返回值并 erase；未命中返回 0。
    // 注：ORIG movzbl 0x14(%pair) = pair+0x14 = map_monster+0x10 = m_level
    //（map_monster 为 pack(1)，second @ pair+4）。
    // ORIG 0x8461731/0x846176b：find/end 各经 _Rb_tree_const_iterator C1(iter&)
    //（0x81529e0 两次）⇒ const_iterator 具名；erase@0x8152a12 arg=&idx(key)。
    // ORIG 0x846170e：roleType 槽 -0xc 于函数头 `movl $0` ⇒ 声明先于迭代器（P0 时机）。
    int roleType = 0;
    std::map<int, map_monster>::const_iterator it = m_mapInfo.m_monsterMap.find(idx);
    if (it == std::map<int, map_monster>::const_iterator(m_mapInfo.m_monsterMap.end()))
    {
        return 0;
    }
    monster = it->second;
    roleType = (unsigned char)it->second.m_level;
    m_mapInfo.m_monsterMap.erase(idx);
    return roleType;
