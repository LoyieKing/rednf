stGenerateResult_t::~stGenerateResult_t() {}

// ============================================================================
// CGenerateRandomNumber —— ORIG ctor 0x085334ea：
//   m_count=count；count!=0 时 new CMTRand[count]（逐元素默认构造）；
//   随后第二循环逐元素播种：seed = time(0) + seedIncValue + i（函数局部
//   static seedIncValue，bss 符号 _ZZN8WongWork21CGenerateRandomNumberC1EiE
//   12seedIncValue 与 ORIG 同名），末尾 seedIncValue += count。
// dtor 0x085335d0：裸 delete []（delete[] 内置空检；ORIG 仅一次 test/je）。
// generateNumber 0x085334a4：idx 超界走 get_rand_int(range)；否则对元素
//   m_table[idx]（CMTRand，0x9c8 字节）调 randInt((unsigned long)range)。
// ============================================================================
