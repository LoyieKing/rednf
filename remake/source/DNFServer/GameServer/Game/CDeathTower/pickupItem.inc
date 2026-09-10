bool CDeathTower::CStage::pickupItem(int idx, map_item& item)
{
    // [b28d-solo-CDeathTower] 旧判「N93 无源码解」作废——[b28d-PoolTower] 翻正 AE：
    // 同 peekItem/killMonster 显式转换临时内联形（it=-0x28/findtmp=-0x24/
    // itEnd=-0x20/endtmp=-0x1c 槽阶梯复现），65=65 逐条全等，P167 CLEAR 7/7 维持。
    // ORIG 0x84615e4：peek 语义 + erase。
    // ORIG 0x8461619/0x8461643：find/end 各经 _Rb_tree_const_iterator C1(0x8152fa2)
    // 转换 ⇒ const_iterator 具名；erase@0x8152c18 arg=&idx（key 版）。
    std::map<int, map_item>::const_iterator it = m_mapInfo.m_itemMap.find(idx);
    if (it == std::map<int, map_item>::const_iterator(m_mapInfo.m_itemMap.end()))
    {
        return false;
    }
    item = it->second;
    m_mapInfo.m_itemMap.erase(idx);
    return true;
