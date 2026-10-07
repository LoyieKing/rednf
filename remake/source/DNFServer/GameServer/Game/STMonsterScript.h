#ifndef GAME_STMONSTERSCRIPT_H_
#define GAME_STMONSTERSCRIPT_H_

// ============================================================================
// df_game_r 还原 —— STMonsterScript（G5 怪物脚本表，sizeof == 0x1cc4）。
// 布局证据：ctor W 0x08368e14（逐成员构造顺序）、Clear T 0x08a398aa（全字段
// 默认值）、dtor W 0x08378dac（析构逆序）、op= W 0x08377994；成员类型大小取自
// 各自 clear/C1 最大偏移（ORIG 符号 _ZN..5clearEv/_ZN..C1Ev，地址随注）。
// 语义命名证据：exportMonsterScript T 0x08a22520 的 fprintf 格式串 ↔ 成员偏移
// 配对、setBasicAbilityByLevel T 0x088b9603 读取面、getRandomChampionElement
// T 0x088b9cce（+0xc40 vector<int>）、getMonsterTableIndex T 0x088ba1c8
// （+0x3bc）。m_fieldNNN 仅用于暂无语义证据的已知字节；m_padNN 为未建模区。
// ============================================================================

#include <deque>
#include <map>
#include <string>
#include <vector>

#include "ActiveStaticInfo.h"
#include "CDataManager.h"   // stMonsterIndependentDrop_t（+0x3ac 容器元素类型）

// ---------------------------------------------------------------------------
// 成员 POD 类型（字段偏移均出自 ORIG 各自 clear/C1 objdump）
// ---------------------------------------------------------------------------

// CatchItemInfo：无独立 ctor/clear 符号（纯 POD）；元素大小 0xc 由
// vector<CatchItemInfo>::push_back W 0x08a3fd44 尾指针步长 lea 0xc(%eax) 实证；
// generateCatchMonsterItem T 0x08349f4b 以其向量遍历。
struct CatchItemInfo
{
    int m_field0;   // +0x00
    int m_field4;   // +0x04
    float m_field8; // +0x08（ORIG export 0x08a28ca8 单指令 flds disp ⇒ float）
};  // sizeof == 0xc

// STThrowAttack：clear W 0x08a3cf40 / C1 W 0x08a3cfda（ORIG 无 D1/D2 → 不加用户析构）。
struct STThrowAttack
{
    void clear()
    {
        m_field0c = 0;   // ORIG 先写 +0x0c = 0
        m_field10 = -1;
        m_field14 = -1;
        m_field18.clear();
        m_field24.clear();
        m_field30.clear();
        m_field3c = -1;
        m_field44 = -1;
        m_field48 = -1;
        m_field50 = -1;
        m_field54 = 0;
        m_field58.clear();
        m_field4c = 0;   // ORIG 最后写 +0x4c
    }

    STThrowAttack() { clear(); }   // ORIG C1：成员默认构造后调 clear

    std::vector<std::string> m_field00;   // +0x00（C1 首成员 vector<string>）
    int m_field0c;                        // +0x0c（clear =0）
    int m_field10;                        // +0x10（clear =-1）
    int m_field14;                        // +0x14（clear =-1）
    std::vector<int> m_field18;           // +0x18
    std::vector<int> m_field24;           // +0x24
    std::vector<int> m_field30;           // +0x30
    int m_field3c;                        // +0x3c（clear =-1）
    std::string m_field40;                // +0x40
    int m_field44;                        // +0x44（clear =-1）
    int m_field48;                        // +0x48（clear =-1）
    int m_field4c;                        // +0x4c（clear =0）
    int m_field50;                        // +0x50（clear =-1）
    int m_field54;                        // +0x54（clear =0）
    std::vector<int> m_field58;           // +0x58
};  // sizeof == 0x64

// STCounterAttack：clear W 0x08a3d0de / C1 W 0x08a3d118 / D1 W 0x08a4c4e2（全 W → 头内联）。
struct STCounterAttack
{
    void clear()
    {
        m_field00 = -1;
        m_field04 = -1;
        m_field08 = -1;
        m_field0c = 0;
        m_field10 = 0;
        m_field0d = 0;   // ORIG 最后写 +0x0d
    }

    STCounterAttack() { clear(); }
    ~STCounterAttack() = default;   // ORIG 值拷贝无 scope-exit D1 调用⇒trivial dtor（D1 W 由显式销毁点发射）

    int m_field00;      // +0x00（clear =-1）
    int m_field04;      // +0x04（clear =-1）
    int m_field08;      // +0x08（clear =-1）
    unsigned char m_field0c;  // +0x0c（clear byte =0）
    unsigned char m_field0d;  // +0x0d（clear byte =0）
    int m_field10;      // +0x10（clear =0）
};  // sizeof == 0x14

// STJumpAttack：clear W 0x08a3d12c / C1 W 0x08a3d1a0 / D1 W 0x08a4c6f2。
struct STJumpAttack
{
    int m_field00;      // +0x00（=-1）
    int m_field04;      // +0x04（=-1）
    int m_field08;      // +0x08（=-1）
    int m_field0c;      // +0x0c（=-1）
    float m_field10;    // +0x10（clear =1.0f，mov 0x3f800000）
    int m_field14;      // +0x14（=-1）
    int m_field18;      // +0x18（=0）
    int m_field1c;      // +0x1c（=1）
    int m_field20;      // +0x20
    int m_field24;      // +0x24
    int m_field28;      // +0x28

    void clear()
    {
        m_field00 = -1;
        m_field04 = -1;
        m_field08 = -1;
        m_field0c = -1;
        m_field10 = 1.0f;
        m_field14 = -1;
        m_field1c = 1;
        m_field20 = 0;
        m_field24 = 0;
        m_field28 = 0;
        m_field18 = 0;   // ORIG 最后写 +0x18
    }

    STJumpAttack() { clear(); }
    ~STJumpAttack() = default;   // ORIG 值拷贝无 scope-exit D1 调用⇒trivial dtor（D1 W 由显式销毁点发射）
};  // sizeof == 0x2c

// STSummonInfo：clear W 0x08a3d248 / C1 W 0x08a3d274 / D1 W 0x08a4c900。
struct STSummonInfo
{
    int m_field00;   // +0x00
    int m_field04;   // +0x04
    int m_field08;   // +0x08
    int m_field0c;   // +0x0c

    void clear()
    {
        m_field00 = 0;
        m_field04 = 0;
        m_field08 = 0;
        m_field0c = 0;
    }

    STSummonInfo() { clear(); }
    ~STSummonInfo() = default;   // ORIG 值拷贝无 scope-exit D1 调用⇒trivial dtor（D1 W 由显式销毁点发射）
};  // sizeof == 0x10

