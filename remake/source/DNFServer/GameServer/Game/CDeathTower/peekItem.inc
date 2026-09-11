bool CDeathTower::CStage::peekItem(int idx, map_item& item)
{
    // [b28d-solo-CDeathTower] 旧判「N93 封顶」作废——[b28d-PoolTower] 翻正 AE：
    // itEnd 具名槽改显式转换临时 `const_iterator(map.end())`（原码即右值比较形，
    // ORIG itEnd=-0x20/endtmp=-0x1c 与匿名临时槽位同构），59=59 逐条全等。
    // ORIG 0x8461534：m_mapInfo.m_itemMap.find(idx)；命中拷贝 0x54 字节返回 true。
    // ORIG 0x8461569/0x8461593：find/end 结果各经 _Rb_tree_const_iterator C1(iter&)
    // 转换（0x8152fa2 两次 call）⇒ 源以 const_iterator 具名持有。
    std::map<int, map_item>::const_iterator it = m_mapInfo.m_itemMap.find(idx);
    if (it == std::map<int, map_item>::const_iterator(m_mapInfo.m_itemMap.end()))
    {
        return false;
    }
    item = it->second;
    return true;
}