#ifndef GAME_EQUIPMENTPARAMETERINFO_H_
#define GAME_EQUIPMENTPARAMETERINFO_H_

// ============================================================================
// df_game_r 还原 —— EquipmentParameterInfo（G5 装备参数脚本，约 0x480 字节）
// 布局依据 ORIG clear（0x089193c0）/ 析构（0x0891a4c6）推导；
// getEquipment* 访问器（0x089c2e48..0x089c2e96）返回装备能力数组元素引用。
// 解析函数（importEquipmentParameterInfo / readAction / readCondtion /
// scanRandomOptionValue / scanIf / scanThen / importAllSkillItem /
// importItemOverpowerDesc / exportEquipmentParameterScript）在同一 TU。
// ============================================================================

#include <cstddef>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "ScriptValue.h"   // ScriptValue / ChangeStatusData / VariableStat 权威定义

enum ENUM_CHANGE_STATUS_TYPE
{
    ENUM_CHANGE_STATUS_TYPE_0 = 0
};


// ORIG 布局（importUpgradeTable [destruction result] 0x08901aba 实证：
// ctor 0x0891a8c0 后连续三 ScanInt 截断 ushort 写 +0/+2/+4，sizeof 6）。
struct STResultItemCnt
{
    // ORIG C1/C2 W 0x0891a8c0：3×movw 0 逐字段清零。
    STResultItemCnt()
    {
        m_a = 0;
        m_b = 0;
        m_c = 0;
    }

    unsigned short m_a;   // +0x00
    unsigned short m_b;   // +0x02
    unsigned short m_c;   // +0x04
};                        // sizeof 6

    // ORIG W ctor 0x089c2e1e（C2 同址）：m_field0=0x4d(77)、m_field4=0、
    // m_float8=0（edx 中转）、m_fieldc=0。
    struct ItemAuraInfo
    {
    ItemAuraInfo()
    {
        m_field0 = 0x4d;
        m_field4 = 0;
        m_float8 = 0.0f;
        m_fieldc = 0;
    }
    int m_field0;            // +0x00
    unsigned char m_field4;  // +0x04（ORIG CStringMaker::operator()(ItemAuraInfo)
                             //   @0x08627066：movzbl v+0x10(ebp)=v+4 零扩展）
    float m_float8;          // +0x08（同函数 flds v+8 → double 提升）
    int m_fieldc;            // +0x0c（同函数读 v+c）
};                           // sizeof 0x10

static_assert(sizeof(ItemAuraInfo) == 0x10, "ItemAuraInfo size mismatch");

// ORIG 布局依 importSkillDataUp（0x88c049c）反编译：元素按偏移
//   +0x00 job / +0x04 value / +0x08 value / +0x0c percent / +0x10 value /
//   +0x14 key(0..6) / +0x18 module（7×int = 0x1c）。
// ctor（ORIG 0x088c41b6）置 job=0xb、module=0x2，其余清零；dtor（0x088c6b9c）为空。
struct EquipmentSkillDataUp
{
    EquipmentSkillDataUp() { m_job = 0xb; m_v4 = 0; m_v8 = 0; m_percent = 0;
                             m_v10 = 0; m_key = 0; m_module = 0x2; }
    int m_job;      // +0x00 getCharacterJob（ctor 默认 0xb）
    int m_v4;       // +0x04 ScanInt（job 后）
    int m_v8;       // +0x08 分派值（static/level/skill-consume 读 ScanInt，其余 0）
    int m_percent;  // +0x0c 0="+" 1=其它
    int m_v10;      // +0x10 ScanInt（sign 后）
    int m_key;      // +0x14 类型键 0..6（static/level/cooltime/mp/maintain mp/skill consume/casting）
    int m_module;   // +0x18 scanModuleType（ctor 默认 0x2）
};
static_assert(sizeof(EquipmentSkillDataUp) == 0x1c, "EquipmentSkillDataUp size mismatch");

// LevelUpSkill（ORIG C1 0x081803a8 W / clear 0x08180386 W / D1 0x0818250e W）：
// clear 置 m_job=0xb(11)、m_field4=0、m_field8=0；ctor 调 clear。
struct LevelUpSkill
{
    int m_job;      // +0x00（clear 默认 0xb）
    int m_field4;   // +0x04
    int m_field8;   // +0x08

    LevelUpSkill() { clear(); }                      // ORIG 0x081803a8
    void clear()                                     // ORIG 0x08180386
    {
        m_job = 0xb;
        m_field4 = 0;
        m_field8 = 0;
    }
};

// D1 0x089d2af0 W）：clear 置 +0/+4/+8 三字段为 0 并对 +0xc 的 LevelUpSkill
// 调 clear；ctor 先构造内嵌 LevelUpSkill 再调 clear。sizeof 0x18
// （AvatarRouletteServer::InitAddCharacSkill 0x0817f386 读元素 +0xc/+0x10/+0x14
// 实证；importEquipmentParameterInfo 0x089ab614 以 fstps 写 +0x08 → float，P79）。
struct STAvatarAbilityVariation
{
    int m_field0;        // +0x00
    int m_field4;        // +0x04
    float m_field8;      // +0x08（fstps 实证：avatar 能力数值）
    LevelUpSkill m_skill;   // +0x0c