// STSummon：clear W 0x08a3d288 / C1 W 0x08a3d2f8 / copy C1 W 0x083d4336
// （copy C1 为编译器合成浅拷贝：标量逐字段 + vector 拷贝，P74 隐式生成即可）。
struct STSummon
{
    void clear()   // ORIG W 0x08a3d288（逐字段转写）
    {
        m_field00 = 0;
        m_field04 = -1;
        m_field08 = 0;
        m_field0c = 0;
        m_infos.clear();
        m_field1c = 0;
        m_field20 = 0;
        m_field24 = 0;
        m_field28 = 0;
        m_field2c = 0;
    }

    STSummon() { clear(); }   // ORIG C1 W 0x08a3d2f8：vector 构造后调 clear

    int m_field00;                            // +0x00（=0）
    int m_field04;                            // +0x04（=-1）
    int m_field08;                            // +0x08
    int m_field0c;                            // +0x0c
    std::vector<STSummonInfo> m_infos;        // +0x10
    int m_field1c;                            // +0x1c
    int m_field20;                            // +0x20
    int m_field24;                            // +0x24
    int m_field28;                            // +0x28
    int m_field2c;                            // +0x2c
};  // sizeof == 0x30

// STDash：clear W 0x08a3d340 / C1 W 0x08a3d3a8 / D1 W 0x08a4ccfe（全 W → 头内联）。
struct STDash
{
    void clear()
    {
        m_field00 = 0;
        m_field04 = -1;
        m_field08 = 0;
        m_field0c = 0;
        m_field10 = 0;
        m_field14 = 0;
        m_field18 = 0;
        m_field1c = 0;
        m_field20 = 0;
        m_field24 = 0;
    }

    STDash() { clear(); }
    ~STDash() = default;   // ORIG 值拷贝无 scope-exit D1 调用⇒trivial dtor（D1 W 由显式销毁点发射）

    int m_field00;   // +0x00（=0）
    int m_field04;   // +0x04（=-1）
    int m_field08;   // +0x08
    int m_field0c;   // +0x0c
    int m_field10;   // +0x10
    int m_field14;   // +0x14
    int m_field18;   // +0x18
    int m_field1c;   // +0x1c
    int m_field20;   // +0x20
    int m_field24;   // +0x24
};  // sizeof == 0x28

// STHold：clear W 0x08a3d3bc / C1 W 0x08a3d41a / D1 W 0x08a4cf02（全 W → 头内联）。
struct STHold
{
    void clear()
    {
        m_field04 = 0;
        m_field08 = 0;
        m_field00 = 0;   // ORIG 第三个写 +0x00
        m_field0c = 0;
        m_field10 = 0;
        m_field14 = 0;
        m_field18 = 0;
        m_field1c = 0;
        m_field20 = 0;
    }

    STHold() { clear(); }
    ~STHold() = default;   // ORIG 值拷贝无 scope-exit D1 调用⇒trivial dtor（D1 W 由显式销毁点发射）

    int m_field00;   // +0x00
    int m_field04;   // +0x04
    int m_field08;   // +0x08
    int m_field0c;   // +0x0c
    int m_field10;   // +0x10
    int m_field14;   // +0x14
    int m_field18;   // +0x18
    int m_field1c;   // +0x1c
    int m_field20;   // +0x20
};  // sizeof == 0x24

// STWarp：clear W 0x08a3d42e / C1 W 0x08a3d48c / D1 W 0x08a4d106（全 W → 头内联）。
struct STWarp
{
    void clear()
    {
        m_field00 = -1;
        m_field04 = -1;
        m_field08 = -1;
        m_field0c = -1;
        m_field10 = -1;
        m_field14 = -1;
        m_field18 = -1;
        m_field1c = -1;
        m_field20 = 0x2c;
    }

    STWarp() { clear(); }
    // ORIG D1 W 0x08a4d106：隐式平凡析构（export 循环内 local 仅调 C1、无 D1
    // 调用实证 08a268xx；显式空析构会迫使 GCC 在非平凡销毁点外联 call D1）。

    int m_field00;   // +0x00（=-1）
    int m_field04;   // +0x04（=-1）
    int m_field08;   // +0x08（=-1）
    int m_field0c;   // +0x0c（=-1）
    int m_field10;   // +0x10（=-1）
    int m_field14;   // +0x14（=-1）
    int m_field18;   // +0x18（=-1）
    int m_field1c;   // +0x1c（=-1）
    int m_field20;   // +0x20（clear =0x2c）
};  // sizeof == 0x24

// STDifficulty：clear W 0x08a3d4a0 / C1 W 0x08a3d4b8 / D1 W 0x08a4d8ce。
struct STDifficulty
{
    int m_field00;   // +0x00
    int m_field04;   // +0x04

    void clear()
    {
        m_field00 = 0;
        m_field04 = 0;
    }

    STDifficulty() { clear(); }
    ~STDifficulty() = default;   // ORIG 值拷贝无 scope-exit D1 调用⇒trivial dtor（D1 W 由显式销毁点发射）
};  // sizeof == 8

// STEventSuper：clear W 0x08a3d4cc / C1 W 0x08a3d52e / D1 W 0x08a4d310。
struct STEventSuper
{
    int m_field00;            // +0x00（=-1）
    int m_field04;            // +0x04（=-1）
    int m_field08;            // +0x08（=-1）
    int m_field0c;            // +0x0c（=0）
    bool m_field10;           // +0x10（ORIG export 0x08a26dbf 双 movzbl 归一 = bool→int）
    int m_field14;            // +0x14（=-1）
    int m_field18;            // +0x18（=-1）
    int m_field1c;            // +0x1c（=-1）
    int m_field20;            // +0x20（=-1）
    bool m_field24;           // +0x24（ORIG export 0x08a26f8c movzbl+test 直判 ⇒ bool）

    void clear()
    {
        m_field00 = -1;
        m_field04 = -1;
        m_field08 = -1;
        m_field0c = 0;
        m_field10 = 0;
        m_field14 = -1;
        m_field18 = -1;
        m_field1c = -1;
        m_field20 = -1;
        m_field24 = 0;
    }

    STEventSuper() { clear(); }
    ~STEventSuper() = default;   // ORIG 值拷贝无 scope-exit D1 调用⇒trivial dtor（D1 W 由显式销毁点发射）
};  // sizeof == 0x28

// STAtkSuper：clear W 0x08a3d542 / C1 W 0x08a3d582 / D1 W 0x08a4d502。
struct STAtkSuper
{
    int m_field00;   // +0x00（=-1）
    int m_field04;   // +0x04（=-1）
    int m_field08;   // +0x08（=-1）
    int m_field0c;   // +0x0c（=-1）
    int m_field10;   // +0x10（=-1）
    int m_field14;   // +0x14（=-1）

