CCompound_ExtreamDun_Item::_determine_level_bonus(
    unsigned int baseLevel, ENUM_INPUTITEMS_RARITY inRarity,
    unsigned int& idxOut, eBindSphereType bind) const
{
    idxOut = _determine_level_bonus_index(baseLevel, inRarity, bind);
    // [b28d-solo] 真语义修正：ORIG 0x832c9ca 段三次 `add $0xb80; add 0xc(%ebp);
    //   add $0xc` = m_lvBonus[bind]+0x0 ⇒ **m_levels**（等级表），非 m_rates[0]
    //   （0xb80+0x18=0xb98=m_rates[0]）。m_rates 仅供 _determine_level_bonus_index
    //   掷点用；本函数返回 pair(下界=levels[idx-1]+1, 上界=levels[idx])。
    //   P183/P157：无 rates/levels 引用局部（ORIG 每次重算成员地址）。
    // [b28d-solo] ORIG 0x832ca06 `mov (%eax),%esi` + `cmp %eax,%esi; jb`：
    //   idx 在比较左位（先物化到 callee-saved 跨 size() call）。
    if (idxOut == 0 ||
        idxOut >= m_lvBonus[(int)bind].m_levels.size())
    {
        // [b28d-solo-CItemList] ORIG 清零序 = 深槽 -0x14(hi) 先、浅槽 -0x10(lo) 后；
        //   4.4 槽按声明序（lo 浅/hi 深）⇒ 声明不带初始化、赋值语句按 hi→lo 序。
        int lo;
        int hi;
        hi = 0;
        lo = 0;
        return std::pair<unsigned int, unsigned int>(lo, hi);  // pair<int,int> 形
    }
    return std::pair<unsigned int, unsigned int>(
        m_lvBonus[(int)bind].m_levels[idxOut - 1] + 1,
        m_lvBonus[(int)bind].m_levels[idxOut]);
