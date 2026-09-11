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


#include "CMTRand/CMTRand_1.inc"


#include "CMTRand/seed.inc"


#include "CMTRand/initialize.inc"


#include "CMTRand/reload.inc"


#include "CMTRand/mixBits.inc"


#include "CMTRand/twist.inc"


#include "CMTRand/randInt.inc"


#include "CMTRand/randInt_1.inc"


#include "CMTRand/seed_1.inc"


#include "CMTRand/seed_2.inc"


#include "CMTRand/hash.inc"