    void clear()
    {
        m_field00 = -1;
        m_field04 = -1;
        m_field08 = -1;
        m_field0c = -1;
        m_field10 = -1;
        m_field14 = -1;
    }

    STAtkSuper() { clear(); }
    ~STAtkSuper() = default;   // ORIG 值拷贝无 scope-exit D1 调用⇒trivial dtor（D1 W 由显式销毁点发射）
};  // sizeof == 0x18

// STCoolTime：clear W 0x08a3d596 / C1 W 0x08a3d5b6（ORIG 无 D1/D2 → 不加用户析构）。
struct STCoolTime
{
    void clear()
    {
        m_field00 = 0;       // ORIG 先写 +0x00 = 0
        m_field04.clear();   // +0x04 vector<int>::clear
    }

    STCoolTime() { clear(); }   // ORIG C1：vector<int> 构造后调 clear

    int m_field00;             // +0x00
    std::vector<int> m_field04;  // +0x04
};  // sizeof == 0x10

// MonsterBaseParameter：权威实现同构（InitScriptProducers_4.cpp TU 内已有
// 同名定义且不含本头，互不影响）；此处按 ORIG clear W 0x08a3ce4a 重述：
// sizeof 0x60（rep movsl $0x18 实证于 ImportMonsterBaseTable 0x08a3c9af）。
struct MonsterBaseParameter
{
    enum { FIELD_COUNT = 24 };
    // [b28c] ORIG 两处循环列组用 base=param+4、idx=i+0xc/i+0x10 寻址
    // （clear `movl $0,0x4(%eax,%edx,4)` @0x08a3ce4a；ImportMonsterBaseTable
    //  `mov %eax,-0xe8(%ebp,%edx,4)` @0x08a3c8a8/0x08a3c8d9）⇒ 原码数组成员自
    //  +0x04 起（首 int 独立）。匿名 union 双视角复现该寻址；sizeof 0x60、全部
    //  偏移、mangled 零变化（POD 拷贝仍 rep movsl 0x18 dwords）。
    union {
        int m_value[FIELD_COUNT];   // +0x00..+0x5c，按解析顺序填充（既有消费点）
        // [b29-SweepSTMonsterScript2] 二维列组视角（P311/S85）：m_grid[3][i]=value[13+i]
        // （this+4+(12+i)*4）、m_grid[4][i]=value[17+i]（this+4+(16+i)*4）⇒ 复现 ORIG
        // `mov i,%edx; add $0xc/$0x10,%edx; movl $0,0x4(%eax,%edx,4)` 两步寻址。
        // sizeof 仍 0x60（4 + 5*4*4 + 3*4 = 96），全部偏移/mangled 零变化。
        struct { int m_value0; int m_grid[5][4]; int m_value21, m_value22, m_value23; };
    };

    // ORIG clear W 0x08a3ce4a：v[0..12] 直写 + 双写循环 v[13+i]/v[17+i]
    // + v[21..23] 直写（分段与解析列组一致，实证 08a3ce4a）
    // [b28d-solo-STMonsterScript N·P30 旧判词，b29-SweepSTMonsterScript2 已推翻]
    // 权威 ORIG 0x08a3ce4a（nm -S 0xe1；旧注释 0x0834852e 系撞名报告 CCharacter::clear
    // 地址，已更正）：两处 `mov i,%edx; mov this,%eax; add $0xc/$0x10,%edx;
    // movl $0,0x4(%eax,%edx,4)`（base=param+4、idx=i+0xc/i+0x10）。
    // 旧判词称「二维具名视图必撑大 sizeof（[2][12] 96→100）⇒ N 封顶」——**不成立**：
    // 用 `m_grid[5][4]`（4 + 5*4*4 + 3*4 = 96 = 0x60，与 m_value[24] 等宽）即 P311/S85
    // 两步寻址形，4.4 fresh 编译产出与 ORIG 逐条相同的 `add $0xc/$0x10,%edx`；
    // clear 与 ImportMonsterBaseTable 两件同时 DIFF→AE（TU 四数 ae 1909→1911、diff 11→9）。
    void clear()
    {
        m_value[0] = 0;   m_value[1] = 0;   m_value[2] = 0;   m_value[3] = 0;
        m_value[4] = 0;   m_value[5] = 0;   m_value[6] = 0;   m_value[7] = 0;
        m_value[8] = 0;   m_value[9] = 0;   m_value[10] = 0;  m_value[11] = 0;
        m_value[12] = 0;
        for (int i = 0; i <= 3; ++i)
        {
            m_grid[3][i] = 0;   // ORIG disp=+0x04、idx=i+0xc（value[13+i]）
            m_grid[4][i] = 0;   // ORIG disp=+0x04、idx=i+0x10（value[17+i]）
        }
        m_value[21] = 0;
        m_value[22] = 0;
        m_value[23] = 0;
    }
    // ORIG C1 W 0x08a3cf2c：仅调用 clear()
    MonsterBaseParameter() { clear(); }
};  // sizeof == 0x60

// STGuard：ORIG 无独立 ctor/clear 符号；元素大小 36 字节（0x24）双证据实证：
// ① vector<STGuard>::size W 0x083aa36e 尾部 sar2+imul 0x38e38e39（÷9 ⇒ ÷36）；
// ② new_allocator<STGuard>::allocate W 0x083d5358 的 `shl3; add; shl2`（36n）。
// 内部字段无解析证据，按 9×int 占位。
struct STGuard
{
    // [P342 triviality 2026-09-11] 用户声明默认构造（内联空体）⇒ 类型非 POD/非平凡
    // ⇒ std::uninitialized_copy / std::copy 走 ILb0 分支，与 ORIG 形态对齐：
    //   ORIG _ZNSt20__uninitialized_copyILb0EE...IP7STGuard.. 0x083d5439（逐字段拷贝）、
    //        _ZNSt11__copy_moveILb0ELb0E...__copy_mIP7STGuard.. 0x083e325e；
    //   析构仍平凡（ORIG _ZNSt12_Destroy_auxILb1EE9__destroyIP7STGuardEEvT_S4_ 0x083cb8a9 = 空）。
    // 布局不变（sizeof 仍 0x24）；不引入任何符号（该 ctor 未被 odr-use）。
    STGuard() {}
    int m_field[9];
};  // sizeof == 0x24
static_assert(sizeof(STGuard) == 0x24, "STGuard layout locked (ORIG 0x24: divide-by-36 in vector::size 0x083aa36e)");

