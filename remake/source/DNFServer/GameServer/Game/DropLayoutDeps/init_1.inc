void CSecialIMonsterDropItemGenRateTable::init()
{
    // [b28d-solo-DropLayoutDeps] 封 N18a（ORIG 0x08534c1a）：与 CItemGenRateTable::init
    // 同款（lea -0x1c,%esi 跨 new 的 4.1 求值序不可造）。语义等价。
    if (m_rand != 0)
        delete m_rand;
    unsigned long seed = time(0);
    register unsigned long* pSeed = &seed;
    m_rand = new CMTRand(*pSeed);
}