    STAvatarAbilityVariation() { clear(); }          // ORIG 0x089c2d82（先 skill 构造再 clear）
    void clear()                                     // ORIG 0x089c2da4
    {
        m_field0 = 0;
        m_field4 = 0;
        m_field8 = 0;
        m_skill.clear();
    }
    ~STAvatarAbilityVariation() {}                   // ORIG 0x089d2af0（空）
};

class EquipmentParameterInfo
{
public:
    EquipmentParameterInfo();
    EquipmentParameterInfo(const EquipmentParameterInfo& other);
    ~EquipmentParameterInfo();
    // operator= 不声明：ORIG 0x83662ca 为 W（weak）且无 self-check，
    // 编译器隐式生成与 ORIG 同形。

    void clear();
    // ORIG 0x089c2e98 返回 int（key）；返回类型随 ORIG。
    int addString(std::string str);
    int& getEquipmentPhysicalAttack(int level);
    int& getEquipmentPhysicalDefense(int level);
    int& getEquipmentMagicalAttack(int level);
    int& getEquipmentMagicalDefense(int level);

    // ORIG GetLevelUpSkill 0x0861d472 签名嵌套枚举：param*0xc+script+0x40c
    // 选第 idx 档 vector<LevelUpSkill>（成员名 DWARF 无 C++ 信息，按档位命名）。
    enum ENUM_SKILL_UP_VECTOR_INDEX
    {
        ENUM_SKILL_UP_VECTOR_INDEX_0 = 0,
        ENUM_SKILL_UP_VECTOR_INDEX_1 = 1,
        ENUM_SKILL_UP_VECTOR_INDEX_2 = 2
    };

    struct RebirthAppendageinfo
    {
        RebirthAppendageinfo();
        void clear();
        // ORIG（C1 0x891935e / clear 0x8919380）：+0x04 为 std::pair<int,int>
        // （ctor 先构造 pair 再调 clear；clear 置 first=-1 second=0），
        // +0x0c/+0x10/+0x14 为 0。sizeof 0x18。
        int m_field0;              // +0x00
        std::pair<int, int> m_pair4;  // +0x04
        int m_fieldc;              // +0x0c
        int m_field10;             // +0x10
        int m_field14;             // +0x14
    };

    struct EquipmentAuraAppendageData
    {
        // ORIG W 0x089d2cf2（D2 同址）：空体用户析构（6 字节 push/mov/pop/ret）。
        // sizeof = 0x14（5×int）：ORIG push_back 0x089c6e76 `lea 0x14(%eax),%edx`
        // （end = begin + 元素尺寸 0x14）。[ice appendage] 写 {0,i1,i2,i3}（第 5 个
        // int ORIG 不写，push_back 仍拷 0x14），[curse appendage] 写满 5 个（type=1）。
        ~EquipmentAuraAppendageData() {}
        int m_field0;    // +0x00 类型：[ice appendage]=0 / [curse appendage]=1
        int m_field4;    // +0x04 第 1 个 ScanInt
        int m_field8;    // +0x08 第 2 个 ScanInt
        int m_fieldc;    // +0x0c 第 3 个 ScanInt
        int m_field10;   // +0x10 第 4 个 ScanInt（[ice appendage] 不写）
    };

    struct SASPROPERTY
    {
        // ORIG（C1 0x89c5046 / copy 0x83cb038 / D1 0x83db300 / assign 0x83dbdfc）
        // 全部为 W（weak）= 编译器隐式生成：C1 默认构造 vector/string、
        // copy/assign 逐字段「重载双基址」（`mov 0xc(%ebp),%eax; mov (%eax),%edx;
        // mov 0x8(%ebp),%eax; mov %edx,(%eax)` ×8）再拷贝 vector/string、
        // D1 逆序析构。故不声明任何特殊成员。
        // [b28d-solo-EquipmentParameterInfo] 8 个 int 必须是**独立成员**而非
        // `int[8]`：4.4 对数组成员的隐式 C2ERKS/op= 恒缓存双基址（+ecx 中转），
        // 对独立标量成员则逐条重载双基址 = ORIG 形（探针 /tmp/b28d_epi/p10.cpp
        // SA(数组) vs SC(独立)，C2ERKS 与 aSERKS_ 双函数逐条同形）。
        // sizeof/偏移零变化（8×4=0x20，后续 m_vec20@+0x20、m_str2c@+0x2c 不变）。
        int m_head0;                    // +0x00
        int m_head1;                    // +0x04
        int m_head2;                    // +0x08
        int m_head3;                    // +0x0c
        int m_head4;                    // +0x10
        int m_head5;                    // +0x14
        int m_head6;                    // +0x18
        int m_head7;                    // +0x1c
        std::vector<float> m_vec20;     // +0x20
        std::string m_str2c;            // +0x2c
    };