// STAttackMethod：clear W 0x08a3d5fe / C1 W 0x08a3d690 / D1 W 0x08a4bed8（全 W → 头内联）。
// clear（ORIG 0x08a3d5fe 逐指令）：+0x00=0；+0x18=0 → +0x14=+0x18；+0x20=0 → +0x1c=+0x20；
// +0x28=0 → +0x24=+0x28；+0x30=0 → +0x2c=+0x30；+0x04/+0x08/+0x0c/+0x10=0。
struct STAttackMethod
{
    STAttackMethod() { clear(); }   // ORIG C1 仅调 clear()
    ~STAttackMethod() = default;   // ORIG 值拷贝无 scope-exit D1 调用⇒trivial dtor（D1 W 由显式销毁点发射）
    void clear()
    {
        m_field00 = 0;
        m_field18 = 0;
        m_field14 = m_field18;
        m_field20 = 0;
        m_field1c = m_field20;
        m_field28 = 0;
        m_field24 = m_field28;
        m_field30 = 0;
        m_field2c = m_field30;
        m_field04 = 0;
        m_field08 = 0;
        m_field0c = 0;
        m_field10 = 0;
    }

    int m_field00;   // +0x00（=0）
    float m_field04; // +0x04（ORIG export 0x08a28a83 flds 0x4(%eax) ⇒ float）
    float m_field08; // +0x08（ORIG export 0x08a23f85 单指令 flds disp ⇒ float）
    float m_field0c; // +0x0c（同上 0x08a23f60）
    float m_field10; // +0x10（同上 0x08a23f3d）
    int m_field14;   // +0x14（clear：=+0x18 值）
    int m_field18;   // +0x18
    int m_field1c;   // +0x1c（=+0x20 值）
    int m_field20;   // +0x20
    int m_field24;   // +0x24（=+0x28 值）
    int m_field28;   // +0x28
    int m_field2c;   // +0x2c（=+0x30 值）
    int m_field30;   // +0x30
};  // sizeof == 0x34

// STRidableSkillExplain：C1 W 0x08a3d6a4（4×string 构造后 +0x10=0）。
struct STRidableSkillExplain
{
    STRidableSkillExplain() { m_field10 = 0; }   // ORIG C1：4×string 构造后置 0

    std::string m_field00;   // +0x00
    std::string m_field04;   // +0x04
    std::string m_field08;   // +0x08
    std::string m_field0c;   // +0x0c
    int m_field10;           // +0x10（=0）
};  // sizeof == 0x14

// STRidableSkill：C1 W 0x08a3d73e（string@4、8×vector<int>@0x28、五组 8×int
// 初值：+0x08=-1 / +0x88=6 / +0xa8=-1 / +0xc8=-1 / +0xe8=-1）。
struct STRidableSkill
{
    STRidableSkill()   // ORIG C1：成员构造后 m_field00=0、m_name.clear()、数组循环赋初值
    {
        m_field00 = 0;
        m_name.clear();
        for (int i = 0; i < 8; ++i)
        {
            m_field08[i] = -1;
            m_field88[i] = 6;
            m_fielda8[i] = -1;
            m_fieldc8[i] = -1;
            m_fielde8[i] = -1;
        }
    }

    int m_field00;                       // +0x00（=0）
    std::string m_name;                  // +0x04（C1 后 Ss::clear）
    int m_field08[8];                    // +0x08..+0x24（C1 全 =-1）
    std::vector<int> m_levels[8];        // +0x28..+0x87（8×vector<int>）
    int m_field88[8];                    // +0x88..+0xa4（C1 全 =6）
    int m_fielda8[8];                    // +0xa8..+0xc4（=-1）
    int m_fieldc8[8];                    // +0xc8..+0xe4（=-1）
    int m_fielde8[8];                    // +0xe8..+0x104（=-1）
};  // sizeof == 0x108

// STInitialTarget：C1 W 0x08a3d892（string@4 构造后、-1@0、-1@8、true@0xc）。
struct STInitialTarget
{
    STInitialTarget()   // ORIG C1：string 构造后置标量初值
    {
        m_field00 = -1;
        m_field08 = -1;
        m_field0c = true;
    }

    int m_field00;           // +0x00（=-1）
    std::string m_field04;   // +0x04
    int m_field08;           // +0x08（=-1）
    bool m_field0c;          // +0x0c（=1）
};  // sizeof == 0x10

// STMonsterEquipment：clear W 0x08368d7e / C1 W 0x08368d92（构造后调 clear，
// 忠实 ORIG 冗余复位）/ D1 W 0x08368dd4 / op= W 0x08377976。
struct STMonsterEquipment
{
    std::vector<std::string> m_items;   // +0x00

    STMonsterEquipment();
    void clear();
};  // sizeof == 0xc

// STMosterAttackReady：clear W 0x08a3d1b4 / C1 W 0x08a3d1e0（逐字段转写）。
struct STMosterAttackReady
{
    void clear()   // ORIG W 0x08a3d1b4：+0x0c=-1 → string vector clear → int vector clear
    {
        m_field0c = -1;
        m_field10.clear();
        m_field00.clear();
    }

    STMosterAttackReady() { clear(); }   // ORIG C1 W 0x08a3d1e0：两 vector 构造后调 clear

    std::vector<int> m_field00;          // +0x00
    int m_field0c;                       // +0x0c（clear =-1）
    std::vector<std::string> m_field10;  // +0x10
};  // sizeof == 0x1c

// stEnamyDropAvatarElement_t：push_back W 0x088ccbd6 步长 lea 0xc(%eax)；
// ORIG 第三次 ScanInt 仅以 mov %al 写入 +0x08，尾部 3 字节不消费。
struct stEnamyDropAvatarElement_t
{
    int m_field0;
    int m_field4;
    unsigned char m_field8;
    char m_pad9[3];
};  // sizeof == 0xc

// stEnamyDropAvatarInfo_t：clear W 0x088cc10c / C1 W 0x08368de8（与
// CharacterAIA2.cpp TU 内同构定义一致，两 TU 不互相包含）。
struct stEnamyDropAvatarInfo_t
{
    stEnamyDropAvatarInfo_t() {}                    // ORIG C1 W 0x08368de8：仅 vector 默认构造
    void clear();                                   // ORIG W 0x088cc10c
    void operator=(const stEnamyDropAvatarInfo_t& other);   // ORIG W 0x08366ea8（尾无 eax 恢复→void）

