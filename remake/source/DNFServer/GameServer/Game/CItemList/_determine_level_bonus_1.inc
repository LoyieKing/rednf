CCompound_ExtreamDun_Item::_determine_level_bonus(
    ENUM_INPUTITEMS_RARITY inRarity, unsigned int baseLevel,
    ENUM_OUTPUTITEMS_RARITY outRarity, eBindSphereType bind) const
{
    unsigned int idx = _determine_level_bonus_index(baseLevel, inRarity, bind);
    if ((int)inRarity == 0 && (int)outRarity > 1 && (int)outRarity <= 3 && idx != 0)
    {
        --idx;
    }
    // [b28d-solo] 同上：ORIG 0x832cadc 段 `add $0xc` = m_levels（非 m_rates[0]）；
    //   P183/P157 去引用缓存（ORIG 逐次重算成员地址）。
    if (idx == 0 || m_lvBonus[(int)bind].m_levels.size() <= idx)
    {
        // [b28d-solo-CItemList] 同 Eii 版：ORIG 先 hi(-0x18) 后 lo(-0x14)。
        int lo;
        int hi;
        hi = 0;
        lo = 0;
        return std::pair<unsigned int, unsigned int>(lo, hi);
    }
    return std::pair<unsigned int, unsigned int>(
        m_lvBonus[(int)bind].m_levels[idx - 1] + 1,
        m_lvBonus[(int)bind].m_levels[idx]);
