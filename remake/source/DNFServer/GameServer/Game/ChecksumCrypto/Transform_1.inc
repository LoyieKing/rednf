void CSHA1::Transform(unsigned int* state, unsigned char const* block)
{
    // ORIG 0x08087daa：块经 m_blockPtr 大端读入展开为 W[80]，四轮 80 步压缩。
    static const unsigned int K[4] = { 0x5a827999u, 0x6ed9eba1u, 0x8f1bbcdcu, 0xca62c1d6u };
    unsigned int w[80];
    for (int i = 0; i < 16; ++i)
    {
        w[i] = ((unsigned int)block[i * 4] << 24) | ((unsigned int)block[i * 4 + 1] << 16)
             | ((unsigned int)block[i * 4 + 2] << 8) | (unsigned int)block[i * 4 + 3];
    }
    for (int i = 16; i < 80; ++i)
    {
        unsigned int v = w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16];
        w[i] = v << 1 | v >> 31;
    }
    unsigned int a = state[0], b = state[1], c = state[2], d = state[3], e = state[4];
    for (int i = 0; i < 80; ++i)
    {
        unsigned int f;
        if (i < 20)      f = (d ^ (b & (c ^ d))) + K[0];       // ORIG 0x5a827999 轮
        else if (i < 40) f = (b ^ c ^ d) + K[1];               // 0x6ed9eba1
        else if (i < 60) f = ((b | c) & d | b & c) + K[2];     // 0x8f1bbcdc
        else             f = (b ^ c ^ d) + K[3];               // 0xca62c1d6
        unsigned int tmp = (a << 5 | a >> 27) + f + e + w[i];
        e = d;
        d = c;
        c = b << 30 | b >> 2;
        b = a;
        a = tmp;
    }
    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}