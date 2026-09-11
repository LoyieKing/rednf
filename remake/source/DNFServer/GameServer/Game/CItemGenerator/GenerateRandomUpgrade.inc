// ============================================================================
// GenerateRandomUpgrade —— ORIG ctor 0x085335ee：
//   time(0) 取种子（存局部）→ operator new(0x9c8)（一个 CMTRand）→
//   CMTRand::CMTRand(this, &seed)（带种子构造，const unsigned long&）。
// dtor 0x0853364e：m_rand 非空则 operator delete，随后置 0。
// ============================================================================
GenerateRandomUpgrade::GenerateRandomUpgrade()
    : m_rand(new CMTRand(static_cast<unsigned long>(time(0))))
{
}