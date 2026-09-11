void CSHA1::Final()
{
    // ORIG 0x0808a2d2：0x80 填充 + 0x00 补位至 (count&0x1f8)==0x1c0 + 8 字节大端位长，
    // 摘要按大端写入 m_digest[20]。
    unsigned char tail[8];
    // ORIG 0x0808a2e6：`cmpl $0x3,i; setbe; add $0x4,%ecx; mov 0x4(%this,%ecx,4)` ⇒
    // i<=3 取槽 5（+0x18 m_countHi）、i>3 取槽 4（+0x14 m_countLo）；位移 (~i&3)<<3。
    // [真 bug 修复] 旧实现写成 lo 在前，SHA-1("abc") 实测 c3173c3d…（应 a9993e36…）。
    for (int i = 0; i < 8; ++i)
    {
        unsigned int v = (i < 4) ? m_countHi : m_countLo;
        tail[i] = (unsigned char)(v >> ((3 - (i & 3)) * 8));
    }
    static unsigned char const pad0x80[1] = { 0x80 };
    Update(pad0x80, 1);
    static unsigned char const padZero[1] = { 0x00 };
    while ((m_countLo & 0x1f8u) != 0x1c0u)
        Update(padZero, 1);
    Update(tail, 8);

    for (int i = 0; i < 0x14; ++i)
    {
        unsigned int v = m_state[i >> 2];
        m_digest[i] = (unsigned char)(v >> ((3 - (i & 3)) * 8));
    }
}