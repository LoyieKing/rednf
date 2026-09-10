int CCompound_ExtreamDun_Item::_FindRandomItem(                     // ORIG T 0x832d2ce
    ENUM_OUTPUTITEMS_RARITY outRarity,
    const std::pair<unsigned int, unsigned int>& range) const
{
    if ((int)outRarity < 0 || (int)outRarity > 3)
    {
        return 0;
    }
    typedef std::vector<OUTPUT_ITEM_STR> ItemVec;
    // ORIG 绝对偏移 this+0xb44（= &m_data + 0xb40 = m_resultItems，实证：
    // _AddItem push_back 目标亦为 +0xb44）。旧值 0xb3c 偏 4，已按 ORIG 修正。
    // [b28d-solo] §4.3 具名化：直接成员访问 m_data.m_resultItems[outRarity]
    //   （等价 this+0xb40+4+12*outRarity），去掉裸偏移 reinterpret_cast。
    const ItemVec& table = m_data.m_resultItems[(int)outRarity];
    ItemVec::const_iterator lo = std::lower_bound(
        table.begin(), table.end(), OUTPUT_ITEM_STR((int)range.first, 0),
        &BindTable_Predicate);
    ItemVec::const_iterator hi = std::upper_bound(
        table.begin(), table.end(),
        OUTPUT_ITEM_STR((int)range.second, 0x7fffffff), &BindTable_Predicate);
    // [b28d-solo] P167/P220/P171：ORIG 0x832d3ea 在比较前**再调一次
    //   table.end()**（sret -0x10）后 `operator!=(lo, end)`（0x832d40c）⇒
    //   源码为 `lo != table.end()`（非 lo != hi；gap 6 = 该次 end 调用）。
    if (table.end() != lo)
    {
        return _FindItemRelateCreationRate(lo, hi);
    }
    return 0;