    int m_field00;                              // +0x00
    int m_field04;                              // +0x04
    // [b28d-solo-STMonsterScript] +0x08..+0x2b：ORIG clear/op= 第二循环按两步索引
    //   `mov i,%r; add/lea $0x4,%r`（P30 反例解）访问 +0x18+4i；一维 `[i+4]` 在 4.4.6
    //   恒折 `lea 0x4(%r)`。二维视角 `[1][i]` 逐条复现 ORIG（探针 /tmp/solo_probe1.cpp）；
    //   i=4 越界至 +0x28 第 9 个 float —— ORIG 原码即如此写（P171 照抄越界），
    //   内存仍在 union 36B 内。union 尺寸 36B，sizeof/偏移/mangled 零变化。
    union {
        float m_field08[9];                     // 一维视角（float 实证）
        float m_field08v[2][4];                 // 二维视角（ORIG 第二循环寻址形）
    };
    int m_field2c;                              // +0x2c
    std::vector<stEnamyDropAvatarElement_t> m_elements;  // +0x30
};  // sizeof == 0x3c

// （copy-ctor 报告 STAIInfoScript/STAIInfoScript.md 权威转写）。ORIG 为 ODR
// 违规双定义（本域 0x208 / CharacterAIA 域 0x224，弱符号合并），两域各自正确：
// 本域 +0x204 flag 由外层 STMonsterScript::Clear 置位，CharacterAIA 域尾部
// POD 由外层 STAICharacterCommonScript::clear 清零。本头方法内联（W 弱，
// 与 CharacterAIA2.cpp 强符号链接合并无冲突）。
class STAIInfoScript
{
public:
    STAIInfoScript() { clear(); }           // ORIG C1 W 0x08366f48：vptr+成员构造+clear
    // ORIG C1ERKS T 0x08a38bc6：vptr+成员默认构造+clear()+operator=()（非编译器合成版）。
    STAIInfoScript(const STAIInfoScript& other) { clear(); *this = other; }
    virtual ~STAIInfoScript() { destroy(); }  // ORIG D1 W 0x083671a4：vptr+destroy+成员逆序析构
    // ORIG T 0x08a38e34（objdump 逐条转写）：destroy → 标量/string → patterns[5] 深拷贝
    // （new string）→ targetingBonus[22] 与 targetingBonusStr[22] → keepDistance →
    // keepRange0..4 → vision/backVision/intelligence/1f0/1f4 → noChangeTarget；
    // +0x204 起不触碰（ORIG 固有）。
    STAIInfoScript& operator=(const STAIInfoScript& other)
    {
        destroy();
        m_thinkTerm = other.m_thinkTerm;
        m_destinationChangeTerm = other.m_destinationChangeTerm;
        m_dir = other.m_dir;
        for (int p = 0; p < 5; ++p)
        {
            for (std::size_t i = 0; i < other.m_patterns[p].size(); ++i)
            {
                std::string* elem = other.m_patterns[p][i];
                std::string* created = new std::string(*elem);
                m_patterns[p].push_back(created);
            }
        }
        for (int i = 0; i <= 21; ++i)
        {
            m_targetingBonus[i] = other.m_targetingBonus[i];
            m_targetingBonusStr[i] = other.m_targetingBonusStr[i];
        }
        m_keepDistance = other.m_keepDistance;
        m_keepRange0 = other.m_keepRange0;
        m_keepRange1 = other.m_keepRange1;
        m_keepRange2 = other.m_keepRange2;
        m_keepRange3 = other.m_keepRange3;
        m_keepRange4 = other.m_keepRange4;
        m_vision = other.m_vision;
        m_backVision = other.m_backVision;
        m_intelligence = other.m_intelligence;
        m_field1f0 = other.m_field1f0;
        m_field1f4 = other.m_field1f4;
        m_noChangeTarget = other.m_noChangeTarget;
        return *this;
    }
    void clear()                            // ORIG W 0x083673d4
    {
        m_thinkTerm = 0x3e8;
        m_destinationChangeTerm = 0x3e8;
        m_keepRange0.clear();     // ORIG：5 次展开 call（+0x1b0/1bc/1c8/1d4/1e0）
        m_keepRange1.clear();
        m_keepRange2.clear();
        m_keepRange3.clear();
        m_keepRange4.clear();
        m_dir.clear();
        for (int i = 0; i < 5; ++i)
            m_patterns[i].clear();
        for (int i = 0; i <= 21; ++i)
        {
            if (i == 0)
                m_targetingBonus[i] = 0x32;   // ORIG：if/else 双 store（非三目选值）
            else
                m_targetingBonus[i] = 0;
            m_targetingBonusStr[i].clear();
        }
        m_keepDistance = 0x64;      // +0x1ac
        m_intelligence = 0x64;      // +0x1ec
        m_field1f0 = 0x64;          // +0x1f0
        m_field1f4 = 0x1388;        // +0x1f4
        m_vision = 0x5dc;           // +0x1f8
        m_backVision = 0x5dc;       // +0x1fc
        m_noChangeTarget = 1;       // +0x200
    }
    void destroy()                          // ORIG W 0x0836753a
    {
        // ORIG 逐块转写：外层 p<=4（setle）；内层每轮重取 size()（setg 有符号）；
        // body 内 ix() 三次独立装载（P48 无 CSE）：判空死块 → 取值 → 置 0。
        for (int p = 0; p <= 4; ++p)
        {
            for (int i = 0; i < (int)m_patterns[p].size(); ++i)
            {
                if (m_patterns[p][i] != 0)
                {
                    delete m_patterns[p][i];
                }
                m_patterns[p][i] = 0;
            }
            m_patterns[p].clear();
        }
    }

    int m_thinkTerm;                    // +0x04
    int m_destinationChangeTerm;        // +0x08
    std::string m_dir;                  // +0x0c
    std::vector<std::string*> m_patterns[5];       // +0x10 easy/medium/hard/ultimate/hero
    int m_targetingBonus[22];           // +0x4c
    std::vector<int> m_targetingBonusStr[22];      // +0xa4
    int m_keepDistance;                 // +0x1ac
    // 5 个独立成员（ORIG C1/D1/op= 对 +0x1b0..+0x1e0 逐个发射 call，非数组循环）。
    std::vector<int> m_keepRange0;      // +0x1b0
    std::vector<int> m_keepRange1;      // +0x1bc
    std::vector<int> m_keepRange2;      // +0x1c8
    std::vector<int> m_keepRange3;      // +0x1d4
    std::vector<int> m_keepRange4;      // +0x1e0
    int m_intelligence;                 // +0x1ec
    int m_field1f0;                     // +0x1f0
    int m_field1f4;                     // +0x1f4
    int m_vision;                       // +0x1f8
    int m_backVision;                   // +0x1fc
    bool m_noChangeTarget;              // +0x200
};  // sizeof == 0x204（+0x201..0x203 为自然对齐填充；+0x204 起不属本类——
    //    ORIG STMonsterScript::operator= W 0x08377994 把 0x33c 作为**外层直接成员**
    //    单独拷贝 `movzbl 0x33c(%eax),%edx; mov %dl,0x33c(%eax)`，而 STAIInfoScript
    //    的 op= T 0x08a38e34 不触碰 +0x204 ⇒ 该字节属 STMonsterScript::m_flag33c）

