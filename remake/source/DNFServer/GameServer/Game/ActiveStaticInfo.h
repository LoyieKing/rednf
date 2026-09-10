#ifndef DNF_GAME_ACTIVESTATICINFO_H_
#define DNF_GAME_ACTIVESTATICINFO_H_

#include <cstddef>   // offsetof（布局断言）

// G2 占位（TODO：真实实现属怪物系统批次）。
// 尺寸 0x2f0：map_monster 内嵌成员 @ +0x38..+0x328（ctor 0x8150ec6 / dtor 0x8150ef6）。

class ActiveStaticInfo
{
public:
    ActiveStaticInfo();
    ~ActiveStaticInfo();
    void clear();
    int getHpMax();
    int getMpMax();
    int m_level;
    int m_hpBase;
    float m_hpRatio;
    int m_mpBase;
    float m_mpRatio;
    char m_flag14;                     // +0x14（ORIG clear movb $1,0x14；byte 非 int）
    char m_pad15[3];                   // +0x15
    int m_attack;
    float m_attackRatio;
    int m_magicAttack;
    float m_magicAttackRatio;
    int m_defense;
    float m_defenseRatio;
    int m_magicDefense;
    float m_magicDefenseRatio;
    int m_elemAttack[4];
    float m_elemAttackRatio[4];
    int m_elemDefense[4];
    float m_elemDefenseRatio[4];
    int m_attr17Base[17];
    float m_attr17Ratio[17];
    int m_statBase0; float m_statRatio0;
    int m_statBase1; float m_statRatio1;
    int m_statBase2; float m_statRatio2;
    int m_statBase3; float m_statRatio3;
    int m_statBase4; float m_statRatio4;
    int m_statBase5; float m_statRatio5;
    int m_statBase6; float m_statRatio6;
    int m_statBase7; float m_statRatio7;
    int m_statBase8; float m_statRatio8;
    int m_statBase9; float m_statRatio9;
    int m_statBase10; float m_statRatio10;
    int m_statBase11; float m_statRatio11;
    int m_statBase12; float m_statRatio12;
    int m_statBase13; float m_statRatio13;
    int m_statBase14; float m_statRatio14;
    // [A24TailBatch2] 下列 14 个 4 字节成员 ORIG `clear` 0x08150a.. 用 **float 常量 store
    // 中转形** `mov $0x0,%edx; mov %edx,N(%eax)`（P238 判据：int/unsigned 字段 4.4 恒直写
    // `movl $0x0,N(%eax)`）⇒ 真型为 float（宽度/偏移/sizeof/mangled 零变化）。
    // 逐个偏移直证：+0x180 / +0x18c / +0x190 / +0x194 / +0x198 / +0x19c / +0x1a0 /
    // +0x1a4 / +0x1ac / +0x1b0 / +0x1b4 / +0x1d4 / +0x2cc / +0x2d0。
    int m_statBase15; int m_statBase16; float m_statBase17; int m_statBase18;
    int m_statBase19; float m_statBase20; float m_statBase21; float m_statBase22;
    float m_statBase23; float m_statBase24; float m_statBase25; float m_statBase26;
    float m_statBase27;                // +0x1a8（ORIG flds 0x1a8 浮点直读→float 实证）
    float m_statBase28; float m_statBase29; float m_statBase30;
    float m_statRatio15; int m_statBase31; int m_statBase32; int m_statBase33;
    int m_statBase34; float m_statRatio16; int m_statBase35; float m_statBase36;
    float m_statRatio17; int m_statBase37;
    int m_table1[18]; int m_table2[18]; int m_table3[18];
    int m_field2b8[5];                 // +0x2b8（ORIG clear 尾循环 k≤4 逐项清零；
                                       //   缺失曾使 ASI=0x2e0/map_monster 整体 -0x10 偏移）
    float m_f2cc; float m_f2d0; int m_f2d4; int m_f2d8;   // +0x2cc/+0x2d0 float（同上证）
    float m_f2dc; float m_f2e0; int m_f2e4; int m_f2e8; float m_f2ec;
};

static_assert(sizeof(ActiveStaticInfo) == 0x2f0, "ActiveStaticInfo size");
// [A24TailBatch2] 布局锁（改 14 个成员 int→float 后逐点复核，全部与 ORIG 反汇编偏移一致）
static_assert(offsetof(ActiveStaticInfo, m_level) == 0x00, "off m_level");
static_assert(offsetof(ActiveStaticInfo, m_hpBase) == 0x04, "off m_hpBase");
static_assert(offsetof(ActiveStaticInfo, m_hpRatio) == 0x08, "off m_hpRatio");
static_assert(offsetof(ActiveStaticInfo, m_flag14) == 0x14, "off m_flag14");
static_assert(offsetof(ActiveStaticInfo, m_elemAttack) == 0x38, "off m_elemAttack");
static_assert(offsetof(ActiveStaticInfo, m_attr17Base) == 0x78, "off m_attr17Base");
static_assert(offsetof(ActiveStaticInfo, m_statBase0) == 0x100, "off m_statBase0");
static_assert(offsetof(ActiveStaticInfo, m_statBase17) == 0x180, "off m_statBase17");
static_assert(offsetof(ActiveStaticInfo, m_statBase20) == 0x18c, "off m_statBase20");
static_assert(offsetof(ActiveStaticInfo, m_statBase27) == 0x1a8, "off m_statBase27");
static_assert(offsetof(ActiveStaticInfo, m_statBase36) == 0x1d4, "off m_statBase36");
static_assert(offsetof(ActiveStaticInfo, m_statBase37) == 0x1dc, "off m_statBase37");
static_assert(offsetof(ActiveStaticInfo, m_table1) == 0x1e0, "off m_table1");
static_assert(offsetof(ActiveStaticInfo, m_table2) == 0x228, "off m_table2");
static_assert(offsetof(ActiveStaticInfo, m_table3) == 0x270, "off m_table3");
static_assert(offsetof(ActiveStaticInfo, m_field2b8) == 0x2b8, "off m_field2b8");
static_assert(offsetof(ActiveStaticInfo, m_f2cc) == 0x2cc, "off m_f2cc");
static_assert(offsetof(ActiveStaticInfo, m_f2ec) == 0x2ec, "off m_f2ec");

#endif  // DNF_GAME_ACTIVESTATICINFO_H_
