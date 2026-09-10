CItemGenRateTable::CItemGenRateTable()
    : m_rand(0)
{
    // ORIG 0x08539e66：map 循环构造（成员自动）→ m_rand=0 → Handle 构造（自动）→ init()。
    init();
