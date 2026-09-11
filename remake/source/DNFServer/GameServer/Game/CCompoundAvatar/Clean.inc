// ORIG 0x0891ab56：标量清零（含 memset 0x28 于 +0x04）+ 三组/尾向量逐个 clear，
// 循环体内顺序为 A.clear/B.clear/m_b/m_c/m_d/C.clear，尾部顺序 2b8/2dc/2e0[]/2c4/2d0。
void tagCompoundAvatar::Clean()
{
    m_idx = 0;
    std::memset(&m_a[0], 0, sizeof(m_a));
    for (int i = 0; i < 10; ++i)
    {
        m_optsA[i].clear();
        m_optsB[i].clear();
        m_b[i] = 0;
        m_c[i] = 0;
        m_d[i] = 0;
        m_optsC[i].clear();
    }
    m_tailA.clear();
    m_flag2dc = 0;
    for (int j = 0; j < 10; ++j)
    {
        m_e[j] = 0;
    }
    m_tailB.clear();
    m_tailC.clear();
}