// ============================================================================
// df_game_r 还原 —— native 宠物/攻击脚本类型（RDARScriptObject.cpp /
// RDARScriptAttackInfo.cpp 域）。
//   本头为权威声明：user_creature::CCreatureScript 是 CCreatureScriptMgr 持有的
//   runtime 包装类；全局 CreatureScript / AttackInfo 是 importCreatureScript /
//   ImportAttackInfoScript 解析的目标结构。全部偏移依据
//   docs/class_func_reports/user_creature__CCreatureScript/* 与
//   _lt_global_gt_/importCreatureScript.md / ImportAttackInfoScript.md。
// ============================================================================
#ifndef DNF_CREATURESCRIPT_H
#define DNF_CREATURESCRIPT_H

#include <map>
#include <set>
#include <string>
#include <vector>

#include "NpcScript.h"   // STScriptFileList（g_creatureScriptFileList 真型）

// ENUM_ELEMENT（AttackInfo set 元素，getElement 0..4）。
enum ENUM_ELEMENT
{
    ENUM_ELEMENT_FIRE = 0,
    ENUM_ELEMENT_WATER = 1,
    ENUM_ELEMENT_DARK = 2,
    ENUM_ELEMENT_LIGHT = 3,
    ENUM_ELEMENT_NONE = 4,
};

// 模块名表（MODULE_NAME_TAG，processModuleTagSet 使用；+8 步长 = 开/闭两
// string 对）。2D 形（batch-12，Main 批准）：ORIG 0x08a97637 切片 `2*i` 寻址
// 折 `shl $3`（stride 8 = pair）+ 同域局部表 [5][2] 438=438 闭合形实证。
extern std::string g_moduleNameTag[17][2];

// ---- 原生 CreatureScript 脚本结构（importCreatureScript 填充） ----
struct st_crature_id_item_id
{
    int m_creatureId;   // +0x00
    int m_itemId;       // +0x04
};

struct CreatureAdditionalAnimation
{
    // ORIG C1EiiPKc 0x08a3dcc2：+0x08 构造 string，this[0]=a、this[4]=b → 布局
    // int@0 / int@4 / string@8（2026-08-29 OnlyOrigBatch3 修正原反序建模）。
    CreatureAdditionalAnimation(int a, int b, const char* name);  // ORIG W 0x08a3dcc2
    int m_field0;         // +0x00
    int m_field4;         // +0x04
    std::string m_name;   // +0x08
};

struct selectiveGrowCreatureInfo
{
    selectiveGrowCreatureInfo();
    void clear();
    unsigned char m_usingExpItem;   // +0x00（grow 判定）
    char m_pad1[3];
    int m_growLevel;                // +0x04
    int m_growATypeCreature;        // +0x08
    int m_growBTypeCreature;        // +0x0c
    int m_growATypeItem;            // +0x10
    int m_growBTypeItem;            // +0x14
    int m_growItemCount;            // +0x18
};

struct CreatureScriptSkillInfo
{
    void clear();
    // ORIG C1 0x8a3dd26：7 string 构造后调用 clear()（m_fieldc=1/m_field10=-1 等）
    CreatureScriptSkillInfo() { clear(); }
    std::string m_str0;   // +0x00
    std::string m_str4;   // +0x04
    int m_field8;         // +0x08
    int m_fieldc;         // +0x0c
    int m_field10;        // +0x10（ORIG clear 0x833e000 写 -1，非 string）
    std::string m_str14;  // +0x14
    std::string m_str18;  // +0x18
    std::string m_str1c;  // +0x1c
    std::string m_str20;  // +0x20
    std::string m_str24;  // +0x24
};

// ORIG CreatureScript（全局，native 解析目标）。布局见 importCreatureScript.md。
class CreatureScript
{
public:
    CreatureScript();
    ~CreatureScript();
    void clear();

