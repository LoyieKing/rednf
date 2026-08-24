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


struct STResultItemCnt
{
    int m_field0;   // +0x00
};

struct ItemAuraInfo
{
    int m_field0;   // +0x00
};

// ORIG 布局依 importSkillDataUp（0x88c049c）反编译：元素按偏移
//   +0x00 job / +0x04 value / +0x08 value / +0x0c percent / +0x10 value /
//   +0x14 key(0..6) / +0x18 module（7×int = 0x1c）。
// ctor（ORIG 0x088c41b6）置 job=0xb、module=0x2，其余清零；dtor（0x088c6b9c）为空。
struct EquipmentSkillDataUp
{
    EquipmentSkillDataUp() { m_job = 0xb; m_v4 = 0; m_v8 = 0; m_percent = 0;
                             m_v10 = 0; m_key = 0; m_module = 0x2; }
    ~EquipmentSkillDataUp() {}
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
    ~LevelUpSkill() {}                               // ORIG 0x0818250e（空）
};

// STAvatarAbilityVariation（ORIG C1 0x089c2d82 W / clear 0x089c2da4 W /
// D1 0x089d2af0 W）：clear 置 +0/+4/+8 三 int 为 0 并对 +0xc 的 LevelUpSkill
// 调 clear；ctor 先构造内嵌 LevelUpSkill 再调 clear。sizeof 0x18
// （AvatarRouletteServer::InitAddCharacSkill 0x0817f386 读元素 +0xc/+0x10/+0x14
// 实证；旧「单 int」模型与该证据冲突，2026-08-24 修正）。
struct STAvatarAbilityVariation
{
    int m_field0;        // +0x00
    int m_field4;        // +0x04
    int m_field8;        // +0x08
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
    EquipmentParameterInfo& operator=(const EquipmentParameterInfo& other);

    void clear();
    void addString(std::string str);
    int& getEquipmentPhysicalAttack(int level);
    int& getEquipmentPhysicalDefense(int level);
    int& getEquipmentMagicalAttack(int level);
    int& getEquipmentMagicalDefense(int level);

    // ---- 嵌套结构 ----
    struct RebirthAppendageinfo
    {
        RebirthAppendageinfo();
        void clear();
        int m_field0;   // +0x00
        int m_field4;   // +0x04
        int m_field8;   // +0x08
    };

    struct EquipmentAuraAppendageData
    {
        int m_field0;   // +0x00
    };

