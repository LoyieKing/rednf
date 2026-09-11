// ORIG 0x08694160 (sz 0x6c):
//   if (m_offset + count >= m_size) return 0;
//   for (i=0; i<count; ++i) { dst[i] = m_buffer[m_offset++]; }
//   return 1;
// [b28d-D9] N18a/P200：ORIG 值先 eax 再中转 edx + m_buffer 缓存 %ebx；4.4 单段/不同寄存器（ORIG 0x08694160 sz 0x0000006c）
// [Q8 2026-09-11 旧N判词重扫] 旧判=DIFF → n1_v 新口径 AE（gap=+0，blocks=0）⇒ 旧判词作废。
bool CPacketHelper::scan(unsigned char* dst, int count)
{
    if (static_cast<int>(static_cast<unsigned>(m_offset)) + count >= m_size)
        return false;
    // [S4NexonProto] 后缀自增形命中 ORIG 全形（探针 /tmp/probe/scn.cpp S2 对 S1 对照）：
    // ORIG 0x086941a8 `lea 0x1(%eax),%edx`（旧 m_offset 留存于 %eax 供回写）+
    // `mov 0x8(%eax),%ebx`（m_buffer 缓存 callee-saved %ebx，故有 push/pop %ebx 与
    // i 槽 -0x8）——旧两句式 `dst[i]=m_buffer[m_offset]; ++m_offset;` 使 4.4 在 edx 中
    // 就地算地址并重载 m_offset（38 条），后缀式令 4.4 保留旧值 → 41 条全等。
    for (int i = 0; i < count; ++i)
        dst[i] = static_cast<char>(m_buffer[m_offset++]);
    return true;
}