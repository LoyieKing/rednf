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
    int m_field8;   // +0x08
};  // sizeof == 0xc

// STThrowAttack：clear W 0x08a3cf40 / C1 W 0x08a3cfda。
struct STThrowAttack
{
    std::vector<std::string> m_field00;   // +0x00（C1 首成员 vector<string>）
    int m_field0c;                        // +0x0c（clear =-1）
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

// STCounterAttack：clear W 0x08a3d0de / C1 W 0x08a3d118。
struct STCounterAttack
{
    int m_field00;      // +0x00（clear =-1）
    int m_field04;      // +0x04（clear =-1）
    int m_field08;      // +0x08（clear =-1）
    unsigned char m_field0c;  // +0x0c（clear byte =0）
    unsigned char m_field0d;  // +0x0d（clear byte =0）
    int m_field10;      // +0x10（clear =0）
};  // sizeof == 0x14

// STJumpAttack：clear W 0x08a3d12c / C1 W 0x08a3d1a0。
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
};  // sizeof == 0x2c

// STSummonInfo：clear W 0x08a3d248 / C1 W 0x08a3d274。
struct STSummonInfo
{
    int m_field00;   // +0x00
    int m_field04;   // +0x04
    int m_field08;   // +0x08
    int m_field0c;   // +0x0c
};  // sizeof == 0x10

// STSummon：clear W 0x08a3d288 / C1 W 0x08a3d2f8 / copy C1 W 0x083d4336。
struct STSummon
{
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

// STDash：clear W 0x08a3d340 / C1 W 0x08a3d3a8。
struct STDash
{
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

// STHold：clear W 0x08a3d3bc / C1 W 0x08a3d41a。
struct STHold
{
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

// STWarp：clear W 0x08a3d42e / C1 W 0x08a3d48c。
struct STWarp
{
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

// STDifficulty：clear W 0x08a3d4a0 / C1 W 0x08a3d4b8。
struct STDifficulty
{
    int m_field00;   // +0x00
    int m_field04;   // +0x04
};  // sizeof == 8

// STEventSuper：clear W 0x08a3d4cc / C1 W 0x08a3d52e。
struct STEventSuper
{
    int m_field00;            // +0x00（=-1）
    int m_field04;            // +0x04（=-1）
    int m_field08;            // +0x08（=-1）
    int m_field0c;            // +0x0c（=0）
    unsigned char m_field10;  // +0x10（byte =0）
    int m_field14;            // +0x14（=-1）
    int m_field18;            // +0x18（=-1）
    int m_field1c;            // +0x1c（=-1）
    int m_field20;            // +0x20（=-1）
    unsigned char m_field24;  // +0x24（byte =0）
};  // sizeof == 0x28

// STAtkSuper：clear W 0x08a3d542 / C1 W 0x08a3d582。
struct STAtkSuper
{
    int m_field00;   // +0x00（=-1）
    int m_field04;   // +0x04（=-1）
    int m_field08;   // +0x08（=-1）
    int m_field0c;   // +0x0c（=-1）
    int m_field10;   // +0x10（=-1）
    int m_field14;   // +0x14（=-1）
};  // sizeof == 0x18

// STCoolTime：clear W 0x08a3d596 / C1 W 0x08a3d5b6。
struct STCoolTime
{
    int m_field00;             // +0x00
    std::vector<int> m_field04;  // +0x04
};  // sizeof == 0x10

// MonsterBaseParameter：权威实现同构（InitScriptProducers_4.cpp TU 内已有
// 同名定义且不含本头，互不影响）；此处按 ORIG clear W 0x08a3ce4a 重述：
// sizeof 0x60（rep movsl $0x18 实证于 ImportMonsterBaseTable 0x08a3c9af）。
struct MonsterBaseParameter
{
    enum { FIELD_COUNT = 24 };
    int m_value[FIELD_COUNT];   // +0x00..+0x5c，按解析顺序填充

