#ifndef GAME_STCHARACTERSCRIPT_H_
#define GAME_STCHARACTERSCRIPT_H_
#include <map>
#include <string>
#include <utility>
#include <vector>
class STRDCharacterStatusClient;  // 完整定义 STRDCharacterStatusClient.h
// ============================================================================
// df_game_r 还原 —— STCharacterScript 静态脚本表宿主类。
// ORIG nm（df_game_r）证据：
//   STCharacterScript::naExpTable  B 0x94fe960 [0x320]  = int[200]
//     （InitCharacterScript 0x08a0cc3c 以 ExpTable.tbl 逐项 ScanInt 回填）
//   STCharacterScript::pvpPrices_  B 0x94fec80 [0x320]  = int[200]
// ---- 武器信息（ORIG C1 W 0x08a3ce08 / clear W 0x08a3cd8e / D1 W 0x083dc13a /
//      copy W 0x08a3dfca；sizeof 0x34，clear 尾循环写至 +0x30 实证）----
class WeaponInfo
{
public:
    WeaponInfo();                        // vector 构造后 clear()
    ~WeaponInfo();                       // 平凡
    void clear();

    std::vector<std::string> m_wavs;     // +0x00（[weapon wav] 行集）
    int m_hitType;                       // +0x0c（blow/cut/burn/etc=0..3；clear=5）
    unsigned char m_blood;               // +0x10（[no blood]=0 / [blood]=1）
    int m_field14;                       // +0x14
    float m_rate18;                      // +0x18（clear=1.0f）
    float m_rate1c;                      // +0x1c
    float m_rate20;                      // +0x20
    float m_skillRates[4];               // +0x24..0x33（[weapon skill info]）
};  // sizeof == 0x34
// 本头只声明静态存储，定义在 InitScriptProducers_4.cpp。
// ============================================================================
class WeaponInfo;

// ---- 角色状态块（ORIG STRDCharacterStatus::Clear 0x08367642 逐字段实证；
//      块步长 0x57 → 1 字节 packed）----
#pragma pack(push, 1)
class STRDCharacterStatus
{
public:
    STRDCharacterStatus() { Clear(); }   // ORIG C1/C2 W 0x0836773e：调 Clear 清零
    void Clear();                        // ORIG _ZN19STRDCharacterStatus5ClearEv W 0x08367642
    // ORIG T 0x08a0c942：Client（0x98 展开块）→ 本 packed 块截断回填
    STRDCharacterStatus& operator=(const STRDCharacterStatusClient&);

    int m_hpMax;                         // +0x00
    int m_mpMax;                         // +0x04
    unsigned short m_physAttack;         // +0x08
    unsigned short m_physDefense;        // +0x0a
    unsigned short m_magAttack;          // +0x0c
    unsigned short m_magDefense;         // +0x0e
    unsigned short m_elemResist[4];      // +0x10 fire/water/dark/light
    unsigned short m_activeResist[17];   // +0x18 ACTIVESTATUS_TAG 顺序
    int m_inventoryLimit;                // +0x3a
    unsigned short m_hpRegen;            // +0x3e
    unsigned short m_mpRegen;            // +0x40
    int m_moveSpeed;                     // +0x42
    unsigned short m_attackSpeed;        // +0x46
    unsigned short m_castSpeed;          // +0x48
    unsigned short m_hitRecovery;        // +0x4a
    unsigned short m_jumpPower;          // +0x4c
    int m_weight;                        // +0x4e
    unsigned char m_field52;
    int m_field53;
};  // sizeof == 0x57（packed）
#pragma pack(pop)

// ---- 角色附加统计（ORIG CharacterEtcStats::clear 0x08367752；步长 0x1c）----
class CharacterEtcStats
{
public:
    CharacterEtcStats();                 // ORIG C1Ev W 0x0836778c（vector C1 + clear）
    void clear();                        // ORIG _ZN17CharacterEtcStats5clearEv W 0x08367752

    std::vector<int> m_category;         // +0x00（[category]）
    float m_moduleDamageRate[4];         // +0x0c（[module damage rate]）
};  // sizeof == 0x1c（vector 0xc + 4×float 0x10）

// ---- 动作文件集（+0xb5c..+0xb9b；ORIG ActiveActionFileSetC1 纯逐成员构造，
//      ImportCharacterScript 的 [xxx action]/[attack action]/[etc action] 直写成员）----
class ActiveActionFileSet
{
public:
    std::string m_waiting;               // +0x00（[waiting action]）
    std::string m_move;
    std::string m_dash;
    std::string m_sit;
    std::string m_damage1;
    std::string m_damage2;
    std::string m_down;
    std::string m_overturn;
    std::string m_jump;
    std::string m_jumpattack;            // +0x24
    std::vector<std::string> m_attackList;  // +0x28（[attack action]/[action list]）
    std::vector<std::string> m_etc;         // +0x34（[etc action]）
};  // sizeof == 0x40

// ---- 连击信息（ORIG _ComboInfo::clear 0x0836785c）----
class _ComboInfo
{
public:
    void clear();                        // ORIG _ZN10_ComboInfo5clearEv W 0x0836785c

    std::map<int, int> m_mapA;           // +0x00
    std::map<int, int> m_mapB;           // +0x18
    std::vector<int> m_vec;              // +0x30
};  // sizeof == 0x3c

class STCharacterScript
{
public:
    static int naExpTable[0xc8];    // ORIG 0x94fe960（200 × int）
    static int pvpPrices_[0xc8];    // ORIG 0x94fec80（200 × int）