    int m_id;                       // +0x00（import 末尾 FindIndexByFullScan 写回）
    std::string m_name;             // +0x04
    int m_field8;                   // +0x08
    int m_fieldc;                   // +0x0c
    int m_field10;                  // +0x10
    int m_layer;                    // +0x14
    int m_field18;                  // +0x18
    int m_field1c;                  // +0x1c
    int m_field20;                  // +0x20
    unsigned char m_directionCorrect;  // +0x24
    char m_pad25[0x28 - 0x25];      // +0x25
    int m_moveSpeed[5];             // +0x28..+0x38
    int m_gravity;                  // +0x3c
    int m_startLevel;               // +0x40
    int m_permissionLevel;          // +0x44
    int m_maxLevel;                 // +0x48
    int m_parentMaxLevel;           // +0x4c
    int m_artifactSlotRed;          // +0x50
    int m_artifactSlotBlue;         // +0x54
    int m_artifactSlotGreen;        // +0x58
    int m_learnOverSkillLevel;      // +0x5c
    int m_skillMP;                  // +0x60
    int m_overSkillMP;              // +0x64
    int m_skillType;                // +0x68
    int m_overSkillType;            // +0x6c
    std::string m_basicMotion;      // +0x70
    std::string m_walkMotion;       // +0x74
    std::string m_runMotion;        // +0x78
    std::string m_flyMotion;        // +0x7c（ORIG clear 0x833dfac 不复位本槽）
    std::string m_getExpMotion;     // +0x80（dtor 0x8340ca4/clear/import 三处
                                    //  `sub $0xffffff80,%eax` 寻址实证为 string，
                                    //  2026-09-04 修正原误判空洞 pad80）
    std::string m_getExpOverMotion; // +0x84
    std::string m_getExpUnderMotion;// +0x88
    std::string m_skillMotion;      // +0x8c（import "[skill motion]"）
    std::string m_overSkillMotion;  // +0x90
    std::string m_responseMotion;   // +0x94
    std::string m_specialMotion;    // +0x98
    std::string m_equipMotion;      // +0x9c
    std::string m_equipOverMotion;  // +0xa0
    std::string m_equipUnderMotion; // +0xa4（import "[equip under motion]"）
    std::string m_basicOverEffect;  // +0xa8（import "[basic over effect]"）
    std::string m_walkOverEffect;   // +0xac
    std::string m_runOverEffect;    // +0xb0
    std::string m_defaultOverEffect;   // +0xb4（推断：仅 dtor 0x8340ca4 销毁，
                                       //  clear/import 均不触碰，命名对齐 under 侧）
    std::string m_skillOverEffect;  // +0xb8
    std::string m_overSkillOverEffect;   // +0xbc
    std::string m_responseOverEffect;    // +0xc0
    std::string m_specialOverEffect;     // +0xc4
    std::string m_basicUnderEffect;      // +0xc8（import "[basic under effect]"）
    std::string m_walkUnderEffect;       // +0xcc
    std::string m_runUnderEffect;        // +0xd0
    std::string m_defaultUnderEffect;    // +0xd4（推断：仅 dtor 销毁，
                                         //  clear/import 不触碰）
    std::string m_skillUnderEffect;      // +0xd8（import "[skill under effect]"）
    std::string m_overSkillUnderEffect;  // +0xdc
    std::string m_responseUnderEffect;   // +0xe0
    std::string m_specialUnderEffect;    // +0xe4（import "[special under effect]"）
    std::vector<CreatureAdditionalAnimation> m_additionalAnimations;  // +0xe8
    std::string m_defaultParticle;  // +0xf4
    std::string m_responseSoundTag; // +0xf8
    std::string m_wingSoundTag;     // +0xfc
    std::string m_walkSoundTag;     // +0x100
    std::string m_runSoundTag;      // +0x104
    int m_revisionX;                // +0x108
    int m_revisionY;                // +0x10c
    std::vector<std::string> m_etcMotion;    // +0x110
    std::vector<std::string> m_attackInfo;   // +0x11c
    std::vector<std::string> m_skillString;  // +0x128
    std::vector<int> m_skillLevelValues;     // +0x134
    std::vector<int> m_overSkillLevelValues; // +0x140
    std::vector<int> m_piercing;             // +0x14c
    std::vector<int> m_intData;              // +0x158
    std::vector<std::string> m_stringData;   // +0x164
    std::string m_directory;                 // +0x170
    int m_evolutionQuestId;                  // +0x174
    int m_evolutionCreatureId;               // +0x178
    int m_evolutionLevel;                    // +0x17c
    std::vector<int> m_evolutionMultyId;     // +0x180
    selectiveGrowCreatureInfo m_selectiveGrow;  // +0x18c（size 0x1c → 0x1a8）
    std::vector<std::string> m_randomMotion;  // +0x1a8
    int m_defaultRandomMotionRate;            // +0x1b4
    int m_increaseRandomMotionRate;           // +0x1b8
    int m_itemEvolution;                      // +0x1bc
    std::vector<st_crature_id_item_id> m_itemEvolCreatureIds;  // +0x1c0
    std::string m_skillInfo;                  // +0x1cc
    std::string m_overSkillInfo;              // +0x1d0
    std::map<int, CreatureScriptSkillInfo> m_skillList;      // +0x1d4
    std::map<int, CreatureScriptSkillInfo> m_overSkillList;  // +0x1ec
    unsigned char m_usingRandomSkill;         // +0x204
    char m_pad205[0x208 - 0x205];             // +0x205
    std::vector<int> m_eventEvlouteCreature;  // +0x208
    // ORIG 静态成员：生物经验表（B _ZN14CreatureScript9ExpTable_E，成员名含尾下划线）
    // 0x094fefa0，readelf size 396 = 99*4）。initCreatureScript（ORIG
    // 0x08a35d37）以 ScanInt 循环 i=0..0x62 填充；消费方
    // CDataManager::get_creature_level_exp（level-2 下标）。定义于
    // InitScriptProducers_5.cpp。
    static int ExpTable_[0x63];

};