    // ORIG clear W 0x08a3ce4a：v[0..12] 直写 + 双写循环 v[13+i]/v[17+i]
    // + v[21..23] 直写（分段与解析列组一致，实证 08a3ce4a）
    void clear()
    {
        m_value[0] = 0;   m_value[1] = 0;   m_value[2] = 0;   m_value[3] = 0;
        m_value[4] = 0;   m_value[5] = 0;   m_value[6] = 0;   m_value[7] = 0;
        m_value[8] = 0;   m_value[9] = 0;   m_value[10] = 0;  m_value[11] = 0;
        m_value[12] = 0;
        for (int i = 0; i <= 3; ++i)
        {
            m_value[13 + i] = 0;
            m_value[17 + i] = 0;
        }
        m_value[21] = 0;
        m_value[22] = 0;
        m_value[23] = 0;
    }
    // ORIG C1 W 0x08a3cf2c：仅调用 clear()
    MonsterBaseParameter() { clear(); }
};  // sizeof == 0x60

// STGuard：ORIG 无独立 ctor/clear 符号；元素大小 28 字节由
// vector<STGuard>::size W 0x083aa36e 尾部 sar2+imul 0x38e38e39（÷28）实证；
// 内部字段无解析证据，按 7×int 占位。
struct STGuard
{
    int m_field[7];
};  // sizeof == 0x1c

// STAttackMethod：clear W 0x08a3d5fe / C1 W 0x08a3d690。
struct STAttackMethod
{
    int m_field00;   // +0x00（=0）
    int m_field04;   // +0x04
    int m_field08;   // +0x08
    int m_field0c;   // +0x0c
    int m_field10;   // +0x10
    int m_field14;   // +0x14（clear：=+0x18 值）
    int m_field18;   // +0x18
    int m_field1c;   // +0x1c（=+0x20 值）
    int m_field20;   // +0x20
    int m_field24;   // +0x24（=+0x28 值）
    int m_field28;   // +0x28
    int m_field2c;   // +0x2c（=+0x30 值）
    int m_field30;   // +0x30
};  // sizeof == 0x34

// STRidableSkillExplain：C1 W 0x08a3d6a4（4×string + int@0x10）。
struct STRidableSkillExplain
{
    std::string m_field00;   // +0x00
    std::string m_field04;   // +0x04
    std::string m_field08;   // +0x08
    std::string m_field0c;   // +0x0c
    int m_field10;           // +0x10（=0）
};  // sizeof == 0x14

// STRidableSkill：C1 W 0x08a3d73e（string@4、8×vector<int>@0x28、四组 8×int）。
struct STRidableSkill
{
    int m_field00;                       // +0x00（=0）
    std::string m_name;                  // +0x04（C1 后 Ss::clear）
    int m_field08[8];                    // +0x08..+0x24（C1 全 =-1）
    std::vector<int> m_levels[8];        // +0x28..+0x87（8×vector<int>）
    int m_field88[8];                    // +0x88..+0xa4（C1 全 =6）
    int m_fielda8[8];                    // +0xa8..+0xc4（=-1）
    int m_fieldc8[8];                    // +0xc8..+0xe4（=-1）
    int m_fielde8[8];                    // +0xe8..+0x104（=-1）
};  // sizeof == 0x108

// STInitialTarget：C1 W 0x08a3d892（string@4、-1@0、-1@8、true@0xc）。
struct STInitialTarget
{
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

// STMosterAttackReady：clear W 0x08a3d1b4 / C1 W 0x08a3d1e0。
struct STMosterAttackReady
{
    std::vector<int> m_field00;          // +0x00
    int m_field0c;                       // +0x0c（clear =-1）
    std::vector<std::string> m_field10;  // +0x10
};  // sizeof == 0x1c

// stEnamyDropAvatarElement_t：push_back W 0x088ccbd6 步长 lea 0xc(%eax)，3×int。
struct stEnamyDropAvatarElement_t
{
    int m_field0;
    int m_field4;
    int m_field8;
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
    float m_field08[9];                         // +0x08..+0x2b（clear 两循环清零；float 实证）
    int m_field2c;                              // +0x2c
    std::vector<stEnamyDropAvatarElement_t> m_elements;  // +0x30
};  // sizeof == 0x3c

// STMonsterAIInfo：怪物域 AI 脚本子对象（ORIG 符号族 _ZN14STAIInfoScript*
// W 0x08366f48/C1、D1 0x083671a4、clear 0x083673d4、destroy 0x0836753a）。
// 本域实例占 +0x138..+0x33f 共 0x208 字节（ctor 下一个成员 deque@0x340 实证），
// 比 CharacterAIA2.cpp 域版本少尾部 POD 区；成员集至 +0x204 两域一致
// （copy-ctor 报告 STAIInfoScript/STAIInfoScript.md 权威转写）。该 TU 不包含
// 本头，两处定义互不影响。
class STMonsterAIInfo
{
public:
    STMonsterAIInfo();
    virtual ~STMonsterAIInfo();         // 发 D1/D0/vtable，匹配 ORIG 符号集
    void clear();                       // ORIG W 0x083673d4
    void destroy();                     // ORIG W 0x0836753a

