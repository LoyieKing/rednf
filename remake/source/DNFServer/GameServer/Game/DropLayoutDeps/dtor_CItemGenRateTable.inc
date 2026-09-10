CItemGenRateTable::~CItemGenRateTable()
{
    // ORIG 0x0853a030：destroy() → Handle D1 → map D1（后两者成员自动）。
    destroy();
