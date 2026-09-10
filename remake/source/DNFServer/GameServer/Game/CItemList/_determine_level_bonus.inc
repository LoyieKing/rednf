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
        // [G10 实验·P61/P178] ORIG 0x0832ca3b..0x0832ca5a 直证：两临时 int 槽
        // （-0x14 先 / -0x10 后）各清零后**取地址**传参，call 目标实例为
        // `pair<unsigned int,unsigned int>::pair<int,int>(int&&,int&&)`（`IiiE`）
        // ⇒ 原码是「字面量 0,0」形（prvalue int ⇒ 转发 ctor 推 _U1=int），
        // 非具名 lo/hi 左值（左值推 _U1=int& ⇒ 实例 `IRiS2_E`，p167 EXTRA 直证）。
        // 实测（fresh .o，check_tu_routed）：改后本件维持 AE；CItemList 四数
        // 694/1357/6/14/470 → 694/1358/6/14/469（missing 减 1 = 不再发射
        // ORIG 无的 `IRiS2_E` 家族；p167 该件差集仅剩 C1/C2、D1/D2 别名对）。
        return std::pair<unsigned int, unsigned int>(0, 0);
    }
    return std::pair<unsigned int, unsigned int>(
        m_lvBonus[(int)bind].m_levels[idxOut - 1] + 1,
        m_lvBonus[(int)bind].m_levels[idxOut]);