    // ---- 二转觉醒域（ORIG STSecondGrowTypeData::clear 0x08367a28；尺寸 0x100）----
    class STSecondGrowTypeData
    {
    public:
        void clear();                    // ORIG W 0x08367a28

        STRDCharacterStatus m_statusA;                  // +0x00（awakening 1）
        STRDCharacterStatus m_statusB;                  // +0x57（awakening 2）
        char m_padae[2];
        std::vector<std::pair<int, int> > m_skills[2];  // +0xb0
        CharacterEtcStats m_etc[2];                     // +0xc8
    };  // sizeof == 0x100

    // 定义在 STCharacterLayoutC3.cpp（ORIG 为 W 内联符号，重建侧按值构造仅经
    // asm 桥，故落盘为普通外链定义；头文件保持纯声明防 ODR）：
    STCharacterScript();                 // ORIG C1/C2 W 0x08368348：逐成员构造后调 Clear()
    ~STCharacterScript();                // ORIG D1/D2 W 0x08376362：逆序逐成员析构

    void Clear();                        // ORIG _ZN17STCharacterScript5ClearEv W 0x08367a98

    // ---- 实例布局（ImportCharacterScript 0x08a0ce06 / C1 / Clear 汇编实证）----
    int m_job;                                        // +0x000（[job]）
    std::string m_growtypeName[6];                    // +0x004（[growtype name]）
    int m_width;                                      // +0x01c
    int m_height;                                     // +0x020
    STRDCharacterStatus m_initialStatus;              // +0x024（[initial value]，步长 0x57）
    STRDCharacterStatus m_growStatus[6];              // +0x07b（[growtype 1..6]）→ 止于 0x285
    std::vector<std::pair<int, int> > m_skillList[7]; // +0x288（[0]=initial，[1..6]=growtype N）
    CharacterEtcStats m_etcStats[7];                  // +0x2dc（[category]/[module damage rate]）
    std::string m_awakeningName[6][2];                // +0x3a0 → 止于 0x3d0
    STSecondGrowTypeData m_secondGrow[7];             // +0x3d0（各 0x100）→ 止于 0xad0
    int m_jumpSpeed;                                  // +0xad0（Clear=100）
    std::string m_motionWaiting;                      // +0xad4
    std::string m_motionMove;                         // +0xad8
    std::string m_motionSit;                          // +0xadc
    std::string m_motionDamage1;                      // +0xae0
    std::string m_motionDamage2;                      // +0xae4
    std::string m_motionDown;                         // +0xae8
    std::string m_motionOverturn;                     // +0xaec
    std::string m_motionJump;                         // +0xaf0
    std::string m_motionJumpattack;                   // +0xaf4
    std::string m_motionRest;                         // +0xaf8
    std::string m_motionSimpleRest;                   // +0xafc
    std::string m_motionSimpleMove;                   // +0xb00
    std::string m_throwMotion11;                      // +0xb04
    std::string m_throwMotion21;                      // +0xb08
    std::string m_throwMotion31;                      // +0xb0c
    std::string m_throwMotion41;                      // +0xb10
    std::string m_throwMotion12;                      // +0xb14
    std::string m_throwMotion22;                      // +0xb18
    std::string m_throwMotion32;                      // +0xb1c
    std::string m_throwMotion42;                      // +0xb20
    std::string m_motionDash;                         // +0xb24
    std::string m_motionDashattack;                   // +0xb28
    std::string m_motionGetitem;                      // +0xb2c
    std::string m_motionBuff;                         // +0xb30
    std::string m_motionBack;                         // +0xb34
    std::vector<std::string> m_ghostMotion;           // +0xb38
    std::vector<std::string> m_attackMotion;          // +0xb44
    std::vector<std::string> m_etcMotion;             // +0xb50
    ActiveActionFileSet m_actionSet;                  // +0xb5c（0x40）→ 止于 0xb9c
    std::string m_lastAction;                         // +0xb9c
    std::string m_procAction;                         // +0xba0
    std::string m_jumpattackInfo;                     // +0xba4
    std::string m_dashattackInfo;                     // +0xba8
    std::vector<std::string> m_attackInfo;            // +0xbac
    std::vector<std::string> m_etcAttackInfo;         // +0xbb8
    std::vector<WeaponInfo> m_weaponWav;              // +0xbc4
    std::string m_scriptPath;                         // +0xbd0
    std::string m_scriptName;                         // +0xbdc
    int m_fieldbd4;                                   // +0xbd4（Clear=0；语义「推断」未建模）
    float m_fieldbd8;                                 // +0xbd8（Clear=1.0f；「推断」）
    _ComboInfo m_combo;                               // +0xbe0 → sizeof 0xc1c
};

static_assert(sizeof(STRDCharacterStatus) == 0x57, "STRDCharacterStatus size drift");
static_assert(sizeof(CharacterEtcStats) == 0x1c, "CharacterEtcStats size drift");
static_assert(sizeof(ActiveActionFileSet) == 0x40, "ActiveActionFileSet size drift");
static_assert(sizeof(_ComboInfo) == 0x3c, "_ComboInfo size drift");
static_assert(sizeof(STCharacterScript::STSecondGrowTypeData) == 0x100,
              "STSecondGrowTypeData size drift");
static_assert(sizeof(STCharacterScript) == 0xc1c, "STCharacterScript size drift");

#endif  // GAME_STCHARACTERSCRIPT_H_
