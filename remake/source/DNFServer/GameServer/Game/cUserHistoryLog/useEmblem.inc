void cUserHistoryLog::useEmblem(unsigned long a, unsigned long b, unsigned int c,
                                const char* d, unsigned int e)
{
    // b28c-L2: 删 (int) 冗余 cast——-m32 下 unsigned long 即 32 位，cast 不改变
    // 压栈宽度但诱发 4.4 callee-saved %ebx 缓存 + 帧 0x28→0x24（P41 变体/BM13
    // charge 同族），删后 4.4 回到 ORIG 0x08685a08 的 edx 逐段即用即弃形。
    m_user->LogHistory("UseEmblem,%d,%d,%d,%s,%d", a, b, c, d, e);
}