stBloodDungeon_t::~stBloodDungeon_t() {}
// [b28d-solo-CDataManagerScripts] AE：P310 —— `for (int i = 0; (unsigned int)i <= 199; ++i)` 复现 ORIG 两段寄存器尾测（`mov -0xc(%ebp),%eax; cmp $0xc7,%eax; setbe`）；fresh clean 42=42；ORIG 0x08370b02。
