void CSHA1::Reset()
{
    // ORIG 0x08087d60：SHA-1 初始链接变量 + 位计数清零。
    m_state[0] = 0x67452301u;
    m_state[1] = 0xefcdab89u;
    m_state[2] = 0x98badcfeu;
    m_state[3] = 0x10325476u;
    m_state[4] = 0xc3d2e1f0u;
    m_countLo  = 0;
    m_countHi  = 0;
}