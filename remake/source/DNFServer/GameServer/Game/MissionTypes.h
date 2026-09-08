#ifndef GAME_MISSIONTYPES_H_
#define GAME_MISSIONTYPES_H_

// df_game_r 任务系统共享类型（G3-1 批次，2026-08-17）。
// 布局依据 ORIG 反汇编推导：
//   MissionInfo              8 字节（kind/index/count/streak 各 short）
//   MissionScript            0x79 字节（CMission ctor/dtor 推导）
//   MissionClearCondition_Parameter  pack(1)，ctor 0x85e1a44
//   combo_script_key         4 字节（char job + short growType，clear=0x8a5a858）
//   combo_script_data        24 字节（两个 vector<pair<int,int>>）

#include <bitset>
#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>

// PvP 战斗模式（PvP_deps.h 亦有定义；本头独立守卫避免重定义）
#ifndef GAME_MISSIONTYPES_PVP_BATTLE_MODE_DEFINED
#define GAME_MISSIONTYPES_PVP_BATTLE_MODE_DEFINED
enum PVP_BATTLE_MODE
{
    PVP_BATTLE_MODE_1 = 1,
    PVP_BATTLE_MODE_2 = 2,
    PVP_BATTLE_MODE_3 = 3,
    PVP_BATTLE_MODE_4 = 4,
    PVP_BATTLE_MODE_5 = 5
};
#endif

// ---- 任务进度（客户端/存档结构，8 字节） ----
struct MissionInfo
{
    short m_kind;    // +0x00
    short m_index;   // +0x02
    short m_count;   // +0x04（进度/连击位图）
    short m_streak;  // +0x06（连胜/状态/连击位图）

    MissionInfo()   // ORIG C1/C2 W 0x0832aede：4×movw 逐字段清零
    {
        m_kind = 0;
        m_index = 0;
        m_count = 0;
        m_streak = 0;
    }

    void clear();   // ORIG 0x85e68b6 W：4×movw 逐字段清零
};

// ---- 连击脚本键（4 字节） ----
struct combo_script_key
{
    char m_job;       // +0x00（0xb 表示无职业）
    short m_growType; // +0x02

    combo_script_key() { clear(); }
    void clear() { m_job = 0xb; m_growType = 0; }
    bool operator<(const combo_script_key& other) const
    {
        if (m_job == other.m_job) return m_growType < other.m_growType;
        return m_job < other.m_job;
    }
};

// ---- 连击脚本数据（20 字节：奖励表 + 单项奖励） ----
// ORIG C1 0x8a5bd94 W（objdump）：隐式 vector C1@+0x00、pair C1@+0x0c 后调 clear()
//   T 0x8a5a7f4（m_rewardList.clear() + m_bonus.first/second 清零）。报告仅有
//   combo_script_data/{README,clear}.md；ctor 序列按 objdump【推断落位】。
struct combo_script_data
{
    std::vector<std::pair<int, int> > m_rewardList;    // +0x00（前 4 项 → count 位图）
    std::pair<int, int> m_bonus;                       // +0x0c（单项组合奖励）

    combo_script_data() { clear(); }                   // ORIG C1/C2 0x8a5bd94 W
    void clear()                                       // ORIG T 0x8a5a7f4 (0x27)
    {
        m_rewardList.clear();
        m_bonus.first = 0;
        m_bonus.second = 0;
    }
};

// ---- 任务脚本（CDataManager 装载，0x88 字节） ----
// ORIG 布局（Factory 0x85e2132 读 +0x34 为类型分派；派生 ctor 参数读
// +0x7c/+0x80/+0x84；comboMap 在 +0x60）。Load_list 的 'MissionScript local_c8[136]'
// 实证总尺寸 0x88。
struct MissionScript
{
    MissionScript();   // ORIG C1 W：成员构造后调 clear()
    void clear();   // ORIG 0x08a5bbb2

    int m_field00;                                   // +0x00（[next mission]）
    int m_prevMissionIndex;                          // +0x04（[prev mission]）
    int m_field08;                                   // +0x08
    int m_field0c;                                   // +0x0c（脚本列表索引）
    // [b28c-lane17] ORIG ctor 0x08a5ba8c 对 +0x10/+0x18 各发一次
    // `_ZNSt4pairIiiEC1Ev`（默认构造），clear() 0x08a5bbb2 仅对 +0x10 发
    // pair 构造+operator= ⇒ 真实成员是两枚 std::pair<int,int>（非 4 个 int）。
    std::pair<int, int> m_grade;                     // +0x10（clear 重置为 (0,0)）
    std::pair<int, int> m_rate;                      // +0x18（仅 ctor 默认构造）
    int m_field20;                                   // +0x20
    int m_job;                                       // +0x24
    std::vector<int> m_field28;                      // +0x28
    int m_field34;                                   // +0x34（Factory 类型分派 switch）
    std::string m_str38;                             // +0x38
    std::string m_str3c;                             // +0x3c
    float m_expRate;                                 // +0x40
    std::vector<std::pair<int, int> > m_rewardItems; // +0x44
    std::vector<int> m_field50;                      // +0x50
    char m_stealingSkill;                            // +0x5c（[stealing skill]）
    char m_rewardSkill;                              // +0x5d（[reward skill]）
    char m_rewardSp;                                 // +0x5e（[reward sp]）
    char m_pad5f;                                    // +0x5f
    std::map<combo_script_key, combo_script_data> m_comboMap;  // +0x60
    int m_kind;                                      // +0x78（_ReadThreeInt 第 1 值；Factory/CMission 使用）
    int m_field7c;                                   // +0x7c（_ReadThreeInt 第 2 值）
    int m_field80;                                   // +0x80（_ReadThreeInt 第 3 值）
    int m_field84;                                   // +0x84（_ReadFourInt 第 4 值）
};

// ---- PvP 任务参数脚本（PvP_MissionSystem::copy_missionParameter 0x85e206e）----
// 布局：+0x00 vector<uint>（daily indices）+0xc int +0x10 list<pair<uint,uint>>
struct MissionParameterScript
{
    void clear();   // ORIG 0x08a5bca2

    std::vector<unsigned int> m_dailyIndices;                // +0x00
    int m_field0c;                                           // +0x0c
    std::list<std::pair<unsigned int, unsigned int> > m_dailyList;  // +0x10
    int m_field18;                                           // +0x18
    std::vector<unsigned int> m_baseExp;                     // +0x1c
};

// ---- 任务清除条件参数（pack(1)） ----
#pragma pack(push, 1)
class MissionClearCondition_Parameter
{
public:
    MissionClearCondition_Parameter(PVP_BATTLE_MODE mode);

    // ORIG canUpdate 等 movzbl 零扩展读取 m_mode → 无符号比较
    unsigned char m_mode;    // +0x00（0x0a 默认；2→1、3→2）
    unsigned short m_victory; // +0x01（bit0 = 本场胜利）
    int m_killCount;         // +0x03（击杀计数）
    int m_channelFrom;       // +0x07（换线前频道）
    int m_channelTo;         // +0x0b（换线后频道）
};
#pragma pack(pop)

// 连胜判定（ORIG 0x8a5aa4e，属统计域 TU）
bool is_Victory_in_a_row(std::bitset<32> bits, short victory);

#endif  // GAME_MISSIONTYPES_H_
