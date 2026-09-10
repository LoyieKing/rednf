CSecialIMonsterDropItemGenRateTable::CSecialIMonsterDropItemGenRateTable()
    : m_rand(0)
{
    // ORIG 0x0853a0b6：map 循环 → rand=0(+0x211fb0) → Handle(+0x211fb4) → init()。
    init();
