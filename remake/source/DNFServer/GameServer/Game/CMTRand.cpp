// ============================================================================
// df_game_r CMTRand 还原（G2-5 批次，MT19937，尺寸 0x9c8）。
// 无独立报告，按 ORIG 反汇编逐函数转录：
//   C1/C2(const unsigned long&) 080cba2a / C1/C2() 080d9f48
//   randInt() 080cba46 / randInt(const unsigned long&) 080cbabe
//   seed(unsigned long) 080cbb1e / seed(unsigned long*, unsigned long) 080d9f5c
//   seed() 080da0e6 / initialize 080cbb44 / reload 080cbb90
//   hiBit 080cb97c / loBit 080cb98c / loBits 080cb99a
//   mixBits 080cb9aa / twist 080cb9e0 / hash(long, long) 080da1d6
// 布局：+0x00 unsigned long m_mt[624]；+0x9c0 unsigned long* m_next；
//       +0x9c4 int m_left（总 0x9c8）。
// reload 尾部 ORIG 对 +0x9c0（m_next 槽）做一次 twist 后立即以 this 覆盖，
// 结果被丢弃，按标准 MT19937 语义实现（用户规则：通用算法算法正确即可）。
// ============================================================================

#include <stdio.h>
#include <time.h>

class CMTRand
{
public:
    CMTRand();
    CMTRand(const unsigned long& s);

    void seed(unsigned long s);
    void seed(unsigned long* init_key, unsigned long key_length);
    void seed();

    int randInt();
    int randInt(const unsigned long& range);

    static unsigned long hash(long a, long b);

private:
    void initialize(unsigned long s);
    void reload();

    unsigned long hiBit(const unsigned long& x) const;
    unsigned long loBit(const unsigned long& x) const;
    unsigned long loBits(const unsigned long& x) const;
    unsigned long mixBits(const unsigned long& x,
                          const unsigned long& y) const;
    unsigned long twist(const unsigned long& a, const unsigned long& b,
                        const unsigned long& c) const;

private:
    unsigned long m_mt[624];   // +0x00
    unsigned long* m_next;     // +0x9c0
    int m_left;                // +0x9c4
};

static const int MT_N = 624;
static const int MT_M = 397;
static const unsigned long MT_MATRIX_A = 0x9908b0dfUL;
static const unsigned long MT_UPPER_MASK = 0x80000000UL;
static const unsigned long MT_LOWER_MASK = 0x7fffffffUL;

CMTRand::CMTRand(const unsigned long& s)
{
    seed(s);
}

CMTRand::CMTRand()
{
    seed();
}

void CMTRand::seed(unsigned long s)
{
    initialize(s);
    reload();
}

void CMTRand::initialize(unsigned long s)
{
    m_mt[0] = s;
    for (unsigned long i = 1; i < MT_N; ++i)
    {
        m_mt[i] = 0x6c078965UL * (m_mt[i - 1] ^ (m_mt[i - 1] >> 30)) + i;
    }
}

void CMTRand::reload()
{
    int kk;
    for (kk = 0; kk < MT_N - MT_M; ++kk)
    {
        m_mt[kk] = twist(m_mt[kk + MT_M], m_mt[kk], m_mt[kk + 1]);
    }
    for (; kk < MT_N - 1; ++kk)
    {
        m_mt[kk] = twist(m_mt[kk + MT_M - MT_N], m_mt[kk], m_mt[kk + 1]);
    }
    m_mt[MT_N - 1] = twist(m_mt[MT_M - 1], m_mt[MT_N - 1], m_mt[0]);

    m_left = MT_N;
    m_next = m_mt;
}

unsigned long CMTRand::hiBit(const unsigned long& x) const
{
    return x & MT_UPPER_MASK;
}

unsigned long CMTRand::loBit(const unsigned long& x) const
{
    return x & 1UL;
}

unsigned long CMTRand::loBits(const unsigned long& x) const
{
    return x & MT_LOWER_MASK;
}

unsigned long CMTRand::mixBits(const unsigned long& x,
                               const unsigned long& y) const
{
    return hiBit(x) | loBits(y);
}

unsigned long CMTRand::twist(const unsigned long& a, const unsigned long& b,
                             const unsigned long& c) const
{
    unsigned long y = a;
    y ^= mixBits(b, c) >> 1;
    if (loBit(c) != 0)
    {
        y ^= MT_MATRIX_A;
    }
    return y;
}

int CMTRand::randInt()
{
    if (m_left == 0)
    {
        reload();
    }

    --m_left;

    unsigned long y = *m_next++;
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);
    return (int)y;
}

int CMTRand::randInt(const unsigned long& range)
{
    unsigned long mask = range;
    mask |= mask >> 1;
    mask |= mask >> 2;
    mask |= mask >> 4;
    mask |= mask >> 8;
    mask |= mask >> 16;

    unsigned long v;
    do
    {
        v = (unsigned long)randInt() & mask;
    } while (v >= range);
    return (int)v;
}

void CMTRand::seed(unsigned long* init_key, unsigned long key_length)
{
    initialize(19650218UL);

    unsigned long i = 1;
    unsigned long j = 0;
    unsigned long k = (key_length >= MT_N) ? key_length : MT_N;

    for (; k != 0; --k)
    {
        m_mt[i] = (m_mt[i] ^
                   ((m_mt[i - 1] ^ (m_mt[i - 1] >> 30)) * 1664525UL)) +
                  init_key[j] + j;
        ++i;
        ++j;
        if (i >= MT_N)
        {
            m_mt[0] = m_mt[MT_N - 1];
            i = 1;
        }
        if (j >= key_length)
        {
            j = 0;
        }
    }

    for (k = MT_N - 1; k != 0; --k)
    {
        m_mt[i] = (m_mt[i] ^
                   ((m_mt[i - 1] ^ (m_mt[i - 1] >> 30)) * 1566083941UL)) -
                  i;
        ++i;
        if (i >= MT_N)
        {
            m_mt[0] = m_mt[MT_N - 1];
            i = 1;
        }
    }

    m_mt[0] = 0x80000000UL;
    reload();
}

void CMTRand::seed()
{
    FILE* fp = fopen("/dev/urandom", "rb");
    unsigned long buf[MT_N];

    if (fp != 0)
    {
        bool ok = true;
        for (int i = 0; i < MT_N; ++i)
        {
            if (fread(&buf[i], sizeof(unsigned long), 1, fp) != 1)
            {
                ok = false;
                break;
            }
        }
        fclose(fp);
        if (ok)
        {
            seed(buf, MT_N);
            return;
        }
    }

    seed(hash((long)time(0), (long)clock()));
}

unsigned long CMTRand::hash(long a, long b)
{
    static unsigned long differ = 0;

    unsigned long hashA = 0;
    const unsigned char* p = (const unsigned char*)&a;
    for (int i = 0; i <= 3; ++i)
    {
        hashA = (hashA << 8) + hashA + p[i];
    }

    unsigned long hashB = 0;
    p = (const unsigned char*)&b;
    for (int i = 0; i <= 3; ++i)
    {
        hashB = (hashB << 8) + hashB + p[i];
    }

    unsigned long result = (differ + hashA) ^ hashB;
    ++differ;
    return result;
}