    // ---- 成员布局 ----
    // 低区（+0x00..+0x53）全为标量（ORIG ctor 首个容器在 +0x158，无 string/vector）。
    int m_field0;                                      // +0x00（clear 置 2）
    int m_hpMax;                                       // +0x04 [HP MAX]
    float m_hpMaxRate;                                 // +0x08 [HP MAX rate]
    int m_mpMax;                                       // +0x0c [MP MAX]
    float m_mpMaxRate;                                 // +0x10 [MP MAX rate]
    int m_physicalAttack;                              // +0x14 [physical attack]
    int m_physicalDefense;                             // +0x18 [physical defense]
    int m_magicalAttack;                               // +0x1c [magical attack]
    int m_20;                                          // +0x20（标量，clear 清零；语义未在 import/export 出现）
    int m_magicalDefense;                              // +0x24 [magical defense]
    // [level linear ability] 起始槽（set_item 拷作 growth A/B）：
    int m_growthLevelBegin;                            // +0x28（[level linear ability] 首 ScanInt；set_item growthA）
    int m_growthLevelEnd;                              // +0x2c（[level linear ability] 次 ScanInt；set_item growthB）
    // +0x30..0x4f：[level linear ability] 基础四组各 2 int（ORIG 内层子标签
    // [physical attack]/[physical defense]/[magical attack]/[magical defense]，
    // (i+0xc/0xe/0x10/0x12)*4 序列）。ORIG 隐式 op=（0x8366361）按 4 个 8 字节对
    // 拷贝；clear（0x89193c0）按 (int*)this[i+12/14/16/18] 循环清零。
    int m_linPhyAtk[2];                                // +0x30
    int m_linPhyDef[2];                                // +0x38
    int m_linMagAtk[2];                                // +0x40
    int m_linMagDef[2];                                // +0x48
    int m_50;                                          // +0x50（clear 清零；语义未知）
    int m_phyAtt[2];                                   // +0x54（getEquipmentPhysicalAttack 0x89c2e48：this+(level+0x14)*4+4）
    int m_phyDef[2];                                   // +0x5c（getEquipmentPhysicalDefense 0x89c2e5c：this+(level+0x14)*4+0xc）
    int m_magAtt[2];                                   // +0x64（getEquipmentMagicalAttack 0x89c2e70：this+(level+0x18)*4+4）
    int m_magDef[2];                                   // +0x6c（getEquipmentMagicalDefense 0x89c2e84：this+(level+0x18)*4+0xc）
    int m_physicalAbsoluteDamage;                      // +0x74 [physical absolute damage]
    int m_physicalDamageReduce;                        // +0x78 [physical damage reduce]
    float m_physicalAbsoluteDefense;                   // +0x7c [physical absolute defense]
    int m_magicalAbsoluteDamage;                       // +0x80 [magical absolute damage]
    int m_magicalDamageReduce;                         // +0x84 [magical damage reduce]
    float m_magicalAbsoluteDefense;                    // +0x88 [magical absolute defense]
    float m_8c;                                        // +0x8c（float，clear 清零）
    float m_90;                                        // +0x90（float，clear 清零）
    int m_94;                                          // +0x94（int，clear 清零）
    int m_98;                                          // +0x98（int，clear 清零）
    float m_9c;                                        // +0x9c（float，clear 清零）
    int m_a0;                                          // +0xa0（int，clear 清零）
    int m_a4;                                          // +0xa4（int，clear 清零）
    float m_a8;                                        // +0xa8（float，clear 清零）
    int m_ac;                                          // +0xac（int，clear 清零）
    // [b28d-solo-EquipmentParameterInfo] P246/P247：ORIG 隐式 op=（0x83664f9）对这 4 个
    // 相邻 int 发「双基址缓存 + 升序 4 连发」= 数组成员形；独立标量恒逐字段重载
    // （探针 /tmp/b28d_epi_probe/mix.cpp MixA vs MixB/C）。匿名 union 保具名访问不变。
    union {
        int m_elementResistance[4];                    // +0xb0（op= 块拷贝视图）
        struct {
            int m_fireResistance;                      // +0xb0 [fire resistance]
            int m_waterResistance;                     // +0xb4 [water resistance]
            int m_darkResistance;                      // +0xb8 [dark resistance]
            int m_lightResistance;                     // +0xbc [light resistance]
        };
    };
    int m_allElementalResistance;                      // +0xc0 [all elemental resistance]
    // [b28d-solo-EquipmentParameterInfo] 同上：ORIG op= 对 +0xc4..0x104 发 17 dword
    // `rep movsl`（计数 0x11）——数组成员形（探针 mix.cpp MixF 逐条同形）。
    union {
        int m_statusResistance[17];                    // +0xc4..0x104（op= rep movsl 视图）
        struct {
            int m_slowResistance;                      // +0xc4 [slow resistance]
            int m_freezeResistance;                    // +0xc8 [freeze resistance]
            int m_poisonResistance;                    // +0xcc [poison resistance]
            int m_stunResistance;                      // +0xd0 [stun resistance]
            int m_curseResistance;                     // +0xd4 [curse resistance]
            int m_blindResistance;                     // +0xd8 [blind resistance]
            int m_lightningResistance;                 // +0xdc [lightning resistance]
            int m_stoneResistance;                     // +0xe0 [stone resistance]
            int m_sleepResistance;                     // +0xe4 [sleep resistance]
            int m_burnResistance;                      // +0xe8 [burn resistance]
            int m_weaponBreakResistance;               // +0xec [weapon break resistance]
            int m_bleedingResistance;                  // +0xf0 [bleeding resistance]
            int m_piercingResistance;                  // +0xf4 [piercing resistance]
            int m_stuckResistance;                     // +0xf8 [stuck resistance]
            int m_confuseResistance;                   // +0xfc [confuse resistance]
            int m_holdResistance;                      // +0x100 [hold resistance]
            int m_armorBreakResistance;                // +0x104 [armor break resistance]
        };
    };
    int m_allActiveStatusResistance;                   // +0x108 [all activestatus resistance]
    int m_inventoryLimit;                              // +0x10c [inventory limit]
    int m_hpRegenSpeed;                                // +0x110 [HP regen speed]
    int m_mpRegenSpeed;                                // +0x114 [MP regen speed]
    float m_mpRegenSpeedRate;                          // +0x118 [MP regen speed rate]
    int m_moveSpeed;                                   // +0x11c [move speed]
    // [b28d-solo-EquipmentParameterInfo] ORIG op= 对 +0x120/+0x124 发 8 字节对（先高后低
    // 读）——float[2] 形（探针 mix.cpp MixE）。
    union {
        float m_moveDashSpeed[2];                      // +0x120（op= 8 字节对视图）
        struct {
            float m_xMoveDashSpeed;                    // +0x120 [x move dash speed]
            float m_yMoveDashSpeed;                    // +0x124 [y move dash speed]
        };
    };
    int m_attackSpeed;                                 // +0x128 [attack speed]
    int m_castSpeed;                                   // +0x12c [cast speed]
    int m_hitRecovery;                                 // +0x130 [hit recovery]
    int m_jumpPower;                                   // +0x134 [jump power]
    float m_physicalCriticalHit;                       // +0x138 [physical critical hit]
    float m_magicalCriticalHit;                        // +0x13c [magical critical hit]
    int m_elementalProperty;                           // +0x140 [elemental property]（getElement）
    int m_activeStatusType;                            // +0x144 [active status] type（getActiveStatusType）
    float m_activeStatusValue;                         // +0x148 [active status] value
    int m_activeStatusLevel;                           // +0x14c [active status] level
    bool m_activeStatusFlag;                           // +0x150 [active status] flag
    int m_activeStatusValue2;                          // +0x154 [active status] value2
    std::vector<int> m_vec158;                         // +0x158（[active status]/[blind]..[curse] 附加参数 push_back）
    float m_164;                                       // +0x164（clear 清零）
    int m_attackedActiveStatusType;                    // +0x168 [attacked active status] type（getActiveStatusType）
    float m_attackedActiveStatusValue;                 // +0x16c [attacked active status] value
    int m_attackedActiveStatusLevel;                   // +0x170 [attacked active status] level
    bool m_attackedActiveStatusFlag;                   // +0x174 [attacked active status] flag
    int m_attackedActiveStatusValue2;                  // +0x178 [attacked active status] value2
    std::vector<int> m_vec17c;                         // +0x17c（[blind]..[curse] attacked 附加参数 push_back）
    std::vector<ItemAuraInfo> m_aura188;               // +0x188
    std::vector<int> m_vec194;                         // +0x194（[additional effect index] push_back）
    int m_addAbsoluteDamageType;                       // +0x1a0 [add absolute damage] type（0=physical/1=magical）
    float m_addAbsoluteDamageValue;                    // +0x1a4 [add absolute damage] value
    int m_addAbsoluteDamageExtra;                      // +0x1a8 [add absolute damage] extra
    int m_addAbsoluteDefenseType;                      // +0x1ac [add absolute defense] type（2=physical/3=magical）
    float m_addAbsoluteDefenseValue;                   // +0x1b0 [add absolute defense] value
    int m_addAbsoluteDefenseExtra;                     // +0x1b4 [add absolute defense] extra
    std::vector<EquipmentSkillDataUp> m_skill1b8;      // +0x1b8
    std::map<int, std::string> m_descMap1c4;           // +0x1c4
    std::vector<EquipmentSkillDataUp> m_skill1dc;      // +0x1dc
    std::vector<std::vector<EquipmentSkillDataUp> > m_skill1e8;  // +0x1e8
    std::vector<std::string> m_vec1f4;                 // +0x1f4
    std::vector<EquipmentAuraAppendageData> m_aura200; // +0x200（[curse appendage]/[ice appendage]）
    float m_decreaseCriticalDamage;                    // +0x20c [decrease critical damage]（local/10，float）
    float m_210;                                       // +0x210（ORIG 0x89aca58 mov 32 位浮点位模式写入；clear 清零）
    std::vector<int> m_vec214;                         // +0x214（[hardcoding parameter] push_back）
    int m_pushAside;                                   // +0x220 [push aside]
    int m_liftUp;                                      // +0x224 [lift up]
    float m_stuck;                                     // +0x228 [stuck]
    bool m_ignoreDefense;                              // +0x22c [ignore defense]
    // [b28d-solo-EquipmentParameterInfo] ORIG op= 对 +0x230..0x238 发双基址缓存 3 连发
    // ——float[3] 形（探针 mix.cpp MixD）。
    union {
        float m_damageRate[3];                         // +0x230（op= 3 连发视图）
        struct {
            float m_humanDamageRate;                   // +0x230 [human damage rate]
            float m_fortDamageRate;                    // +0x234 [fort damage rate]
            float m_monsterDamageRate;                 // +0x238 [monster damage rate]
        };
    };
    bool m_hpDrain;                                    // +0x23c [HP drain]
    bool m_mpDrain;                                    // +0x23d [MP drain]
    std::vector<std::vector<float> > m_vec240;         // +0x240（[level linear ability] 表）
    // ORIG C1（0x8919ce8）对 +0x24c/+0x258 两个 vector<LevelUpSkill> 用
    // do-while 循环逐个构造（esi/edi/ebx 缓存），故建模为数组成员。
    std::vector<LevelUpSkill> m_levelUpSkills[2];      // +0x24c/+0x258（skill_levelup 档位；GetLevelUpSkill idx）
    std::vector<STAvatarAbilityVariation> m_avatar264; // +0x264（[avatar select ability]）
    int m_270;                                         // +0x270（clear 置 -1）
    float m_roomListMoveSpeedRate;                     // +0x274 [room list move speed rate]
    std::string m_str278;                              // +0x278
    std::string m_str27c;                              // +0x27c
    int m_antiEvil;                                    // +0x280 [anti evil]
    int m_rigidity;                                    // +0x284 [rigidity]
    // [b28d-solo-EquipmentParameterInfo] ORIG op= 对 +0x288..0x294 发双基址缓存 4 连发
    // ——int[4] 形（探针 mix.cpp MixB）。
    union {
        int m_elementAttack[4];                        // +0x288（op= 4 连发视图）
        struct {
            int m_fireAttack;                          // +0x288 [fire attack]
            int m_waterAttack;                         // +0x28c [water attack]
            int m_darkAttack;                          // +0x290 [dark attack]
            int m_lightAttack;                         // +0x294 [light attack]
        };
    };
    int m_allElementalAttack;                          // +0x298 [all elemental attack]
    // +0x29c..0x373：状态概率/等级 18 槽表（export/import [active status
    // probability]/[active status level]，base (i+0xa4)*4+0xc / (i+0xb8)*4+4）。
    int m_activeStatusProbability[18];                 // +0x29c..0x2e3
    int m_activeStatusLevelTable[18];                       // +0x2e4..0x32b
    int m_activeStatusCtrl[18];                        // +0x32c..0x373（control info 第三表，(i+0xc8)*4+0xc）
    int m_activeStatusControlInfo[5];                  // +0x374..0x384 [active status control info]（control info 特判列）
    int m_separateAttack[2];                         // +0x388/+0x38c
    float m_separateRate[2];                          // +0x390/+0x394
    int m_398;                                        // +0x398
    int m_39c;                                        // +0x39c（clear 清零；未知语义）
    int m_linEqPhyAtk[4];                             // +0x3a0（[equipment physical attack]，4 档）
    int m_linEqPhyDef[4];                             // +0x3b0（[equipment physical defense]）
    int m_linEqMagAtk[4];                             // +0x3c0（[equipment magical attack]）
    int m_linEqMagDef[4];                             // +0x3d0（[equipment magical defense]）
    // [b28d-solo-EquipmentParameterInfo] ORIG op= 对 +0x3e0/+0x3e4 发 8 字节对（先高后低读）
    // ——int[2] 形（探针 mix.cpp MixE 同族）。
    union {
        int m_growthGrade[2];                          // +0x3e0（op= 8 字节对视图）
        struct {
            int m_growthGradeBegin;                    // +0x3e0（[grade] 首值；set_item growthC）
            int m_growthGradeEnd;                      // +0x3e4（[grade] 次值；set_item growthD）
        };
    };
    int m_linSepAtk[4];                               // +0x3e8（[separate attack]，4 档）
    float m_linSepRate[4];                            // +0x3f8（[separate rate]，4 档 float）
    // [b28d-solo-EquipmentParameterInfo] 同 +0x3e0：ORIG op= 对 +0x408/+0x40c 发 8 字节对。
    union {
        int m_growthRepair[2];                         // +0x408（op= 8 字节对视图）
        struct {
            int m_growthRepairBegin;                   // +0x408（[repair price] 首值；set_item growthE）
            int m_growthRepairEnd;                     // +0x40c（[repair price] 次值；set_item growthF）
        };
    };
    int m_410;                                         // +0x410
    int m_414;                                         // +0x414
    // +0x418..0x434 = ORIG `._119` 匿名结构（importAllSkillItem/importItemOverpowerDesc
    // 写入）。ORIG 符号 _ZN22EquipmentParameterInfo5._119C1/C2/D1/D2/aSERKS0_ t
    // （0x088fd884/0x0898c680 两 TU 各一份）+ ._159aSERKS0_（另一 TU 同形副本）。
    // GCC 无法声明 `class ._119`；命名类取 5 字符 `t_119` 使自然 mangled 段
    // `5t_119` 与 ORIG 别名 `5._119` 同长（objdump 同址取较短/同长时字典序小者，
    // 探针 /tmp/b28c_l6_alias_probe6.s 实证：同长时点号别名胜出）。ctor/dtor/
    // operator= 在 EquipmentParameterInfo.cpp 显式定义 + `.set` 汇编别名强制
    // ORIG 带点号符号（mangling 必要性，非 §4.5 违例）。
    // 成员经 m_119. 前缀访问（原匿名提升语义由 .set 别名符号保持）。
    class t_119
    {
    public:
        t_119();     // ORIG _ZN22EquipmentParameterInfo5._119C1Ev t 0x088fd884（构造 +0/+0x10 两 vector）
        ~t_119();    // ORIG _ZN22EquipmentParameterInfo5._119D1Ev t 0x088fd8ca（逆序析构）
        // [b28d-solo-EquipmentParameterInfo] asm 标签实测对 operator= **有效**（探针
        // /tmp/b28d_epi_probe/asmlabel.cpp：声明带标签 ⇒ 定义与调用点重定位均用标签名），
        // 故不再用 `.set`。ORIG 的 EquipmentParameterInfo::operator= 调用点是
        // `._159aSERKS0_`（0x08348024，成员在 ORIG 侧为匿名 struct `._159`）⇒ 本成员类型的
        // op= 标签取 `._159aSERKS0_` 才能让 p167 调用点差集归零；同形的 `._119aSERKS0_`
        // （ORIG 0x088fd910）由 tt_159 的 op= 标签独立发射（P262：两别名须各有独立体）。
        t_119& operator=(const t_119& other)
            asm("_ZN22EquipmentParameterInfo5._159aSERKS0_");  // ORIG _159aSERKS0_ t 0x08348024
        std::vector<SASPROPERTY> m_418;          // +0x418（importAllSkillItem push_back）
        char m_424;                              // +0x424（importItemOverpowerDesc 置 1）
        char m_425;                              // +0x425（importItemOverpowerDesc 置 1）
        char m_pad426[2];                        // +0x426..0x427
        std::vector<std::vector<float> > m_428;  // +0x428（importItemOverpowerDesc 写入）
    } m_119;                                     // +0x418（._119 类型实例）
    // ORIG 另一 TU 的同形副本 `._159`（0x08348024/0x0861c9b8），仅 aSERKS0_，
    // 函数体与本 TU `._119aSERKS0_` 逐条相同（norm 实测相等）。同址多别名时
    // objdump 只标注一个 ⇒ 必须独立类+独立函数体才能同时闭合两个点号符号
    // （P203 附注；无成员实例，不影响布局）。
    // 类名 6 字符 `tt_159`（自然名段 `6tt_159` 7 字符 > 别名段 `5._159` 6 字符）
    // ⇒ objdump 同址取更短名时点号别名胜出（探针 /tmp/b28c_l6_alias_probe1.s）。
    class tt_159
    {
    public:
        // 标签与类名交叉（t_119 的 op= 发 `._159`，本类发 `._119`）是为了同时匹配 ORIG
        // 的两份同形副本地址（0x08348024 / 0x088fd910）；两者函数体独立。
        tt_159& operator=(const tt_159& other)
            asm("_ZN22EquipmentParameterInfo5._119aSERKS0_");  // ORIG _119aSERKS0_ t 0x088fd910
        std::vector<SASPROPERTY> m_418;
        char m_424;
        char m_425;
        char m_pad426[2];
        std::vector<std::vector<float> > m_428;
    };
    EquipmentParameterInfo* m_434;                     // +0x434（scanRandomOptionValue 传入的 EquipmentParameterInfo* 子指针）
    int m_438;                                         // +0x438（clear 置 -1）
    int m_43c;                                         // +0x43c（clear 置 -1）
    char m_440;                                        // +0x440（clear 置 0）
    // b28d-55（P26/P74）：ORIG 隐式 operator= 拷贝段**跳过** +0x441..0x443
    // （0x080f.../0x08366xx 段实证：+0x440 后直接 +0x444）⇒ 该 3 字节是编译器
    // 自然对齐填充，非成员。显式 char[3] 会被逐字节搬移，删除后由编译器填充。
    float m_upgradeCostDiscount;                       // +0x444 [upgrade cost discount]
    float m_upgradeProbIncrease;                       // +0x448 [upgrade prob increase]
    float m_assaultCostDiscount;                       // +0x44c [assault cost discount]
    // +0x450..0x464 = RebirthAppendageinfo（ORIG clear 0x8919cb7 调其 clear；
    //   import 写 +0x450/+0x454/+0x458/+0x45c/+0x464/+0x460）
    RebirthAppendageinfo m_rebirth450;                 // +0x450
    std::vector<std::pair<int, int> > m_vec468;        // +0x468
    std::vector<VariableStat> m_var474;                // +0x474
};

