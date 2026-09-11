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
        // [G10 实验·P61/P178] ORIG 0x0832cb64..0x0832cb83 直证：两临时 int 槽
        // （-0x18 先 / -0x14 后）各清零后**取地址**传参，call 目标实例为
        // `pair<unsigned int,unsigned int>::pair<int,int>(int&&,int&&)`（`IiiE`）
        // ⇒ 原码是「字面量 0,0」形（prvalue int ⇒ 转发 ctor 推 _U1=int），
        // 非具名 lo/hi 左值（左值推 _U1=int& ⇒ 实例 `IRiS2_E`，p167 EXTRA 直证）。
        // 实测（fresh .o，check_tu_routed）：改前 DIFF/NEAR `+15/-15` 6 hunks →
        // 改后本件落 AE；CItemList 四数 694/1357/6/14/470 → 694/1358/6/14/469。
        return std::pair<unsigned int, unsigned int>(0, 0);
    }
    return std::pair<unsigned int, unsigned int>(
        m_lvBonus[(int)bind].m_levels[idx - 1] + 1,
        m_lvBonus[(int)bind].m_levels[idx]);
}