// STMonsterParameterCategory：vector 元素（+0x7c），大小 0xc；
// getValueInt/getValueFloat 为 ORIG W 0x88ba480 / W 0x88ba4a2 同名方法
// （实现见 MonsterAbilityLeafC3.cpp）。C1 W 0x08a3cd6c（+0x00=0xe、其余 0，
// 内联赋值无 clear）/ D1 W 0x08a4bcbc（空体，全 W → 头内联）；无 clear 符号。
struct STMonsterParameterCategory
{
    STMonsterParameterCategory()
    {
        m_field0 = 14;
        m_field4 = 0;
        m_field8 = 0;
    }
    ~STMonsterParameterCategory() = default;   // ORIG 值拷贝无 scope-exit D1 调用⇒trivial dtor（D1 W 由显式销毁点发射）

    int m_field0;   // +0x00（类别 id，upgradeAbilityByMonsterParameterCategory 分派）
    int m_field4;   // +0x04（0=int 加成，非 0=float 倍率）
    int m_field8;   // +0x08（数值）

    int getValueInt() const;      // ORIG W 0x88ba480：m_field8 / 100
    float getValueFloat() const;  // ORIG W 0x88ba4a2：(float)m_field8 / 100.0f
};

// ---------------------------------------------------------------------------
// STMonsterScript 本体
// ---------------------------------------------------------------------------
class STMonsterScript
{
public:
    STMonsterScript();
    ~STMonsterScript();
    // ORIG op= W 0x08377994 为编译器合成版（无自赋值检查、rep movsl、尾部
    // eax=this）。不显式声明，由编译器按成员序隐式合成。
    // [SweepSTMonster] 1200/1200 gap=0：删除 13 处显式 `char m_padNNN` 数组
    // （P26：显式 pad 会被隐式拷贝逐字节搬移，ORIG 跳过对齐空洞；改由编译器
    // 自然填充，sizeof/全部偏移不变）+ +0x33c 改归外层 `m_flag33c`（见 m_aiInfo
    // 注释）⇒ ndiff 100→20。残差 20 行全为 N90：ORIG(4.1) 对相邻标量段缓存
    // src/dst 基址后连发 store（+0x64/+0x68/+0x6c、+0x6fc/+0x700/+0x704），
    // 4.4 恒逐字段重载双基址（源码侧不可控，隐式函数）。
    void Clear();

