// ---- choose_item —— T 0x085f1560（choose_item.md 全文转写）----
// m_disabled 非 0 → -1；level>9 时向下取整到十位（钳 0x50）；
// level<10 且 rarity==2 → rarity 清 0 返 -1；_random_application 通过后
// 按 (rarity,levelLimit,partType) 查 m_baseItems60，取第 baseItem 项
// （越界回退末项）的 BaseItemValue::m_field0；任何失配 rarity 清 0 返 -1。
unsigned int CRandomOptionItemHandle::choose_item(ENUM_RARITY& rarity,
                                                  int level)
{
    // [Lane17 C 判词 · 已归零] AE（2026-09-10 fresh 复编，TU 四数 295/335/1/6 →
    // choose_item 由 DIFF 转 AE）。ORIG 0x085f1560。命中三招：
    // ①判据⑩/P212 声明重排：ORIG 槽 rem=−0x10(浅)/baseItem=−0x14/partType=−0x18/
    //   levelLimit=−0x1c(深)（085f15ac/085f166a/085f164c/085f1637 四处 store 实证）
    //   ⇒ 四量提到同层、按 ORIG 深序声明（levelLimit 最前=最深，rem 最后=最浅）；
    // ②P59/P224：empty 守卫改正极性早退（ORIG 085f16fe `call empty; test %al,%al;
    //   je <主流程>` 无 xor；我方旧 `if(!empty())` 多一条 `xor $0x1,%eax`）；
    // ③比较书写序：ORIG 085f1788 `mov -0xc(=idx),%eax; cmp -0x14(=baseItem),%eax;
    //   jge` ⇒ 源码写 `idx < baseItem`（旧 `baseItem > idx` 使 4.4 反用 `jle`）。
    // 旧判词：DIFF 201/205，语义等价。
    // [Lane13] P59/P16：ORIG 085f1566 段 `cmpl $0x0,0xc(%eax); je <主体>` ——
    // m_disabled!=0 时早返 -1（fall-through 为主体）⇒ 早返形，非 `== 0` 正向包裹
    // （后者 4.4 产 `jne <fail>` 反向块序）。
    if (m_disabled != 0)
    {
        return 0xffffffff;
    }
    {
        // [Lane17 C1] 判据⑩/P212：ORIG 槽 rem=-0x10(最浅)/baseItem=-0x14/partType=-0x18/
        // levelLimit=-0x1c(最深)（085f15ac/085f166a/085f164c/085f1637 四处 store 实证）。
        // 4.4 实测对函数级声明按「先声明深」（我方旧形 levelLimit-0x18<partType-0x14<
        // baseItem-0x10，块内 rem 另占更深 -0x1c）⇒ 四个量全提为同层、按 ORIG 深序
        // 声明（levelLimit 最先=最深，rem 最后=最浅），块内只赋值。
        int levelLimit;
        int partType;
        int baseItem;
        int rem;
        if (level >= KItemStartLevel)
        {
            if (level < kMaxItemLevel)
            {
                rem = level % KItemLevelInterval;
                level = level - rem;
            }
            else
            {
                level = kMaxItemLevel;
            }
        }
        if (level < KItemStartLevel && rarity == 2)
        {
            rarity = static_cast<ENUM_RARITY>(0);
            return 0xffffffff;
        }
        // [Lane13] P248 档①：callee bool 化后调用点写裸形（`== 1` 会产 cmp/sete）。
        // [Lane13] P59/P248档①：ORIG 085f163d `xor $0x1,%eax; test %al,%al; je <主体>`
        // = `if (!bool_call()) return -1;` 早返形（callee 已 bool 化）。
        if (!_random_application(rarity, level))
        {
            return 0xffffffff;
        }
        {
            m_field08 = m_field08 + 1;
            levelLimit = _get_level_limit(rarity, level);
            partType = _get_part_type(levelLimit);
            baseItem = _get_base_item(rarity, levelLimit);
            BaseItemKey key((unsigned char)rarity,
                            (unsigned char)levelLimit,
                            (unsigned short)partType);
            // ORIG：外层 map<int,...> 键 = BaseItemKey 小端 int 打包
            // [b28d-solo-RandomRentalA2] P0/P48：ORIG 2 处独立调 GetRandomOption。
            std::map<int, std::map<int, BaseItemValue> >::const_iterator it =
                reinterpret_cast<const RandomOptionScript*>(GetRandomOption())
                    ->m_baseItems60.find(key.m_key);   // [Lane13] 同上（去 *(int*)&key cast）
            // [b28d-solo-RandomRentalA2 P222] ORIG 走 `ne` 正极性（p167 MISSING
            // `const_iterator ne`，EXTRA `eq`）⇒ 正向包裹 + 末尾共享失败出口。
            if (it != reinterpret_cast<const RandomOptionScript*>(GetRandomOption())
                          ->m_baseItems60.end())
            {
                // [Lane17 C2] P59/P224：ORIG 085f16fe `call empty; test %al,%al;
                // je <主流程>` + fall-through 失败块 ⇒ 正极性早退（`if (!empty())`
                // 恒多一条 `xor $0x1,%eax`）。
                if (it->second.empty())
                {
                    rarity = static_cast<ENUM_RARITY>(0);
                    return 0xffffffff;
                }
                int idx = 0;
                std::map<int, BaseItemValue>::const_iterator vit = it->second.begin();
                // [Lane17 C2] ORIG 085f1788 `mov -0xc(=idx),%eax; cmp -0x14(=baseItem),%eax;
                // jge` ⇒ 比较书写序为 `idx < baseItem`（`baseItem > idx` 使 4.4 把左式
                // 放 eax 并反用 `jle`）。
                while (vit != it->second.end() && idx < baseItem)
                {
                    ++vit;
                    idx = idx + 1;
                }
                if (vit == it->second.end())
                {
                    --vit;
                }
                return static_cast<unsigned int>(vit->second.m_field0);
            }
            rarity = static_cast<ENUM_RARITY>(0);
            return 0xffffffff;
        }
        return 0xffffffff;
    }
    return 0xffffffff;
}