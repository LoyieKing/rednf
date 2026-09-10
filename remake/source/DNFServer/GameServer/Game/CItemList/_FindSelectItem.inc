int CCompound_ExtreamDun_Item::_FindSelectItem(                     // ORIG T 0x832d15e
    unsigned int group, ENUM_OUTPUTITEMS_RARITY outRarity,
    const std::pair<unsigned int, unsigned int>& range) const
{
    // ORIG 0x832d164：group 为无符号双查（==0 je / >0x3a jbe），outRarity 独立
    // 有符号窗口（<0 js / >3 jle）——原合并写法被折成单 jle，且缺 outRarity 守卫。
    if ((group == 0) || (group > 0x3a))
    {
        return 0;
    }
    if ((int)outRarity < 0 || (int)outRarity > 3)
    {
        return 0;
    }
    typedef std::vector<OUTPUT_ITEM_STR> ItemVec;
    // [b28d-solo] §4.3 具名化：m_data.m_bindItems[group][outRarity]（等价
    //   this+0x30+48*group+12*outRarity），去裸偏移 reinterpret_cast。
    // [b28d-solo-CItemList] P183/P167：ORIG 0x832d15e **无 table 引用槽**——
    //   begin()/end() 各自重算完整地址（0x832d193 与 0x832d1cd 两段 12 条
    //   全等地址链），且只调 begin()/end() 各一次，lower_bound/upper_bound
    //   复用 tbegin/tend ⇒ 边界缓存但地址内联（消 -0xc 引用槽）。
    ItemVec::const_iterator tbegin = m_data.m_bindItems[group][outRarity].begin();
    ItemVec::const_iterator tend = m_data.m_bindItems[group][outRarity].end();
    ItemVec::const_iterator lo = std::lower_bound(
        tbegin, tend, OUTPUT_ITEM_STR((int)range.first, 0),
        &BindTable_Predicate);
    ItemVec::const_iterator hi = std::upper_bound(
        tbegin, tend, OUTPUT_ITEM_STR((int)range.second, 0x7fffffff),
        &BindTable_Predicate);
    // [b28d-solo] P167/P171：ORIG 0x832d296 段 `operator==(lo, tend)`——比较
    //   对象是 **tend**(-0x20) 而非 hi(-0x28)（hi 仅供 _FindItemRelateCreationRate）。
    if (lo == tend)
    {
        return 0;
    }
    return _FindItemRelateCreationRate(lo, hi);