// ---- AttackInfo（ImportAttackInfoScript 解析目标）----
struct CatchObjectInfo
{
    int m_field0;   // +0x00
    int m_field4;   // +0x04
    float m_field8; // +0x08
};

class AttackInfo
{
public:
    enum ENUM_ATTACKTYPE
    {
        ATTACKTYPE_PHYSICAL = 0,
        ATTACKTYPE_MAGICAL = 1,
        ATTACKTYPE_ABSOLUTE = 2,
    };

    AttackInfo();
    ~AttackInfo();
    void clear();
    bool isIntendDamage();
    void setElement(ENUM_ELEMENT element);
    bool isValidElement(ENUM_ELEMENT element);
    void setAttackType(ENUM_ATTACKTYPE type);
    static int getAttackType(std::string& tag);

    float m_damage;                    // +0x00
    float m_damageIncreaseRate;        // +0x04
    int m_absoluteDamage;              // +0x08
    int m_damageBonus;                 // +0x0c
    ENUM_ATTACKTYPE m_attackType;      // +0x10（N76 反形同型化：与 setAttackType
                                       // 形参同型 → 4.4 LHS 地址先载 = ORIG 0x88908b8 形；
                                       // 偏移/sizeof 不变）
    int m_weaponDamageApply;           // +0x14
    float m_humanActiveStatusRate;     // +0x18
    int m_damageReaction;              // +0x1c
    int m_pushAside;                   // +0x20
    int m_liftUp;                      // +0x24
    float m_criticalHit;               // +0x28
    float m_stuck;                     // +0x2c
    int m_stuckProbIncrease;           // +0x30
    int m_knuckBack;                   // +0x34
    int m_knuckBackAngle;              // +0x38
    float m_weakness;                  // +0x3c
    float m_weakness2;                 // +0x40
    int m_bounce;                      // +0x44
    int m_ignoreDefense;               // +0x48
    std::string m_hitWav;              // +0x4c
    int m_attackDirection;             // +0x50
    int m_hitInfo;                     // +0x54
    unsigned char m_blood;             // +0x58
    char m_pad59[0x5c - 0x59];         // +0x59
    int m_field5c;                     // +0x5c
    float m_field60;                   // +0x60
    unsigned char m_isUseWeaponHitDelayRate;  // +0x64
    char m_pad65[0x68 - 0x65];         // +0x65
    int m_hitDirection;                // +0x68（auto/front/back/outer/inner）
    int m_ignoreSuperArmor;            // +0x6c
    int m_ignoreWeight;                // +0x70
    int m_forceHitStunTime;            // +0x74
    int m_hpDrain;                     // +0x78
    int m_mpDrain;                     // +0x7c
    float m_field80;                   // +0x80（ORIG clear mov $0x0,%edx 中转 → float）
    float m_field84;                   // +0x84
    unsigned char m_isHitNumberIncreasing;  // +0x88
    char m_pad89[0x8c - 0x89];         // +0x89
    int m_moduleTag;                   // +0x8c
    unsigned char m_ignoreDiehard;     // +0x90
    unsigned char m_hitCheckByScreenPos;  // +0x91
    char m_pad92[0x94 - 0x92];         // +0x92
    int m_maxTargetHitNumber;          // +0x94
    int m_attackFlags[3];              // +0x98（[0]=myself [1]=friend [2]=enemy；
                                       //  ORIG clear 循环 0x98+i*4=(i==2) 实证）
    std::set<ENUM_ELEMENT> m_elements;   // +0xa4（0x18）
    // 布局修正（Layout_D 2026-08-30）：ORIG C1 0x088903c3 实证 vector 数组在
    // 0xf8（set+0x54），标量区 0xbc..0xf8 在前——按 ORIG 声明序排列。
    int m_activeStatusType[3];       // +0xbc
    float m_activeStatusValue[3];    // +0xc8
    int m_activeStatusInt[3];        // +0xd4
    int m_activeStatusInt2[3];       // +0xe0
    int m_activeStatusInt3[3];       // +0xec
    std::vector<int> m_activeStatusVectors[3];   // +0xf8, +0x104, +0x110
    float m_damageRate[3];           // +0x11c, +0x120, +0x124
    std::vector<CatchObjectInfo> m_catchObjects;  // +0x128
};

