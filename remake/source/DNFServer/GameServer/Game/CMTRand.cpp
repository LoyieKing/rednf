// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CMTRand/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r CMTRand 还原（G2-5 批次，MT19937，尺寸 0x9c8）。
// [2026-09-10 round5 复核] 本 TU 三件（seed/hash/reload）全部为 §6 通用算法类
//   （MT19937 init_by_array / hash / reload）：判词维持 N——seed 129 vs 103
//   （ORIG 用寄存器承载 i/j/k + `cmp $0x270,%eax; jae` 分支形，已加纯 register
//   i/j/k 后 104→103；4.4 仍产 cmovae，-march=i486/-mtune=generic A/B 实测本 TU
//   计数不变）、reload 59 vs 81（ORIG 内联 twist + 寄存器指针遍历）、hash 54 vs 50
//   （ORIG 全栈槽形，加 register 反劣 54 条/ndiff 12→58 ⇒ 已回退）。算法正确性优先。
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

#include "CMTRand/CMTRand.inc"
}

#include "CMTRand/CMTRand_1.inc"
}

#include "CMTRand/seed.inc"
}

#include "CMTRand/initialize.inc"
}

// [b28d-L14 N82/算法类] ORIG 0x080cbb90：MT19937 通用算法（§6 算法正确即豁免）；
// ORIG 用寄存器指针遍历，4.4 槽化，gap 59 vs 83。
// [round3-L29 2026-09-10 复核] 算法类（§6 豁免）。ORIG 59 条为**内联 twist + 寄存器指针遍历**
//   （%ebx=this、%esi=kk=0xe3）；我方 81 条（register kk 后 83→81）⇒ N82 槽/寄存器族。
#include "CMTRand/reload.inc"
}

#include "CMTRand/mixBits.inc"
}

#include "CMTRand/twist.inc"
}

#include "CMTRand/randInt.inc"
}

#include "CMTRand/randInt_1.inc"
}

// [b28d-L14 N82/算法类] ORIG 0x080d9f5c：init_by_array 通用算法（§6 豁免）；
// ORIG 寄存器指针遍历形，4.4 槽化，gap 129 vs 104。
// [round3-L29 2026-09-10 复核] 算法类（§6）。ORIG 0x080da0?? 用寄存器承载 i/j/k
//   （`mov $0x1,%ebx; mov $0x0,%edi; mov %eax,%esi`）+ `cmp $0x270,%eax; jae` 分支形；
//   已加纯 `register`（P231，非绑定）i/j/k：104→103 条（ORIG 129）；4.4 仍产 cmovae
//   （`-march=i486 -mtune=generic` A/B 实测本 TU 计数不变 ⇒ 路由变更无收益，未上报改路由）。
#include "CMTRand/seed_1.inc"
}

#include "CMTRand/seed_2.inc"
}

// [round3-L29 2026-09-10 复核] 算法类（§6 豁免口径）。ORIG 0x080da1d6 为**全栈槽**形
//   （hashA@-0x14/i@-0x8/p@-0x10 槽往返）；加 `register` 会反向变寄存器形（54 条 vs 原 50 条、
//   ndiff 12→58）⇒ 已回退保持槽形。残差 = 两端槽/寄存器分配差（N82 族）。
#include "CMTRand/hash.inc"
}