    int m_index;                         // +0x000（Clear=0；[name] 首列/isNamedMonster）
    std::string m_name;                  // +0x004 [name]
    std::string m_altName;               // +0x008（export 作 name 回退串【推断】）
    int m_width;                         // +0x00c =40 [width]
    int m_height;                        // +0x010 =20 [width]
    int m_floatingHeight;                // +0x014 =0 [floating height]
    int m_layer;                         // +0x018 =1 [layer]
    std::vector<int> m_field01c;         // +0x01c
    std::vector<std::pair<int, int> > m_field028[4];   // +0x028..+0x57
    std::vector<std::pair<int, int> > m_field058;      // +0x058
    // [DNF-FIX-PVF 2026-10-07] +0x64..+0x6f 为 [catch item] 暂存的 CatchItemInfo
    // 聚合（旧为 3 个 int）：ImportMonsterScript ORIG 0x08a20437/0x08a20449 写
    // +0x64/+0x68、0x08a2045b `fstps 0x6c` 写 float，再 `lea 0x64(script)` 作
    // push_back 实参；ORIG op= 0x08377a71..0x08377a80 对此段缓存双基址连拷
    // （聚合拷贝形），故建模为 CatchItemInfo 成员。宽度/偏移/sizeof 不变。
    CatchItemInfo m_catchItem;           // +0x064..+0x06f（ctor/Clear 不触碰）
    std::vector<CatchItemInfo> m_catchItems;           // +0x070
    std::vector<STMonsterParameterCategory> m_parameterCategories;  // +0x07c
    int m_field088;                      // +0x088
    std::vector<std::pair<int, int> > m_field08c;      // +0x08c
    std::string m_field098;              // +0x098
    std::vector<STThrowAttack> m_throwAttacks;         // +0x09c
    std::vector<STCounterAttack> m_counterAttacks;     // +0x0a8
    std::vector<STJumpAttack> m_jumpAttacks;           // +0x0b4
    std::vector<STSummon> m_summons;                   // +0x0c0
    std::vector<STDash> m_dashes;                      // +0x0cc
    std::vector<STHold> m_holds;                       // +0x0d8
    std::vector<STWarp> m_warps;                       // +0x0e4
    std::vector<STDifficulty> m_difficulties;          // +0x0f0
    std::vector<STEventSuper> m_eventSupers;           // +0x0fc
    std::vector<STAtkSuper> m_atkSupers;               // +0x108
    std::vector<STCoolTime> m_coolTimes;               // +0x114
    std::vector<int> m_field120;                       // +0x120
    std::vector<MonsterBaseParameter> m_baseParameters;  // +0x12c
    STAIInfoScript m_aiInfo;                           // +0x138（0x204，止于 0x33c）
    bool m_flag33c;                      // +0x33c（Clear=1；Import [ai info] 首列 bool。
                                         //   ORIG op= W 0x08377994 在此单独拷贝本字节）
    std::deque<STInitialTarget> m_initialTargets;      // +0x340
    bool m_flag368;                      // +0x368（Clear=1）
    int m_field36c;                      // +0x36c
    int m_field370;                      // +0x370
    bool m_flag374;                      // +0x374（Clear=false）
    int m_field378;                      // +0x378
    int m_field37c;                      // +0x37c
    int m_field380;                      // +0x380
    std::vector<STGuard> m_guards;                     // +0x384
    STMonsterEquipment m_equipment;                    // +0x390
    std::vector<STMosterAttackReady> m_attackReadies;  // +0x39c
    bool m_flag3a8;                      // +0x3a8（Clear=false）
    std::vector<stMonsterIndependentDrop_t> m_independentDrops;  // +0x3ac
    bool m_flag3b8;                      // +0x3b8（Clear=1；setBasic → info.m_flag14）
    int m_abilityTable;                  // +0x3bc（getMonsterTableIndex：==1 → idx3）
    int m_field3c0;                      // +0x3c0
    float m_field3c4[10];                // +0x3c4..0x3eb（Clear 循环 i≤9 清零；float 实证：imm 经寄存器中转形态）
    int m_levelByDiff[2];                // +0x3ec..0x3f3 [level]
    int m_resistTable[4][2];             // +0x3f4..0x413 [fire/water/dark/light][difficulty]
                                         // （Clear 单循环 [j][i] 清零实证；export 标签
                                         //   fire/water/dark/light resistance 对应行 0..3）
    int m_linearAbilities[17][2];        // +0x414..0x49b（17 组低/高难度线性对）
    int m_hpRegenSpeed[2];               // +0x49c [HP regen speed]
    int m_mpRegenSpeed[2];               // +0x4a4 [MP regen speed]
    int m_moveSpeed[2];                  // +0x4ac [move speed]
    int m_attackSpeed[2];                // +0x4b4 [attack speed]
    int m_castSpeed[2];                  // +0x4bc [cast speed]
    int m_hitRecovery[2];                // +0x4c4 [hit recovery]
    int m_jumpPower[2];                  // +0x4cc [jump power]
    int m_jumpSpeed[2];                  // +0x4d4 [jump speed]（Clear[0]=100）
    int m_normalDashSpeed[2];            // +0x4dc [normal dash speed]
    int m_slantDashSpeed[2];             // +0x4e4 [slant dash speed]
    int m_weight[2];                     // +0x4ec [weight]
    int m_stuckBonusOnDamage[2][2];      // +0x4f4..0x503 [stuckbonus on damage]
    int m_sight;                         // +0x504 =200 [sight]
    int m_paramTable[4][2];              // +0x508..0x527（elemDefense 线性源）
    int m_field528[2];                   // +0x528..0x52f（Clear 双元素清零）
    std::string m_comeInAction;          // +0x530 [come in action]
    std::string m_comeInActionEx;        // +0x534 [come in action ex]
    int m_field538;                      // +0x538 =100
    int m_field53c;                      // +0x53c =200
    float m_field540;                    // +0x540 =-1.0f
    float m_field544;                    // +0x544 =-1.0f
    bool m_field548;                     // +0x548
    float m_field54c;                    // +0x54c =20.0f
    float m_field550;                    // +0x550 =-300.0f
    std::vector<int> m_field554;         // +0x554
    std::vector<int> m_field560;         // +0x560
    bool m_field56c;                     // +0x56c
    std::string m_field570;              // +0x570
    int m_targetingTimeTerm;             // +0x574 =1000 [targeting time term]
    int m_warlike;                       // +0x578 =50 [warlike]
    int m_attackDelay;                   // +0x57c =1000 [attack delay]
    bool m_targetingOnAttack;            // +0x580
    bool m_targetingOnDamage;            // +0x581
    int m_targetingLevel;                // +0x584 [targeting high/low level]（==2 高等级 / ==1 低等级）
    int m_targetingHp;                   // +0x588 [targeting high/low HP]（==2 高 HP / ==1 低 HP）
    int m_field58c;                      // +0x58c（未导出）
    bool m_targetingNearest;             // +0x590 [targeting nearest]
    bool m_targetingAttacker;            // +0x591 [targeting attacker]
    bool m_targetingFromDirection;       // +0x592 [targeting from direction]
    int m_targetingLowPhysAttack;        // +0x594 [targeting low physical attack]
    int m_targetingLowPhysDefense;       // +0x598 [targeting low physical defense]
    int m_targetingLowMagAttack;         // +0x59c [targeting low magical attack]
    int m_targetingLowMagDefense;        // +0x5a0 [targeting low magical defense]
    bool m_targetingDowned;              // +0x5a4 [targeting downed]
    bool m_superarmorOnAttack;           // +0x5a5
    int m_field5a8;                      // +0x5a8
    std::string m_faceImage;             // +0x5ac [face image]
    int m_field5b0;                      // +0x5b0（face image 第二列 %d）
    // 以下 6 个为独立成员而非数组：C1 逐一 SsC1 实证（合成 ctor 不生成循环）。
    // 语义命名证据（exportMonsterScript fprintf 格式串 ↔ 偏移）：
    //   +0x5b4=[waiting motion] +0x5b8=[move motion] +0x5bc..0x5c4=未导出
    //   +0x5c8=[sit motion]（原 m_sound0..5 命名错误，已按导出语义修正）
    std::string m_waitingMotion;         // +0x5b4 [waiting motion]
    std::string m_moveMotion;            // +0x5b8 [move motion]
    std::string m_field5bc;              // +0x5bc（未导出）
    std::string m_field5c0;              // +0x5c0（未导出）
    std::string m_field5c4;              // +0x5c4（未导出）
    std::string m_sitMotion;             // +0x5c8 [sit motion]
    std::string m_damageMotion[2];       // +0x5cc..0x5d3 [damage motion 1/2]
    std::string m_downMotion;            // +0x5d4 [down motion]
    std::string m_overturnMotion;        // +0x5d8 [overturn motion]
    std::string m_jumpMotion;            // +0x5dc [jump motion]
    std::string m_jumpAttackMotion;      // +0x5e0 [jumpattack motion]
    std::vector<std::string> m_attackMotions;   // +0x5e4 [attack motion] 列表
    std::vector<std::string> m_etcMotions;      // +0x5f0 [etc motion] 列表
    std::string m_waitingAction;         // +0x5fc [waiting action]
    std::string m_moveAction;            // +0x600 [move action]
    std::string m_movebackAction;        // +0x604 [moveback action]
    std::string m_dashAction;            // +0x608 [dash action]
    std::string m_dashattackAction;      // +0x60c [dashattack action]
    std::string m_sitAction;             // +0x610 [sit action]
    std::string m_damageAction[2];       // +0x614..0x61b [damage action 1/2]
    std::string m_downAction;            // +0x61c [down action]
    std::string m_overturnAction;        // +0x620 [overturn action]
    std::string m_jumpAction;            // +0x624 [jump action]
    std::string m_jumpAttackAction;      // +0x628 [jumpattack action]
    std::vector<std::string> m_attackActions;    // +0x62c [attack action] 列表
    std::vector<std::string> m_etcActions;       // +0x638 [etc action] 列表
    std::string m_lastAction;            // +0x644 [last action]
    std::string m_procAction;            // +0x648 [proc action]
    std::string m_mountingAction;        // +0x64c [mounting action]
    std::string m_mountedAction;         // +0x650 [mounted action]
    std::string m_unmountingAction;      // +0x654 [unmounting action]
    int m_field658;                      // +0x658（Clear=-1）
    int m_field65c;                      // +0x65c
    int m_field660;                      // +0x660
    int m_field664;                      // +0x664
    int m_field668;                      // +0x668
    float m_field66c;                    // +0x66c（Clear=0 经寄存器中转→float 实证）
    float m_field670;                    // +0x670
    bool m_field674;                     // +0x674
    int m_field678;                      // +0x678 =255
    int m_field67c;                      // +0x67c =255
    bool m_field680;                     // +0x680
    int m_field684;                      // +0x684 =4
    std::vector<std::vector<int> > m_patterns;    // +0x688 [pattern]
    std::string m_monsterTitle;          // +0x694 [monster title]
    std::string m_basicAction;           // +0x698 [basic action]
    std::string m_damageSound;           // +0x69c [damage sound]
    std::string m_dieSound;              // +0x6a0 [die sound]
    std::string m_appearSound;           // +0x6a4 [appear sound]
    std::string m_attackSound;           // +0x6a8 [attack sound]
    std::string m_ambientSound;          // +0x6ac [ambient sound]
    std::string m_etcSound;              // +0x6b0 [etc sound]
    std::string m_jumpAttackInfo;        // +0x6b4 [jumpattack info]
    std::vector<std::string> m_attackInfos;      // +0x6b8 [attack info] 列表
    std::vector<std::string> m_etcAttackInfos;   // +0x6c4 [etc attack info] 列表
    std::string m_explain;               // +0x6d0 [explain]
    std::vector<int> m_intData;          // +0x6d4 [int data]
    int m_keepDistance;                  // +0x6e0 =-1 [keep distance]
    bool m_isOnTheObject;                // +0x6e4 [is on the object]
    std::vector<STAttackMethod> m_attackMethods;       // +0x6e8
    int m_field6f4;                      // +0x6f4（Clear=0）
    int m_field6f8;                      // +0x6f8（Clear=0）
    int m_dieEffect[3];                  // +0x6fc..0x707 [die effect]（Clear 0/0/-1）
    std::string m_field708;              // +0x708（ImportMonsterScript 存目录路径）
    std::vector<std::string> m_field70c[111];          // +0x70c..0xc3f
    std::vector<int> m_championElements; // +0xc40（getRandomChampionElement 实证）
    int m_fieldc4c;                      // +0xc4c（Clear=0）
    bool m_flagc50;                      // +0xc50（Clear=false）
    float m_fieldc54;                    // +0xc54（Clear=0 经寄存器中转→float 实证）
    float m_fieldc58;                    // +0xc58
    int m_fieldc5c[5];                   // +0xc5c..0xc6f（Clear 循环 i≤4 清零）
    bool m_flagc70;                      // +0xc70（Clear=false）
    bool m_flagc71;                      // +0xc71（Clear=false）
    int m_fieldc74;                      // +0xc74（Clear=0）
    int m_fieldc78;                      // +0xc78（Clear=0）
    int m_fieldc7c;                      // +0xc7c（Clear=0）
    std::vector<int> m_fieldc80;         // +0xc80
    std::vector<int> m_fieldc8c;         // +0xc8c
    stEnamyDropAvatarInfo_t m_dropAvatarInfo;          // +0xc98（0x3c）
    bool m_flagcd4;                      // +0xcd4（Clear=false）
    int m_fieldcd8;                      // +0xcd8（Clear=4）
    std::vector<STRidableSkill> m_ridableSkills;       // +0xcdc
    std::vector<STRidableSkillExplain> m_ridableSkillExplains;  // +0xce8
    float m_difficultyScales[200][5];    // +0xcf4..0x1c93（Clear 全 =-1.0f；语义【推断】）
    int m_field1c94;                     // +0x1c94
    int m_field1c98;                     // +0x1c98
    int m_field1c9c;                     // +0x1c9c（Clear=3；processModuleTagSet 子模块号）
    int m_field1ca0;                     // +0x1ca0
    float m_field1ca4;                   // +0x1ca4（Clear=0 经寄存器中转→float 实证）
    bool m_field1ca8;                    // +0x1ca8
    int m_field1cac;                     // +0x1cac（Clear=-1）
    int m_field1cb0;                     // +0x1cb0（Clear=-1）
    std::vector<int> m_field1cb4;        // +0x1cb4
    int m_field1cc0;                     // +0x1cc0
};  // sizeof == 0x1cc4