// ---- 全局脚本文件列表 / 目录 ----
// ORIG g_creatureScriptFileList 是 STScriptFileList（+0x00 即
// std::map<int,std::string>）；LoadCreatureScriptFile / importCreatureScript
// 均按 map 语义访问，故此处直接声明为 std::map<int,std::string>（0x18B，
// 与 STScriptFileList 布局一致），避免跨头 class/struct 声明冲突。
extern STScriptFileList g_creatureScriptFileList;   // b28d-Main：ORIG 真型 STScriptFileList（+0x00 即 map<int,string>；探针 66/66）
extern char g_creatureScriptBaseDirectory[0x104];

// ---- 解析函数（ORIG T）----
bool importCreatureScript(CreatureScript* script, const char* path);
bool ImportAttackInfoScript(AttackInfo* info, const char* path, int param3);
bool InitAttackInfoScript();                                   // ORIG 0x88d1450（无参）
bool InitAttackInfoScript(AttackInfo* info, const char* path, int param3);  // 既有转发契约

// ---- 辅助自由函数 ----
int getElement(const char* s);                       // 0x88bfe5b
const char* getElementName(ENUM_ELEMENT element);    // 0x88bfee0
int getActiveStatusType(const std::string& s);       // 0x88944fb
int getDrawLayer(const char* s);                     // 0x88be12d
void getDirectoryPath(std::string& out, const char* fmt, ...);  // NpcScript.h
bool isValidAttackInfoElement(const void* sdc, int element);  // 0x809576e

#endif  // DNF_CREATURESCRIPT_H