// ---- 尺寸 / 关键偏移断言（ORIG importEquipmentParameterInfo 0x089a75ea 字段布局）----
// GCC4.4 下 offsetof 不能用于含非平凡构造/析构的类（本类有用户 ctor/dtor/operator=）。
// 采用 LayoutMirror：与真实类同布局的标准布局 struct，对镜像取 offsetof 即验证 ORIG 偏移，无警告。
struct EquipmentParameterInfoLayoutMirror
{
    int m_field0;
    int m_hpMax;
    float m_hpMaxRate;
    int m_mpMax;
    float m_mpMaxRate;
    int m_physicalAttack;
    int m_physicalDefense;
    int m_magicalAttack;
    int m_20;
    int m_magicalDefense;
    int m_growthLevelBegin;
    int m_growthLevelEnd;
    int m_linPhyAtk[2];
    int m_linPhyDef[2];
    int m_linMagAtk[2];
    int m_linMagDef[2];
    int m_50;
    int m_phyAtt[2];
    int m_phyDef[2];
    int m_magAtt[2];
    int m_magDef[2];
    int m_physicalAbsoluteDamage;
    int m_physicalDamageReduce;
    float m_physicalAbsoluteDefense;
    int m_magicalAbsoluteDamage;
    int m_magicalDamageReduce;
    float m_magicalAbsoluteDefense;
    float m_8c;
    float m_90;
    int m_94;
    int m_98;
    float m_9c;
    int m_a0;
    int m_a4;
    float m_a8;
    int m_ac;
    int m_fireResistance;
    int m_waterResistance;
    int m_darkResistance;
    int m_lightResistance;
    int m_allElementalResistance;
    int m_slowResistance;
    int m_freezeResistance;
    int m_poisonResistance;
    int m_stunResistance;
    int m_curseResistance;
    int m_blindResistance;
    int m_lightningResistance;
    int m_stoneResistance;
    int m_sleepResistance;
    int m_burnResistance;
    int m_weaponBreakResistance;
    int m_bleedingResistance;
    int m_piercingResistance;
    int m_stuckResistance;
    int m_confuseResistance;
    int m_holdResistance;
    int m_armorBreakResistance;
    int m_allActiveStatusResistance;
    int m_inventoryLimit;
    int m_hpRegenSpeed;
    int m_mpRegenSpeed;
    float m_mpRegenSpeedRate;
    int m_moveSpeed;
    float m_xMoveDashSpeed;
    float m_yMoveDashSpeed;
    int m_attackSpeed;
    int m_castSpeed;
    int m_hitRecovery;
    int m_jumpPower;
    float m_physicalCriticalHit;
    float m_magicalCriticalHit;
    int m_elementalProperty;
    int m_activeStatusType;
    float m_activeStatusValue;
    int m_activeStatusLevel;
    bool m_activeStatusFlag;
    int m_activeStatusValue2;
    char m_vec158[sizeof(std::vector<int>)];
    float m_164;
    int m_attackedActiveStatusType;
    float m_attackedActiveStatusValue;
    int m_attackedActiveStatusLevel;
    bool m_attackedActiveStatusFlag;
    int m_attackedActiveStatusValue2;
    char m_vec17c[sizeof(std::vector<int>)];
    char m_aura188[sizeof(std::vector<ItemAuraInfo>)];
    char m_vec194[sizeof(std::vector<int>)];
    int m_addAbsoluteDamageType;
    float m_addAbsoluteDamageValue;
    int m_addAbsoluteDamageExtra;
    int m_addAbsoluteDefenseType;
    float m_addAbsoluteDefenseValue;
    int m_addAbsoluteDefenseExtra;
    char m_skill1b8[sizeof(std::vector<EquipmentSkillDataUp>)];
    char m_descMap1c4[sizeof(std::map<int, std::string>)];
    char m_skill1dc[sizeof(std::vector<EquipmentSkillDataUp>)];
    char m_skill1e8[sizeof(std::vector<std::vector<EquipmentSkillDataUp> >)];
    char m_vec1f4[sizeof(std::vector<std::string>)];
    char m_aura200[sizeof(std::vector<EquipmentParameterInfo::EquipmentAuraAppendageData>)];
    float m_decreaseCriticalDamage;
    float m_210;
    char m_vec214[sizeof(std::vector<int>)];
    int m_pushAside;
    int m_liftUp;
    float m_stuck;
    bool m_ignoreDefense;
    float m_humanDamageRate;
    float m_fortDamageRate;
    float m_monsterDamageRate;
    bool m_hpDrain;
    bool m_mpDrain;
    char m_vec240[sizeof(std::vector<std::vector<float> >)];
    char m_levelUpSkills[2][sizeof(std::vector<LevelUpSkill>)];
    char m_avatar264[sizeof(std::vector<STAvatarAbilityVariation>)];
    int m_270;
    float m_roomListMoveSpeedRate;
    char m_str278[sizeof(std::string)];
    char m_str27c[sizeof(std::string)];
    int m_antiEvil;
    int m_rigidity;
    int m_fireAttack;
    int m_waterAttack;
    int m_darkAttack;
    int m_lightAttack;
    int m_allElementalAttack;
    int m_activeStatusProbability[18];
    int m_activeStatusLevelTable[18];
    int m_activeStatusCtrl[18];
    int m_activeStatusControlInfo[5];
    int m_separateAttack[2];
    float m_separateRate[2];
    int m_398;
    int m_39c;
    int m_linEqPhyAtk[4];
    int m_linEqPhyDef[4];
    int m_linEqMagAtk[4];
    int m_linEqMagDef[4];
    int m_growthGradeBegin;
    int m_growthGradeEnd;
    int m_linSepAtk[4];
    float m_linSepRate[4];
    int m_growthRepairBegin;
    int m_growthRepairEnd;
    int m_410;
    int m_414;
    char m_pad418[sizeof(std::vector<EquipmentParameterInfo::SASPROPERTY>)];
    char m_424;
    char m_425;
    char m_pad426[2];
    char m_428[sizeof(std::vector<std::vector<float> >)];
    void* m_434;
    int m_438;
    int m_43c;
    char m_440;
    float m_upgradeCostDiscount;
    float m_upgradeProbIncrease;
    float m_assaultCostDiscount;
    char m_rebirth450[sizeof(EquipmentParameterInfo::RebirthAppendageinfo)];
    char m_vec468[sizeof(std::vector<std::pair<int, int> >)];
    char m_var474[sizeof(std::vector<VariableStat>)];
};
static_assert(sizeof(EquipmentParameterInfo) == sizeof(EquipmentParameterInfoLayoutMirror),
              "EquipmentParameterInfo size mismatch vs mirror");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_phyAtt) == 0x54,
              "m_phyAtt +0x54 (getEquipmentPhysicalAttack)");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_phyDef) == 0x5c,
              "m_phyDef +0x5c (getEquipmentPhysicalDefense)");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_magAtt) == 0x64,
              "m_magAtt +0x64 (getEquipmentMagicalAttack)");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_magDef) == 0x6c,
              "m_magDef +0x6c (getEquipmentMagicalDefense)");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_fireResistance) == 0xb0,
              "m_fireResistance +0xb0 [fire resistance]");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_allActiveStatusResistance) == 0x108,
              "m_allActiveStatusResistance +0x108 [all activestatus resistance]");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_moveSpeed) == 0x11c,
              "m_moveSpeed +0x11c [move speed]");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_attackSpeed) == 0x128,
              "m_attackSpeed +0x128 [attack speed]");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_vec158) == 0x158,
              "m_vec158 +0x158 ([active status] 附加 vector)");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_var474) == 0x474,
              "m_var474 +0x474");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_pad418) == 0x418,
              "m_pad418 (vector<SASPROPERTY>) +0x418 (importAllSkillItem push_back)");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_424) == 0x424,
              "m_424 +0x424 (importItemOverpowerDesc 置 1)");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_425) == 0x425,
              "m_425 +0x425 (importItemOverpowerDesc 置 1)");
