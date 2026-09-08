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

static const unsigned long MT_UPPER_MASK = 0x80000000UL;
static const unsigned long MT_LOWER_MASK = 0x7fffffffUL;

class CMTRand
{
public:
    CMTRand();
    CMTRand(const unsigned long& s);

    void seed(unsigned long s);
    void seed(unsigned long* init_key, unsigned long key_length);
    void seed();

    int randInt();
    unsigned long randInt(const unsigned long& range);   // ORIG 返回 ulong（调用侧 setb 实证）

    static unsigned long hash(long a, long b);

private:
    void initialize(unsigned long s);
    void reload();

    // ORIG hiBit/loBit/loBits 均为 W 0x080cb97c/0x080cb98c/0x080cb99a =
    // 类内 inline 定义（P147 口径）；类外定义致 mixBits outgoing 0x8 ≠
    // ORIG 0x14（探针 p4/p5 实测类内 0x14 / 类外 0x8）。
    unsigned long hiBit(const unsigned long& x) const { return x & MT_UPPER_MASK; }
    unsigned long loBit(const unsigned long& x) const { return x & 1UL; }
    unsigned long loBits(const unsigned long& x) const { return x & MT_LOWER_MASK; }
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

// [b28d-L14 N82/算法类] ORIG 0x080cbb90：MT19937 通用算法（§6 算法正确即豁免）；
// ORIG 用寄存器指针遍历，4.4 槽化，gap 59 vs 83。
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

unsigned long CMTRand::mixBits(const unsigned long& x,
                               const unsigned long& y) const
{
    return hiBit(x) | loBits(y);
}

unsigned long CMTRand::twist(const unsigned long& a, const unsigned long& b,
                             const unsigned long& c) const
{
    // [b28c-21] ORIG mov (%eax),%ebx 单步缓存 + 全程 %ebx 累计；纯 register
    // 关键字（非 register asm 绑定）令 4.4-O0 复现同形（probe1.cpp fA 逐条全等）。
    register unsigned long y = a;
    y ^= mixBits(b, c) >> 1;
    // ORIG neg+and+xor 无分支形：(0-t)&mask 复现；末步作为返回表达式直落 %eax
    // （写回 y 会多 mov %ebx,%eax）。
    return y ^ ((0UL - loBit(c)) & MT_MATRIX_A);
}

int CMTRand::randInt()
{
    if (m_left == 0)
    {
        reload();
    }

    --m_left;

    // [b28c-21] ORIG mov (%eax),%ebx 跨段缓存；纯 register 关键字复现（P0 限定：
    // 仅 register asm("reg") 绑定禁用；纯 register 为标准 C++ 存储建议）。
    register unsigned long y = *m_next++;
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    // 末步不写回 y：ORIG xor %ebx,%eax 直落返回值。
    return (int)(y ^ (y >> 18));
}

unsigned long CMTRand::randInt(const unsigned long& range)
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
        } while (v > range);   // ORIG cmp/setb：v<=range 时退出
    return v;
}

// [b28d-L14 N82/算法类] ORIG 0x080d9f5c：init_by_array 通用算法（§6 豁免）；
// ORIG 寄存器指针遍历形，4.4 槽化，gap 129 vs 104。
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
        // [b28d-L14 实证] ORIG 0x080da0e6：纯 register 指针/计数/标志循环
        // （p=%edi、n=%esi、ok=%ebx），条件形 `ok && n-- != 0`（setne 先于 sub），
        // 命中条件为 fread(...)!=0（setne %bl）。按 ORIG 逐条转写。
        register unsigned long* p = buf;
        register int n = MT_N;
        register bool ok = true;
        while (ok && n-- != 0)
        {
            ok = fread(p++, sizeof(unsigned long), 1, fp) != 0;
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
    for (unsigned int i = 0; i <= 3; ++i)
    {
        // [b28d-L14 实证] ORIG 0x080da1d6 分两句（先 `(hashA<<8)+hashA` 落槽，
        // 再 `add p[i],hashA`），循环变量 unsigned（setbe）。
        hashA = hashA + (hashA << 8);
        hashA += p[i];
    }

    unsigned long hashB = 0;
    p = (const unsigned char*)&b;
    for (unsigned int i = 0; i <= 3; ++i)
    {
        hashB = hashB + (hashB << 8);
        hashB += p[i];
    }

    return (differ++ + hashA) ^ hashB;
}
