// ORIG 0x080f4a0a。在 m_boosterIndexMap 查 cardIdx/boosterIdx，两键都存在时返回对应
// m_recipeMap[(min,max)] 的值（min/max 为两键对应的 short 值）；否则 0。
unsigned long Event_Upgrade_Jar::getBoosterIndex_byUpgradeCardIndex(unsigned long& cardIdx, unsigned long& boosterIdx)
{
    // b28d-lane25：P167 差集实证 ORIG 用非 const `_Rb_tree_iterator`（ne/ptEv）且
    // make_pair(low,high) 为 pair<short,short>→pair<int,int> 转换（P96/P270/P192）。
    std::map<unsigned long, short>::iterator itCard = m_boosterIndexMap.find(cardIdx);
    std::map<unsigned long, short>::iterator itBooster = m_boosterIndexMap.find(boosterIdx);
    // [b28d-solo-EventUpgradeJar] ORIG 0x080f4a78/0x080f4aa6 两条件各 test/je 后
    // `mov $1/%eax … mov $0` 再 `test %al,%al; je`（bool 短路物化、值无槽往返）⇒
    // P226 语句表达式包裹 && 短路（嵌套 if 会被 4.4 直跳、缺物化岛）。
    // P74 补充②：ORIG 0x080f4a66/0x080f4a94 的 ne 调用 this=end 临时、实参=it
    // ⇒ 源码把 end() 写在表达式左侧（`end() != it`）。
    if (({ m_boosterIndexMap.end() != itCard && m_boosterIndexMap.end() != itBooster; }))
    {
        // ORIG 槽序：low=-0x2c（深）/high=-0x2a（浅）。用 short[2] 数组：ORIG
        // 0x080f4b1e 的 make_pair 第二实参 `lea -0x2c(%ebp),%edx; add $0x2,%edx`
        // 只在数组形下复现（独立标量恒直接 lea -0x2a；探针 /tmp/eu_probe8.cpp
        // fA 独立标量/fB 数组对照）。
        short lh[2];
        lh[0] = itCard->second;
        lh[1] = itBooster->second;
        // ORIG 0x080f4af1..0x080f4b17：三次 `movzwl; xor; mov %ax` ⇒ 原码 XOR 交换
        // （tmp 局部产 mov 三连，非 ORIG 形）。
        // ORIG 0x080f4ae4 先装 low（-0x2c）后装 high（-0x2a），`cmp %ax,%dx; jle`
        // ⇒ 跳过条件为 low<=high，写成 `low > high` 才得同装载序/极性。
        if (lh[0] > lh[1])
        {
            lh[0] ^= lh[1];
            lh[1] ^= lh[0];
            lh[0] ^= lh[1];
        }

        std::map<std::pair<int, int>, unsigned long>::iterator itRecipe =
            m_recipeMap.find(std::make_pair(lh[0], lh[1]));
        if (m_recipeMap.end() != itRecipe)
        {
            return itRecipe->second;
        }
    }
    return 0;
}