static_assert(offsetof(EquipmentParameterInfoLayoutMirror, m_428) == 0x428,
              "m_428 (vector<vector<float>>) +0x428 (importItemOverpowerDesc push_back)");
static_assert(sizeof(EquipmentParameterInfo::EquipmentAuraAppendageData) == 0x14,
              "EquipmentAuraAppendageData sizeof 0x14 (ORIG push_back 0x089c6e76 lea 0x14)");

// ---- 解析/导出函数（ORIG 符号） ----
bool importEquipmentParameterInfo(EquipmentParameterInfo* info,
                                  std::string& path, const char* dir,
                                  int& a, int& b);
void exportEquipmentParameterScript(FILE* file, EquipmentParameterInfo* info);
// ORIG _Z10readActionRKSsP22EquipmentParameterInfoRSsRSt6vectorIfSaIfEEi 返回 bool
//（scanThen 以其返回值决定是否继续）。
bool readAction(const std::string& line, EquipmentParameterInfo* info,
                std::string& out, std::vector<float>& values, int idx);
void readCondtion(EquipmentParameterInfo* info, std::string& line,
                  std::vector<int>& values);
bool scanRandomOptionValue(EquipmentParameterInfo* info, std::string& line);
void scanIf(EquipmentParameterInfo* info, int idx);
// ORIG _Z8scanThenSsP22EquipmentParameterInfoi 返回 bool（"[/then]" → true）。
bool scanThen(std::string line, EquipmentParameterInfo* info, int idx);
// ORIG importAllSkillItem（0x089bec41）/importItemOverpowerDesc（0x089bf290）为
// 文件内 static（t）helper，定义在 EquipmentParameterInfo.cpp，不对外声明。
// ORIG _Z17importSkillDataUpRSt6vectorI20EquipmentSkillDataUpSaIS0_EE（0x088c049c）：
// 从当前词法流逐条解析 equipment skill 数据（job/value/module + 标签/等级/百分比/数值），
// 追加到 out（先 reserve 0x14）。
void importSkillDataUp(std::vector<EquipmentSkillDataUp>& out);

#endif  // GAME_EQUIPMENTPARAMETERINFO_H_
