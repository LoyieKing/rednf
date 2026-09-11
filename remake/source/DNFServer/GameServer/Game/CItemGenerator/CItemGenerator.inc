// ============================================================================
// CItemGenerator —— ORIG ctor 0x08533b3e：
//   vptr（编译器自动）→ m_random(+0x08, CGenerateRandomNumber(count)) →
//   m_upgrade(+0x10)。注意 ORIG 不初始化 +0x04 m_common 与 +0x14 type。
// dtor 0x08533b9a：重置 vptr → 析构 m_upgrade → 析构 m_random（非虚析构，
//   成员析构顺序由编译器保证）。
// 6 个 W inline 方法体均来自 objdump 直译（空实现/字段读写）。
// ============================================================================
CItemGenerator::CItemGenerator(int count)
    : m_random(count)
{
}