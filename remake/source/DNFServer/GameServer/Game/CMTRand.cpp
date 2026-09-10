// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMTRand/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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

#include "CMTRand/CMTRand.inc"
{
    seed(s);
}

#include "CMTRand/CMTRand_1.inc"
{
    seed();
}

#include "CMTRand/seed.inc"
{
    initialize(s);
    reload();
}

#include "CMTRand/initialize.inc"
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
// [round3-L29 2026-09-10 复核] 算法类（§6 豁免）。ORIG 59 条为**内联 twist + 寄存器指针遍历**
//   （%ebx=this、%esi=kk=0xe3）；我方 81 条（register kk 后 83→81）⇒ N82 槽/寄存器族。
#include "CMTRand/reload.inc"
{
    // [round3-L29] P231：ORIG 以 %esi 承载 kk（`mov $0xe3,%esi`），纯 register 复现。
    register int kk;
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

#include "CMTRand/mixBits.inc"
                               const unsigned long& y) const
{
    return hiBit(x) | loBits(y);
}

#include "CMTRand/twist.inc"
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

#include "CMTRand/randInt.inc"
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

#include "CMTRand/randInt_1.inc"
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
// [round3-L29 2026-09-10 复核] 算法类（§6）。ORIG 0x080da0?? 用寄存器承载 i/j/k
//   （`mov $0x1,%ebx; mov $0x0,%edi; mov %eax,%esi`）+ `cmp $0x270,%eax; jae` 分支形；
//   已加纯 `register`（P231，非绑定）i/j/k：104→103 条（ORIG 129）；4.4 仍产 cmovae
//   （`-march=i486 -mtune=generic` A/B 实测本 TU 计数不变 ⇒ 路由变更无收益，未上报改路由）。
#include "CMTRand/seed_1.inc"
{
    initialize(19650218UL);

    // [round3-L29] P231：ORIG 以寄存器承载 i/j/k（`mov $0x1,%ebx; mov $0x0,%edi;
    //   mov %eax,%esi`），纯 register 存储建议复现免槽形（非绑定，P0 合规）。
    // [轴 B 2026-09-10 · a_S->U 清单命中件] ORIG 080da007 `cmp $0x26f,%ebx; jle
    //   080da022` 为**有符号**序（%ebx 承载 i）⇒ i 须为 `int`；旧的
    //   `unsigned long i` 使 4.4 产无符号 `jb`（P196 前置已核：ORIG 有符号 ⇒ 允许改）。
    // [AxisB3 属主复核 2026-09-10 ⇒ **保留**] 地址自校验通过：`nm -S` 得
    //   `_ZN7CMTRand4seedEPmm` = [0x080d9f5c, +0x18a)，清单 orig_addr 0x080da00d
    //   落在区间内；我方 .o 该尾测现为 `jle`（原 `jb`）⇒ 符号性已对齐。
    //   实测：our 103→104（gap 26→25，向 ORIG 129 收敛）；TU 四数 3/9/0/3/2
    //   逐值不变 ⇒ 不劣 ⇒ 保留。
    //   §6 算法豁免域（MT19937 init_by_array）标注：改动仅局部变量**类型**
    //   （`unsigned long i`→`int i`），i 值域 [1, MT_N-1] = [1,623]，int 完全覆盖、
    //   无溢出/无符号分叉 ⇒ **算法语义不变、算法正确**；其余 byte 层差异
    //   （ORIG 129 vs 我方 104，寄存器指针遍历形 vs 4.4 槽化 + cmovae）按 §6
    //   「通用算法类只保证算法正确，不强制 identical」封顶，不计入回归。
    register int i = 1;
    register unsigned long j = 0;
    register unsigned long k = (key_length >= MT_N) ? key_length : MT_N;

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

#include "CMTRand/seed_2.inc"
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

// [round3-L29 2026-09-10 复核] 算法类（§6 豁免口径）。ORIG 0x080da1d6 为**全栈槽**形
//   （hashA@-0x14/i@-0x8/p@-0x10 槽往返）；加 `register` 会反向变寄存器形（54 条 vs 原 50 条、
//   ndiff 12→58）⇒ 已回退保持槽形。残差 = 两端槽/寄存器分配差（N82 族）。
#include "CMTRand/hash.inc"
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
