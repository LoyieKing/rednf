// ---- RegenVer_choose_item —— T 0x085f1802（同名报告全文转写）-------------
// [RentalPrett 2026-09-11 · 负结论 / 维持手写串] 本件**无法**换 `__PRETTY_FUNCTION__`：
//   ORIG prett 符号 `_ZZN13random_option23CRandomOptionItemHandle20RegenVer_choose_item
//   E11ENUM_RARITYiRKSt6vectorIiSaIiEEE19__PRETTY_FUNCTION__` @0x8cd10a0 size=0x8a，
//   串 = `itemIndex_t random_option::CRandomOptionItemHandle::RegenVer_choose_item(
//   ENUM_RARITY, int, const std::vector<int, std::allocator<int> >&)`。
//   宏按定义处拼写打印**返回类型** ⇒ 定义侧须写 `itemIndex_t`(=unsigned long)，
//   与类内声明 `unsigned int`（CItemGenerator.h:446）**类型不同** ⇒ 实测编译错误
//   `RegenVer_choose_item.inc:1: prototype for 'itemIndex_t …' does not match any in class
//   'random_option::CRandomOptionItemHandle'（candidate is: unsigned int …）`。
//   修它必须改 CItemGenerator.h（本任务禁止改 .h）⇒ 保留 .cpp 手写串。
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
        // [RentalPrett 2026-09-11 · 负结论] 保留手写串：ORIG prett 串返回类型为
        // `itemIndex_t`(=unsigned long)，我方类内声明为 `unsigned int`
        // （CItemGenerator.h:446）⇒ 宏展开不符；改它须改 .h（本任务禁止）。
        // 详见 RandomRentalA2/RegenVer_choose_item.inc 头部负结论。
        "itemIndex_t random_option::CRandomOptionItemHandle::"
        "RegenVer_choose_item(ENUM_RARITY, int, const std::vector<int, "
        "std::allocator<int> >&)",
        0x19a, "choose_item (%d,%d,%d,%d)", (int)key.m_rarity,
        (int)key.m_level, (int)key.m_partType, count);
    return 0xffffffffu;
}