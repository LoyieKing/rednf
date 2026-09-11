// ---------------------------------------------------------------------------
// _findBonusItem —— ORIG 0x081ac04c：遍历 list 取最后一个 m_flag <= threshold
// 的元素 memcpy 0xa 字节到 out；无命中返回 false。
// ---------------------------------------------------------------------------
// b28d-solo-Arad_ItemBonus: ORIG best 为**迭代器**（best=list.end()，槽 -0x18），
// it 槽 -0x14；命中 `best=it`（无 operator*），尾判 `best==list.end()`（-0xc）后
// `out=*best`（结构赋值 ⇒ 0xa 字节 memcpy + 自拷贝守卫，ORIG 0x081ac0d3..0x081ac136）。
bool CeraShopPurcahseCountBonus::_findBonusItem(
    const std::vector<PurcahseBonus>& list, PurcahseBonus& out,
    unsigned short threshold)
{
    std::vector<PurcahseBonus>::const_iterator it = list.begin();
    std::vector<PurcahseBonus>::const_iterator best = list.end();
    for (; it != list.end(); ++it)
    {
        if ((unsigned short)it->m_flag <= threshold)
            best = it;
    }
    if (best == list.end())
        return false;

    out = *best;
    return true;
}