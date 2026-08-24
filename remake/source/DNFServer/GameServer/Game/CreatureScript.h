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

// ENUM_ELEMENT（AttackInfo set 元素，getElement 0..4）。
enum ENUM_ELEMENT
{
    ENUM_ELEMENT_FIRE = 0,
    ENUM_ELEMENT_WATER = 1,
    ENUM_ELEMENT_DARK = 2,
    ENUM_ELEMENT_LIGHT = 3,
    ENUM_ELEMENT_NONE = 4,
};

// 模块名表（MODULE_NAME_TAG，processModuleTagSet 使用；+8 步长 = 开/闭两 string）。
extern std::string g_moduleNameTag[0x20];

// ---- 原生 CreatureScript 脚本结构（importCreatureScript 填充） ----
struct st_crature_id_item_id
{
    int m_creatureId;   // +0x00
    int m_itemId;       // +0x04
};

struct CreatureAdditionalAnimation
{
    std::string m_name;   // +0x00
    int m_field4;         // +0x04
    int m_field8;         // +0x08
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
    std::string m_str0;   // +0x00
    std::string m_str4;   // +0x04
    int m_field8;         // +0x08
    int m_fieldc;         // +0x0c
    std::string m_str10;  // +0x10
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
    std::string m_flyMotion;        // +0x7c
    std::string m_getExpMotion;     // +0x84
    std::string m_getExpOverMotion; // +0x88
    std::string m_getExpUnderMotion;// +0x8c
    std::string m_skillMotion;      // +0x90
    std::string m_overSkillMotion;  // +0x94
    std::string m_responseMotion;   // +0x98
    std::string m_specialMotion;    // +0x9c
    std::string m_equipMotion;      // +0xa0
    std::string m_equipOverMotion;  // +0xa4
    std::string m_equipUnderMotion; // +0xa8
    std::string m_basicOverEffect;  // +0xac
    std::string m_walkOverEffect;   // +0xb0
    std::string m_runOverEffect;    // +0xb4
    std::string m_skillOverEffect;  // +0xb8
    std::string m_overSkillOverEffect;   // +0xbc
    std::string m_responseOverEffect;    // +0xc0
    std::string m_specialOverEffect;     // +0xc4
    std::string m_basicUnderEffect;      // +0xc8
    std::string m_walkUnderEffect;       // +0xcc
    std::string m_runUnderEffect;        // +0xd0
    std::string m_skillUnderEffect;      // +0xd4
    std::string m_overSkillUnderEffect;  // +0xd8
    std::string m_responseUnderEffect;   // +0xdc
    std::string m_specialUnderEffect;    // +0xe0
    std::string m_defaultUnderEffect;    // +0xe4
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
    selectiveGrowCreatureInfo m_selectiveGrow;  // +0x18c
    char m_pad1a4[0x1a8 - 0x1a4];            // +0x1a4
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
    // ORIG 静态成员：生物经验表（B _ZN14CreatureScript9ExpTable_E
    // 0x094fefa0，readelf size 396 = 99*4）。initCreatureScript（ORIG
    // 0x08a35d37）以 ScanInt 循环 i=0..0x62 填充；消费方
    // CDataManager::get_creature_level_exp（level-2 下标）。定义于
    // InitScriptProducers_5.cpp。
    static int ExpTable_E[0x63];

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
    int getAttackType(std::string& tag);

    float m_damage;                    // +0x00
    float m_damageIncreaseRate;        // +0x04
    int m_absoluteDamage;              // +0x08
    int m_damageBonus;                 // +0x0c
    int m_attackType;                  // +0x10
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
    int m_field80;                     // +0x80
    float m_field84;                   // +0x84
    unsigned char m_isHitNumberIncreasing;  // +0x88
    char m_pad89[0x8c - 0x89];         // +0x89
    int m_moduleTag;                   // +0x8c
    unsigned char m_ignoreDiehard;     // +0x90
    unsigned char m_hitCheckByScreenPos;  // +0x91
    char m_pad92[0x94 - 0x92];         // +0x92
    int m_maxTargetHitNumber;          // +0x94
    unsigned char m_attackMyself;      // +0x98
    unsigned char m_attackFriend;      // +0x9c
    unsigned char m_attackEnemy;       // +0xa0
    char m_pada1[0xa4 - 0xa1];         // +0xa1
    std::set<ENUM_ELEMENT> m_elements;   // +0xa4
    // 3 个活动状态组（+0xf8/0x104/0x110 各 12B vector<int>）+ 标量区
    std::vector<int> m_activeStatusVectors[3];   // +0xf8, +0x104, +0x110
    int m_activeStatusType[3];       // +0xbc
    float m_activeStatusValue[3];    // +0xc8
    int m_activeStatusInt[3];        // +0xd4
    int m_activeStatusInt2[3];       // +0xe0
    int m_activeStatusInt3[3];       // +0xec
    float m_damageRate[3];           // +0x11c, +0x120, +0x124
    std::vector<CatchObjectInfo> m_catchObjects;  // +0x128
};

// ---- 全局脚本文件列表 / 目录 ----
// ORIG g_creatureScriptFileList 是 STScriptFileList（+0x00 即
// std::map<int,std::string>）；LoadCreatureScriptFile / importCreatureScript
// 均按 map 语义访问，故此处直接声明为 std::map<int,std::string>（0x18B，
// 与 STScriptFileList 布局一致），避免跨头 class/struct 声明冲突。
extern std::map<int, std::string> g_creatureScriptFileList;
extern char g_creatureScriptBaseDirectory[0x104];

// ---- 解析函数（ORIG T）----
bool importCreatureScript(CreatureScript* script, const char* path);
bool ImportAttackInfoScript(AttackInfo* info, const char* path, int param3);
bool InitAttackInfoScript(AttackInfo* info, const char* path, int param3);

// ---- 辅助自由函数 ----
int getElement(const char* s);                       // 0x88bfe5b
const char* getElementName(int element);             // 0x88bfee0
int getActiveStatusType(const std::string& s);       // 0x88944fb
int getDrawLayer(const char* s);                     // 0x88be12d
void getDirectoryPath(std::string& out, const char* fmt, ...);  // NpcScript.h
bool isValidAttackInfoElement(const void* sdc, int element);  // 0x809576e

#endif  // DNF_CREATURESCRIPT_H