typedef char assert_stmonster_script_size[(sizeof(STMonsterScript) == 0x1cc4) ? 1 : -1];

// ---- 自由函数（签名 = ORIG mangled）----
// [b28d-Main 代落·S224 P248档①] int→bool：ORIG 调用点 0x0834fa9f/0x08350cff 均
// `call; xor $1; test %al`（bool 直判）。返回域 0/1（44 处 return 0 + 尾 return 1）。
// 返回类型不入 mangled。
bool ImportMonsterScript(STMonsterScript* script, int type, const char* path,
                        bool flag);                                   // 0x08a12827
bool ImportMonsterBaseTable(STMonsterScript* script, const char* path);  // 0x08a3c736
bool exportMonsterScript(
    STMonsterScript* script, const char* path,
    std::map<std::string, std::vector<std::string> >& extra);          // 0x08a22520
bool isNamedMonster(int monsterIndex);                                 // 0x088ba18c
int getMonsterTableIndex(const STMonsterScript* script, int dungeonIdx,
                         int roleType, bool flagA, bool flagB);         // 0x088ba1c8
void setBasicAbilityByLevel(const STMonsterScript* script, ActiveStaticInfo* info,
                            int level, int dungeonIdx, int roleType,
                            bool a, bool b);                            // 0x088b9603
int getRandomChampionElement(const STMonsterScript* script, int seed);  // 0x088b9cce

// ---- 怪物能力升级自由函数（ORIG 实现见 MonsterAbilityLeafC3.cpp）----
void upgradeAbilityByMonsterParameterCategory(
    const std::vector<STMonsterParameterCategory>* categories,
    ActiveStaticInfo* info, bool isChampion);            // ORIG 0x088b7f36
void upgradeAbilityByDifficultyOfDungeon(int diff, ActiveStaticInfo* info,
                                         int level,
                                         const std::vector<std::vector<float> >& table);
                                            // ORIG 0x088b813a
void upgradeAbilityByBossFeature(ActiveStaticInfo* info);   // ORIG 0x088b7c24
void makeChampionFeatureList(std::vector<int>& out, int level, int seed,
                             std::vector<int>& tmp, int diff);  // ORIG 0x088ba0fd
void upgradeAbilityByChampionFeature(int elem, int roleType, std::vector<int>* features,
                                     ActiveStaticInfo* info);   // ORIG 0x088b7c29

#endif  /* GAME_STMONSTERSCRIPT_H_ */
