// ORIG W 0x0854b496 C1/C2：仅尾调 upgrade_table_t::C1(0x089000ca)，由编译器
// 生成的基类构造调用即等价，函数体留空并由基类构造完成初始化。
// [R18rti · B3] 基类 → 成员 m_table（RTTI 由 si 变 plain class，布局/调用形不变）。
CItemUpgradeTable::CItemUpgradeTable()
    : m_table()
{
}