    int m_thinkTerm;                    // +0x04
    int m_destinationChangeTerm;        // +0x08
    std::string m_dir;                  // +0x0c
    std::vector<std::string*> m_patterns[5];       // +0x10 easy/medium/hard/ultimate/hero
    int m_targetingBonus[22];           // +0x4c
    std::vector<int> m_targetingBonusStr[22];      // +0xa4
    int m_keepDistance;                 // +0x1ac
    std::vector<int> m_keepRange[5];    // +0x1b0..+0x1eb
    int m_intelligence;                 // +0x1ec
    int m_field1f0;                     // +0x1f0
    int m_field1f4;                     // +0x1f4
    int m_vision;                       // +0x1f8
    int m_backVision;                   // +0x1fc
    bool m_noChangeTarget;              // +0x200
    char m_pad201[3];
    unsigned char m_flag204;            // +0x204（外层 Clear 置 1）
    char m_pad205[3];                   // +0x205..0x207
};  // sizeof == 0x208

typedef char assert_monster_ai_info_size[(sizeof(STMonsterAIInfo) == 0x208) ? 1 : -1];

// STMonsterParameterCategory：vector 元素（+0x7c），大小 0xc；
// getValueInt/getValueFloat 为 ORIG W 0x88ba480 / W 0x88ba4a2 同名方法
// （实现见 MonsterAbilityLeafC3.cpp）。
struct STMonsterParameterCategory
{
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
    // eax=this）。不显式声明，由编译器按成员序隐式合成，指令级与 ORIG 一致。
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
    int m_field064;                      // +0x064（ctor/Clear 不触碰，op= 拷贝实证）
    int m_field068;                      // +0x068
    int m_field06c;                      // +0x06c
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
    STMonsterAIInfo m_aiInfo;                          // +0x138（0x208，止于 0x340）
    std::deque<STInitialTarget> m_initialTargets;      // +0x340
    bool m_flag368;                      // +0x368（Clear=1）
    char m_pad369[3];                    // +0x369..0x36b
    int m_field36c;                      // +0x36c
    int m_field370;                      // +0x370
    bool m_flag374;                      // +0x374（Clear=false）
    char m_pad375[3];                    // +0x375..0x377
    int m_field378;                      // +0x378
    int m_field37c;                      // +0x37c
    int m_field380;                      // +0x380
    std::vector<STGuard> m_guards;                     // +0x384
    STMonsterEquipment m_equipment;                    // +0x390
    std::vector<STMosterAttackReady> m_attackReadies;  // +0x39c
    bool m_flag3a8;                      // +0x3a8（Clear=false）
    char m_pad3a9[3];                    // +0x3a9..0x3ab
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
    char m_pad56d[3];                    // +0x56d..0x56f
    std::string m_field570;              // +0x570
    int m_targetingTimeTerm;             // +0x574 =1000 [targeting time term]
    int m_warlike;                       // +0x578 =50 [warlike]
    int m_attackDelay;                   // +0x57c =1000 [attack delay]
    bool m_targetingOnAttack;            // +0x580
    bool m_targetingOnDamage;            // +0x581
    char m_pad582[2];                    // +0x582..0x583
    int m_targetingNearest;              // +0x584
    int m_targetingAttacker;             // +0x588
    int m_targetingFromDirection;        // +0x58c
    bool m_targetingLowPhysAttack;       // +0x590
    bool m_targetingLowPhysDefense;      // +0x591
    bool m_targetingLowMagAttack;        // +0x592
    char m_pad593[1];                    // +0x593
    int m_targetingLowMagDefense;        // +0x594
    int m_targetingDowned;               // +0x598
    int m_field59c;                      // +0x59c
    int m_field5a0;                      // +0x5a0
    bool m_field5a4;                     // +0x5a4
    bool m_superarmorOnAttack;           // +0x5a5
    char m_pad5a6[2];                    // +0x5a6..0x5a7
    int m_field5a8;                      // +0x5a8
    std::string m_faceImage;             // +0x5ac [face image]
    int m_field5b0;                      // +0x5b0（face image 第二列 %d）
    // 以下 6 个为独立成员而非数组：C1 逐一 SsC1 实证（合成 ctor 不生成循环）
    std::string m_sound0;                // +0x5b4 [ambient sound]
    std::string m_sound1;                // +0x5b8 [attack sound]
    std::string m_sound2;                // +0x5bc [appear sound]
    std::string m_sound3;                // +0x5c0 [damage sound]
    std::string m_sound4;                // +0x5c4 [die sound]
    std::string m_sound5;                // +0x5c8 [etc sound]
    std::string m_field5cc[2];           // +0x5cc..0x5d3（C1/Clear 均为数组循环实证）
    std::string m_field5d40;             // +0x5d4..0x5e3 独立成员（C1 逐个构造实证）
    std::string m_field5d41;             // +0x5d8
    std::string m_field5d42;             // +0x5dc
    std::string m_field5d43;             // +0x5e0
    std::vector<std::string> m_field5e4; // +0x5e4
    std::vector<std::string> m_field5f0; // +0x5f0
    std::string m_field5fc0;             // +0x5fc..0x613 独立成员（C1 实证）
    std::string m_field5fc1;             // +0x600
    std::string m_field5fc2;             // +0x604
    std::string m_field5fc3;             // +0x608
    std::string m_field5fc4;             // +0x60c
    std::string m_field5fc5;             // +0x610
    std::string m_field614[2];           // +0x614..0x61b
    std::string m_field61c0;             // +0x61c..0x62b 独立成员（C1 实证）
    std::string m_field61c1;             // +0x620
    std::string m_field61c2;             // +0x624
    std::string m_field61c3;             // +0x628
    std::vector<std::string> m_field62c; // +0x62c
    std::vector<std::string> m_field638; // +0x638
    std::string m_field6440;             // +0x644..0x657 独立成员（C1/D1 逐个实证）
    std::string m_field6441;             // +0x648
    std::string m_field6442;             // +0x64c
    std::string m_field6443;             // +0x650
    std::string m_field6444;             // +0x654
    int m_field658;                      // +0x658（Clear=-1）
    int m_field65c;                      // +0x65c
    int m_field660;                      // +0x660
    int m_field664;                      // +0x664
    int m_field668;                      // +0x668
    float m_field66c;                    // +0x66c（Clear=0 经寄存器中转→float 实证）
    float m_field670;                    // +0x670
    bool m_field674;                     // +0x674
    char m_pad675[3];                    // +0x675..0x677
    int m_field678;                      // +0x678 =255
    int m_field67c;                      // +0x67c =255
    bool m_field680;                     // +0x680
    char m_pad681[3];                    // +0x681..0x683
    int m_field684;                      // +0x684 =4
    std::vector<std::vector<int> > m_field688;         // +0x688
    std::string m_waitingMotion;         // +0x694 [waiting motion]
    std::string m_moveMotion;            // +0x698 [move motion]
    std::string m_sitMotion;             // +0x69c [sit motion]
    std::string m_damageMotion1;         // +0x6a0 [damage motion 1]
    std::string m_damageMotion2;         // +0x6a4 [damage motion 2]
    std::string m_downMotion;            // +0x6a8 [down motion]
    std::string m_overturnMotion;        // +0x6ac [overturn motion]
    std::string m_jumpMotion;            // +0x6b0 [jump motion]
    std::string m_jumpAttackMotion;      // +0x6b4 [jumpattack motion]
    std::vector<std::string> m_field6b8; // +0x6b8
    std::vector<std::string> m_field6c4; // +0x6c4
    std::string m_field6d0;              // +0x6d0
    std::vector<int> m_field6d4;         // +0x6d4
    int m_keepDistance;                  // +0x6e0 =-1 [keep distance]
    bool m_isOnTheObject;                // +0x6e4 [is on the object]
    char m_pad6e5[3];                    // +0x6e5..0x6e7
    std::vector<STAttackMethod> m_attackMethods;       // +0x6e8
    int m_field6f4;                      // +0x6f4（Clear=0）
    int m_field6f8;                      // +0x6f8（Clear=0）
    int m_dieEffect[3];                  // +0x6fc..0x707 [die effect]（Clear 0/0/-1）
    std::string m_field708;              // +0x708（ImportMonsterScript 存目录路径）
    std::vector<std::string> m_field70c[111];          // +0x70c..0xc3f
    std::vector<int> m_championElements; // +0xc40（getRandomChampionElement 实证）
    int m_fieldc4c;                      // +0xc4c（Clear=0）
    bool m_flagc50;                      // +0xc50（Clear=false）
    char m_padc51[3];                    // +0xc51..0xc53
    float m_fieldc54;                    // +0xc54（Clear=0 经寄存器中转→float 实证）
    float m_fieldc58;                    // +0xc58
    int m_fieldc5c[5];                   // +0xc5c..0xc6f（Clear 循环 i≤4 清零）
    bool m_flagc70;                      // +0xc70（Clear=false）
    bool m_flagc71;                      // +0xc71（Clear=false）
    char m_padc72[2];                    // +0xc72..0xc73
    int m_fieldc74;                      // +0xc74（Clear=0）
    int m_fieldc78;                      // +0xc78（Clear=0）
    int m_fieldc7c;                      // +0xc7c（Clear=0）
    std::vector<int> m_fieldc80;         // +0xc80
    std::vector<int> m_fieldc8c;         // +0xc8c
    stEnamyDropAvatarInfo_t m_dropAvatarInfo;          // +0xc98（0x3c）
    bool m_flagcd4;                      // +0xcd4（Clear=false）
    char m_padcd5[3];                    // +0xcd5..0xcd7
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
    char m_pad1ca9[3];                   // +0x1ca9..0x1cab
    int m_field1cac;                     // +0x1cac（Clear=-1）
    int m_field1cb0;                     // +0x1cb0（Clear=-1）
    std::vector<int> m_field1cb4;        // +0x1cb4
    int m_field1cc0;                     // +0x1cc0
};  // sizeof == 0x1cc4

typedef char assert_stmonster_script_size[(sizeof(STMonsterScript) == 0x1cc4) ? 1 : -1];

// ---- 自由函数（签名 = ORIG mangled）----
int ImportMonsterScript(STMonsterScript* script, int type, const char* path,
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
