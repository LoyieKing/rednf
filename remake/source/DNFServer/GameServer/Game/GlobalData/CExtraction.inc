// [b28d-18 IDENTICAL_AE] 修：register 指针 pt=&t 使 &t 提前物化 %esi 跨 operator new 保活（P231 新实证）；NDIFF 4→1（余 vtable 重定位伪差） | ORIG 0x084a2938
expert_extraction::CExtraction::CExtraction()
{
    // ORIG 0x084a2938：vptr + new CMTRand 并以 time(0) seed。
    // 0x084a296c：CMTRand ctor 形参为 const unsigned long&（mangled ERKm，
    // 32 位 ulong=4B → time 结果单 dword 落槽 0x084a2956、lea 槽址作实参）。
    unsigned long t = (unsigned long)time(0);
    // ORIG 0x084a2959：seed 槽地址提前物化 %esi —— 4.4 对实参 &t 表达式
    // 自然产 lea（P0：寄存器仅承载表达式值）。
    register unsigned long* pt = &t;
    m_rand = new CMTRand(*pt);
}