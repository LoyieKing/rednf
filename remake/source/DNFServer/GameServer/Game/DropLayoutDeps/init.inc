void CItemGenRateTable::init()
{
    // [b28d-solo-DropLayoutDeps] 封 N18a（ORIG 0x085345b6）：new 表达式实参地址
    // 求值序跨版本——ORIG 先 `lea -0x1c(%ebp),%esi` 跨 operator new 保活，4.4 恒
    // 分配后求实参（`lea …%edx` 在 new 之后）。本轮实测 P226 裸语句表达式
    // `new CMTRand(*({&seed;}))` 无效（同 43 条，序不变）。语义等价。
    if (m_rand != 0)
        delete m_rand;
    unsigned long seed = time(0);
    register unsigned long* pSeed = &seed;
    m_rand = new CMTRand(*pSeed);
