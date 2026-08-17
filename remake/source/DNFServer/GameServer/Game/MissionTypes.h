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
        if (m_job != other.m_job) return m_job < other.m_job;
        return m_growType < other.m_growType;
    }
};

// ---- 连击脚本数据（20 字节：奖励表 + 单项奖励） ----
struct combo_script_data
{
    std::vector<std::pair<int, int> > m_rewardList;    // +0x00（前 4 项 → count 位图）
    std::pair<int, int> m_bonus;                       // +0x0c（单项组合奖励）
};

// ---- 任务脚本（CDataManager 装载，0x79 字节） ----
struct MissionScript
{
    int m_field00;                                   // +0x00
    int m_prevMissionIndex;                          // +0x04
    int m_field08;                                   // +0x08
    int m_field0c;                                   // +0x0c
    int m_gradeMin;                                  // +0x10
    int m_gradeMax;                                  // +0x14
    int m_rateMin;                                   // +0x18
    int m_rateMax;                                   // +0x1c
    int m_field20;                                   // +0x20
    int m_job;                                       // +0x24
    std::vector<int> m_field28;                      // +0x28
    char m_pad34[4];                                 // +0x34
    std::string m_str38;                             // +0x38
    std::string m_str3c;                             // +0x3c
    float m_expRate;                                 // +0x40
    std::vector<std::pair<int, int> > m_rewardItems; // +0x44
    std::vector<int> m_field50;                      // +0x50
    char m_pad5c[4];                                 // +0x5c
    std::map<combo_script_key, combo_script_data> m_comboMap;  // +0x60
    char m_kind;                                     // +0x78
};

// ---- 任务清除条件参数（pack(1)） ----
#pragma pack(push, 1)
class MissionClearCondition_Parameter
{
public:
    MissionClearCondition_Parameter(PVP_BATTLE_MODE mode);

    char m_mode;             // +0x00（0x0a 默认；2→1、3→2）
    unsigned short m_victory; // +0x01（bit0 = 本场胜利）
    int m_killCount;         // +0x03（击杀计数）
    int m_channelFrom;       // +0x07（换线前频道）
    int m_channelTo;         // +0x0b（换线后频道）
};
#pragma pack(pop)

// 连胜判定（ORIG 0x8a5aa4e，属统计域 TU）
bool is_Victory_in_a_row(std::bitset<32> bits, short victory);

#endif  // GAME_MISSIONTYPES_H_
