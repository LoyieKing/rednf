#ifndef GAME_STCHARACTERSCRIPT_H_
#define GAME_STCHARACTERSCRIPT_H_
#include <cstddef>
#include <map>
#include <string>
#include <utility>
#include <vector>
class STRDCharacterStatusClient;  // 完整定义 STRDCharacterStatusClient.h
// [bigfix-enum] ENUM_CHARACTERJOB 以既有 guard 块内联（与 JobKey.h /
// STEquipmentScript.h 同 guard 同成员表，幂等）。证据：ORIG ImportCharacterScript
// 0x08a0f951/0x08a0f970/0x08a0fa1f 读 m_job 发 `mov (%eax),%eax; mov %eax,%edx`
// （P254：4.1 对 enum 字段保留 eax→edx 中转，int 字段 4.4 恒折单段）。
#ifndef DNF_ENUM_CHARACTERJOB_DEFINED
#define DNF_ENUM_CHARACTERJOB_DEFINED
enum ENUM_CHARACTERJOB
{
    ENUM_CHARACTERJOB_0 = 0
};
#endif
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
    // [b28d-solo-ScriptDepsA] P317：ORIG D1 W 0x083dc13a 为编译器隐式合成
    //（成员逆序析构）；删用户空析构后 D1/D2 仍由使用点发射同名 W 符号。
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

    // [bigfix-unsigned] T2 直证（ORIG exportCharacterStatusScript 0x08a0fb41/
    // 0x08a0fb89/0x08a0feaa）：+0x0/+0x4/+0x42 的 `(double)field` 转换发
    // `mov $0x0,%edx; … fildll`（零扩展 ⇒ unsigned int），而 +0x3a/+0x4e 发
    // `fildl`（int）。同宽 4B，sizeof/偏移/mangled 不变。
    unsigned int m_hpMax;                // +0x00
    unsigned int m_mpMax;                // +0x04
    unsigned short m_physAttack;         // +0x08
    unsigned short m_physDefense;        // +0x0a
    unsigned short m_magAttack;          // +0x0c
    unsigned short m_magDefense;         // +0x0e
    unsigned short m_elemResist[4];      // +0x10 fire/water/dark/light
    unsigned short m_activeResist[17];   // +0x18 ACTIVESTATUS_TAG 顺序
    int m_inventoryLimit;                // +0x3a
    unsigned short m_hpRegen;            // +0x3e
    unsigned short m_mpRegen;            // +0x40
    unsigned int m_moveSpeed;            // +0x42（同上，fildll 零扩展直证）
    unsigned short m_attackSpeed;        // +0x46
    unsigned short m_castSpeed;          // +0x48
    unsigned short m_hitRecovery;        // +0x4a
    unsigned short m_jumpPower;          // +0x4c
    int m_weight;                        // +0x4e
    unsigned char m_field52;
    // b28c-28：真实类型 float（P79 判据——ORIG 脚本导入对该槽发 `fstps 0x5f`，
    // 见 CharacterAIA2.cpp:1265 实证；ORIG Clear 尾 `mov $0x0,%edx;
    // mov %edx,0x53(%eax)` 与探针 /tmp/l28_probe5.cpp gE 的 float=0.0f 形逐条一致）。
    float m_field53;
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
    // ORIG AFS C1 W 0x08367eee：+0x10/+0x14 为 2 元素 string 数组
    // （构造=倒计数循环 mov $0x1,%ebx;…cmp $-1，P151；同域 D1 亦数组循环）。
    std::string m_damage[2];             // +0x10（[damage action 1]/[2]）
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
    _ComboInfo();                        // ORIG C1（W）调 clear()
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
        // ORIG C1 W 0x08367a28 域：构造后调 clear()（138 条；NEW 隐式 C1
        // 仅 111 条不调 clear）。
        STSecondGrowTypeData() { clear(); }
        void clear();                    // ORIG W 0x08367a28

        // [round1 L26 数组化] ORIG clear 0x08367a28 `imul $0x57; add 0x8(%ebp),%eax`
        // = 数组寻址 ⇒ awakening 1/2 为 stride 0x57 的 2 元素数组；原显式
        // m_padae[2] 由自然对齐填充替代（sizeof/偏移零变化）。
        STRDCharacterStatus m_statusA[2];               // +0x00（awakening 1/2）
        std::vector<std::pair<int, int> > m_skills[2];  // +0xb0
        CharacterEtcStats m_etc[2];                     // +0xc8
    };  // sizeof == 0x100

    // 定义在 STCharacterLayoutC3.cpp（ORIG 为 W 内联符号，重建侧按值构造仅经
    // asm 桥，故落盘为普通外链定义；头文件保持纯声明防 ODR）：
    STCharacterScript();                 // ORIG C1/C2 W 0x08368348：逐成员构造后调 Clear()
    ~STCharacterScript();                // ORIG D1/D2 W 0x08376362：逆序逐成员析构

    void Clear();                        // ORIG _ZN17STCharacterScript5ClearEv W 0x08367a98

    // ---- 实例布局（ImportCharacterScript 0x08a0ce06 / C1 / Clear 汇编实证）----
    ENUM_CHARACTERJOB m_job;                          // +0x000（[job]；enum 见上）
    std::string m_growtypeName[6];                    // +0x004（[growtype name]）
    int m_width;                                      // +0x01c
    int m_height;                                     // +0x020
    // [round1 L26 数组化] ORIG 原形即单一数组：Clear 0x08367b40 `imul $0x57;
    // add $0x20; add this; add $4` = 「数组元素 + 成员偏移」，且 initial@+0x24 与
    // growtype1@+0x7b 步长同为 0x57 首尾相接 ⇒ 原码 `STRDCharacterStatus m_status[7]`。
    STRDCharacterStatus m_status[7];                  // +0x024（[0]=initial，[1..6]=growtype 1..6）→ 止于 0x285
    std::vector<std::pair<int, int> > m_skillList[7]; // +0x288（[0]=initial，[1..6]=growtype N）
    CharacterEtcStats m_etcStats[7];                  // +0x2dc（[category]/[module damage rate]）
    std::string m_awakeningName[6][2];                // +0x3a0 → 止于 0x3d0
    STSecondGrowTypeData m_secondGrow[7];             // +0x3d0（各 0x100）→ 止于 0xad0
    int m_jumpSpeed;                                  // +0xad0（Clear=100）
    std::string m_motionWaiting;                      // +0xad4
    std::string m_motionMove;                         // +0xad8
    std::string m_motionSit;                          // +0xadc
    // [b28d-solo-ScriptDepsA] ORIG Clear 0x08367c0c 段：`add $0x2b8; shl $2`
    // 的 i<=1 循环 ⇒ 原码是数组成员（2 元素），非两个独立 string。
    std::string m_motionDamage[2];                    // +0xae0（[damage motion 1/2]）
    std::string m_motionDown;                         // +0xae8
    std::string m_motionOverturn;                     // +0xaec
    std::string m_motionJump;                         // +0xaf0
    std::string m_motionJumpattack;                   // +0xaf4
    std::string m_motionRest;                         // +0xaf8
    std::string m_motionSimpleRest;                   // +0xafc
    std::string m_motionSimpleMove;                   // +0xb00
    // ORIG Clear 0x08367cba 段：同一 i<=3 循环内两处 `(i+0x2c0/0x2c4)*4+4`
    // ⇒ 两个 4 元素数组成员（throw N-1 / throw N-2）。
    std::string m_throwMotion1[4];                    // +0xb04（1-1,2-1,3-1,4-1）
    std::string m_throwMotion2[4];                    // +0xb14（1-2,2-2,3-2,4-2）
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
    int m_fieldbd4;                                   // +0xbd4（Clear=0；ORIG Clear 0x08367a98 实证）
    float m_fieldbd8;                                 // +0xbd8（Clear=1.0f；ORIG Clear 实证）
    std::string m_scriptName;                         // +0xbdc（ImportCharacterScript 0x08a0ce06 实证）
    _ComboInfo m_combo;                               // +0xbe0 → sizeof 0xc1c
};

static_assert(sizeof(STRDCharacterStatus) == 0x57, "STRDCharacterStatus size drift");
static_assert(sizeof(CharacterEtcStats) == 0x1c, "CharacterEtcStats size drift");
static_assert(sizeof(ActiveActionFileSet) == 0x40, "ActiveActionFileSet size drift");
static_assert(sizeof(_ComboInfo) == 0x3c, "_ComboInfo size drift");
static_assert(sizeof(STCharacterScript::STSecondGrowTypeData) == 0x100,
              "STSecondGrowTypeData size drift");
static_assert(sizeof(STCharacterScript) == 0xc1c, "STCharacterScript size drift");
// [round1 L26] 数组化后的同址/步长锁（零布局变化证明）
static_assert(offsetof(STCharacterScript, m_status[1]) -
              offsetof(STCharacterScript, m_status) == 0x57, "m_status stride drift");
static_assert(offsetof(STCharacterScript::STSecondGrowTypeData, m_statusA[1]) -
              offsetof(STCharacterScript::STSecondGrowTypeData, m_statusA) == 0x57,
              "m_statusA stride drift");

#endif  // GAME_STCHARACTERSCRIPT_H_
