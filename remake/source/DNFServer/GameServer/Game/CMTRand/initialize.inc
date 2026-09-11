void CMTRand::initialize(unsigned long s)
{
    // [b28d-L14 实证] ORIG 0x080cbb44 为**纯寄存器**指针遍历（无栈帧）：dst=%ebx、
    // src=%esi、i=%edi（int 有符号 ⇒ setle），`+ i` 经 mov+lea（int→ulong 转换）。
    // 探针 /tmp/l14_probe3.cpp a 形逐条全等（含 push edi/esi/ebx 三件套与
    // setle 尾测）；仅增量序按 ORIG 改为 dst→src→i。
    register unsigned long* dst = m_mt;
    register unsigned long* src = m_mt;
    register int i = 1;
    *dst = s;
    ++dst;
    while (i < MT_N)
    {
        *dst = 0x6c078965UL * (*src ^ (*src >> 30)) + i;
        ++dst;
        ++src;
        ++i;
    }
}