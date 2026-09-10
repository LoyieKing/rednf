unsigned int CRandomOptionItemHandle::RegenVer_choose_item(
    ENUM_RARITY rarity, int level, const std::vector<int>& itemGroups)
{
    // [b28d-solo-RandomRentalA2 判词] DIFF 170/173（ORIG 0x085f1802）：P0/P48
    // table/script 内联已归位；残余槽序/物化次数。旧判词：
    // 语义等价。
    int limit = _get_level_limit(rarity, level);

    int partType = 0;
    if (itemGroups.empty())
    {
        partType = _get_part_type(limit);
    }
    else
    {
        partType = _get_part_type_by_ItemGroup(limit, itemGroups);
    }

    int count = _get_base_item(rarity, limit);

    BaseItemKey key((unsigned char)rarity, (unsigned char)limit,
                    (unsigned short)partType);

    // ORIG：外层 map<int,...> 键 = BaseItemKey 小端 int 打包
    // [b28d-solo-RandomRentalA2] P0/P48：ORIG 2 处独立调 GetRandomOption
    // （find/end 各一），无 table/script 具名局部。
    std::map<int, std::map<int, BaseItemValue> >::const_iterator outer =
        reinterpret_cast<const RandomOptionScript*>(GetRandomOption())
            ->m_baseItems60.find(key.m_key);   // [Lane13] BaseItemKey 具名 int 键视图（去 cast 反模式）

    // [SweepRandomRental-2] ORIG 0x085f18fb 走 `ne` 正极性、0x085f1902
    // `je 085f1a03` 直落失败块（日志+返回 -1）⇒ 正向包裹 + 单一尾 return
    // （`==` 早返形令 p167 MISSING ne / EXTRA eq 且块序反）。
    if (outer != reinterpret_cast<const RandomOptionScript*>(GetRandomOption())->m_baseItems60.end())
    {
        // [SweepRandomRental-2 P183] ORIG 每处 `call ptEv; add/lea 0x4` 重取
        // outer->second（0x085f1908/0x085f1933/0x085f1964/0x085f19b2），
        // 无 `const std::map& inner` 引用缓存。
        if (outer->second.empty())
        {
            return 0xffffffffu;
        }

        int steps = 0;
        std::map<int, BaseItemValue>::const_iterator walk = outer->second.begin();
        // [SweepRandomRental-2] ORIG 0x085f199a `mov steps,%eax; cmp count,%eax;
        // jge` ⇒ 源码条件为 `steps < count`（`count > steps` 形产 jle 反向）。
        while (walk != outer->second.end() && steps < count)
        {
            ++walk;
            ++steps;
        }
        if (walk == outer->second.end())
        {
            --walk;
        }
        return (unsigned int)walk->second.m_field0;
    }

    LogManager::logFormat(
        1, "RandomOption.cpp",
        "itemIndex_t random_option::CRandomOptionItemHandle::"
        "RegenVer_choose_item(ENUM_RARITY, int, const std::vector<int, "
        "std::allocator<int> >&)",
        0x19a, "choose_item (%d,%d,%d,%d)", (int)key.m_rarity,
        (int)key.m_level, (int)key.m_partType, count);
    return 0xffffffffu;