    struct SASPROPERTY
    {
        SASPROPERTY();
        SASPROPERTY(const SASPROPERTY& other);
        ~SASPROPERTY();
        SASPROPERTY& operator=(const SASPROPERTY& other);
        // ORIG（C1 0x89c5046 / copy 0x83cb038 / D1 0x83db300 / assign 0x83dbdfc）：
        // 头部 0x20 字节按整块 memcpy，vector<float> @+0x20，string @+0x2c。
        int m_head[8];                  // +0x00..0x1c
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
    int m_28;                                          // +0x28（import 写 int；语义未确认）
    int m_2c;                                          // +0x2c（import 写 int；语义未确认）
    char m_pad30[0x20];                                // +0x30..0x4f（8 个标量 int，语义未知，clear 不重置）
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
    int m_8c;                                          // +0x8c（标量，clear 清零；语义未在 import/export 出现）
    char m_pad90[0x20];                                // +0x90..0xaf（8 个标量 int，clear 清零 0x94/0x98/0xa0/0xa4/0xac，语义未知）
    int m_fireResistance;                              // +0xb0 [fire resistance]
    int m_waterResistance;                             // +0xb4 [water resistance]
    int m_darkResistance;                              // +0xb8 [dark resistance]
    int m_lightResistance;                             // +0xbc [light resistance]
    int m_allElementalResistance;                      // +0xc0 [all elemental resistance]
    int m_slowResistance;                              // +0xc4 [slow resistance]
    int m_freezeResistance;                            // +0xc8 [freeze resistance]
    int m_poisonResistance;                            // +0xcc [poison resistance]
    int m_stunResistance;                              // +0xd0 [stun resistance]
    int m_curseResistance;                             // +0xd4 [curse resistance]
    int m_blindResistance;                             // +0xd8 [blind resistance]
    int m_lightningResistance;                         // +0xdc [lightning resistance]
    int m_stoneResistance;                             // +0xe0 [stone resistance]
    int m_sleepResistance;                             // +0xe4 [sleep resistance]
    int m_burnResistance;                              // +0xe8 [burn resistance]
    int m_weaponBreakResistance;                       // +0xec [weapon break resistance]
    int m_bleedingResistance;                          // +0xf0 [bleeding resistance]
    int m_piercingResistance;                          // +0xf4 [piercing resistance]
    int m_stuckResistance;                             // +0xf8 [stuck resistance]
    int m_confuseResistance;                           // +0xfc [confuse resistance]
    int m_holdResistance;                              // +0x100 [hold resistance]
    int m_armorBreakResistance;                        // +0x104 [armor break resistance]
    int m_allActiveStatusResistance;                   // +0x108 [all activestatus resistance]
    int m_inventoryLimit;                              // +0x10c [inventory limit]
    int m_hpRegenSpeed;                                // +0x110 [HP regen speed]
    int m_mpRegenSpeed;                                // +0x114 [MP regen speed]
    float m_mpRegenSpeedRate;                          // +0x118 [MP regen speed rate]
    int m_moveSpeed;                                   // +0x11c [move speed]
    float m_xMoveDashSpeed;                            // +0x120 [x move dash speed]
    float m_yMoveDashSpeed;                            // +0x124 [y move dash speed]
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
    char m_pad164[4];                                  // +0x164
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
    int m_210;                                         // +0x210（float 写入；clear 清零；语义未确认）
    std::vector<int> m_vec214;                         // +0x214（[hardcoding parameter] push_back）
    int m_pushAside;                                   // +0x220 [push aside]
    int m_liftUp;                                      // +0x224 [lift up]
    float m_stuck;                                     // +0x228 [stuck]
    bool m_ignoreDefense;                              // +0x22c [ignore defense]
    float m_humanDamageRate;                           // +0x230 [human damage rate]
    float m_fortDamageRate;                            // +0x234 [fort damage rate]
    float m_monsterDamageRate;                         // +0x238 [monster damage rate]
    bool m_hpDrain;                                    // +0x23c [HP drain]
    bool m_mpDrain;                                    // +0x23d [MP drain]
    std::vector<std::vector<float> > m_vec240;         // +0x240（[level linear ability] 表）
    std::vector<LevelUpSkill> m_levelUpSkills24c;      // +0x24c
    char m_pad258[0xc];                                // +0x258
    std::vector<STAvatarAbilityVariation> m_avatar264; // +0x264（[avatar select ability]）
    char m_pad270[4];                                  // +0x270
    float m_roomListMoveSpeedRate;                     // +0x274 [room list move speed rate]
    std::string m_str278;                              // +0x278
    std::string m_str27c;                              // +0x27c
    int m_antiEvil;                                    // +0x280 [anti evil]
    int m_rigidity;                                    // +0x284 [rigidity]
    int m_fireAttack;                                  // +0x288 [fire attack]
    int m_waterAttack;                                 // +0x28c [water attack]
    int m_darkAttack;                                  // +0x290 [dark attack]
    int m_lightAttack;                                 // +0x294 [light attack]
    int m_allElementalAttack;                          // +0x298 [all elemental attack]
    char m_pad29c[0xd8];                               // +0x29c..0x373（标量，语义未知）
    int m_activeStatusControlInfo[5];                  // +0x374..0x384 [active status control info]
    int m_388[5];                                      // +0x388（[separate attack] 0x388/0x38c + [separate rate] float 0x390/0x394）
    char m_pad39c[0x74];                               // +0x39c..0x40f（标量，语义未知）
    int m_410;                                         // +0x410
    int m_414;                                         // +0x414
    // +0x418..0x434 = ORIG `._119` 匿名结构（importAllSkillItem/importItemOverpowerDesc 写入）
    std::vector<SASPROPERTY> m_418;                    // +0x418（importAllSkillItem push_back）
    char m_424;                                        // +0x424（importItemOverpowerDesc 置 1）
    char m_425;                                        // +0x425（importItemOverpowerDesc 置 1）
    char m_pad426[2];                                  // +0x426..0x427
    std::vector<std::vector<float> > m_428;            // +0x428（importItemOverpowerDesc 写入）
    EquipmentParameterInfo* m_434;                     // +0x434（scanRandomOptionValue 传入的 EquipmentParameterInfo* 子指针）
    char m_pad438[0xc];                                // +0x438..0x443
    float m_upgradeCostDiscount;                       // +0x444 [upgrade cost discount]
    float m_448;                                       // +0x448 [upgrade cost discount] 第二值
    float m_assaultCostDiscount;                       // +0x44c [assault cost discount]
    int m_consumeItem;                                 // +0x450 [consume item]
    int m_upgradeProbIncrease;                         // +0x454 [upgrade prob increase]
    int m_hpRecovery;                                  // +0x458 [hp recovery]
    int m_cooltime;                                    // +0x45c [cooltime]
    int m_rebirth;                                     // +0x460 [rebirth]
    int m_mpRecovery;                                  // +0x464 [mp recovery]
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
    int m_28;
    int m_2c;
    char m_pad30[0x20];
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
    int m_8c;
    char m_pad90[0x20];
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
    char m_pad164[4];
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
    int m_210;
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
    char m_levelUpSkills24c[sizeof(std::vector<LevelUpSkill>)];
    char m_pad258[0xc];
    char m_avatar264[sizeof(std::vector<STAvatarAbilityVariation>)];
    char m_pad270[4];
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
    char m_pad29c[0xd8];
    int m_activeStatusControlInfo[5];
    int m_388[5];
    char m_pad39c[0x74];
    int m_410;
    int m_414;
    char m_pad418[sizeof(std::vector<EquipmentParameterInfo::SASPROPERTY>)];
    char m_424;
    char m_425;
    char m_pad426[2];
    char m_428[sizeof(std::vector<std::vector<float> >)];
    void* m_434;
    char m_pad438[0xc];
    float m_upgradeCostDiscount;
    float m_448;
    float m_assaultCostDiscount;
    int m_consumeItem;
    int m_upgradeProbIncrease;
    int m_hpRecovery;
    int m_cooltime;
    int m_rebirth;
    int m_mpRecovery;
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
