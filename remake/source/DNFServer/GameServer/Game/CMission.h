#ifndef GAME_CMISSION_H_
#define GAME_CMISSION_H_

// df_game_r 任务类族（G3-1 批次，2026-08-17）。
// CMission 布局依据 ORIG ctor 0x85e2ffc / 弱访问器 0x85e6906..0x85e6959：
//   vptr +0x00；m_conditionType +0x04（byte，canUpdate 用）；
//   m_kind +0x08；m_missionIndex +0x0c；m_field10 +0x10；
//   m_prevMissionIndex +0x14；m_gradeMin/Max +0x18/+0x1c；
//   m_rateMin/Max +0x20/+0x24；m_grade +0x28；m_expRate +0x2c；
//   m_rewardItems vector +0x30；m_job +0x3c。
// 虚表槽位（0x8cc4048 族）：_makeTotalMissionInfo/_canUpdate/_isAcceptCondition/
//   ~dtor(D1/D0)/isClearable/verifyMission/isSeriesMission/makeMissionInfo/
//   Update/getClearCondition/Reward/_Reward。

#include <map>
#include <vector>

#include "MissionTypes.h"

class CUser;

class CMission
{
public:
    CMission(const MissionScript& script);
    virtual ~CMission();

    virtual void _makeTotalMissionInfo(MissionInfo& info,
                                       const CUser& user) const = 0;
    virtual bool _canUpdate(const MissionClearCondition_Parameter& param) const;
    virtual bool _isAcceptCondition(const CUser& user) const;
    virtual bool isClearable(MissionInfo& info, bool& clear) const = 0;
    virtual bool verifyMission(const MissionInfo& info) const = 0;
    virtual bool isSeriesMission() const = 0;
    virtual bool makeMissionInfo(MissionInfo& info, const CUser& user) const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const = 0;
    virtual int getClearCondition() const = 0;
    virtual int Reward(CUser& user) const;
    virtual int _Reward(CUser& user) const;

    // ---- 访问器（ORIG 弱符号，偏移已核对） ----
    int GetMissionIndex() const { return m_missionIndex; }
    int GetMissionKind() const { return m_kind; }
    int GetPrevMissionIndex() const { return m_prevMissionIndex; }
    int GetMissionGrade() const { return m_grade; }
    void GetRankRange(int* out) const
    {
        out[0] = m_gradeMin;
        out[1] = m_gradeMax;
    }

    // ---- 非虚入口 ----
    int isAcceptCondition(const CUser& user) const;
    bool canUpdate(const MissionClearCondition_Parameter& param) const;
    // ORIG mangled 名为 _ZN8CMissionltERKS_（非 const 成员）；语义同 const
    bool operator<(const CMission& other);

protected:
    // 系列任务接受条件（_isAcceptCondition_Series，ORIG 0x85e3490）
    bool _isAcceptCondition_Series(const CUser& user) const;

    // 基类成员（+0x04 起；vptr 占 +0x00）
    char m_conditionType;                          // +0x04
    int m_kind;                                    // +0x08
    int m_missionIndex;                            // +0x0c
    int m_field10;                                 // +0x10
    int m_prevMissionIndex;                        // +0x14
    int m_gradeMin;                                // +0x18
    int m_gradeMax;                                // +0x1c
    int m_rateMin;                                 // +0x20
    int m_rateMax;                                 // +0x24
    int m_grade;                                   // +0x28
    float m_expRate;                               // +0x2c
    std::vector<std::pair<int, int> > m_rewardItems;  // +0x30
    int m_job;                                     // +0x3c
};

// ---- 派生任务类（+0x40 起各带条件成员；ctor 弱符号 0x85e69a8..0x85e6e40） ----

class CMission_rank : public CMission
{
public:
    virtual ~CMission_rank();
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_clearCount;  // +0x40
};

class CMission_winning_count : public CMission
{
public:
    CMission_winning_count(const MissionScript& script, int count);
    virtual ~CMission_winning_count();
    virtual void _makeTotalMissionInfo(MissionInfo& info,
                                       const CUser& user) const;
    virtual bool _canUpdate(const MissionClearCondition_Parameter& param) const;
    virtual bool _isAcceptCondition(const CUser& user) const;
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_winCount;  // +0x40
};

class CMission_winning_streak : public CMission
{
public:
    CMission_winning_streak(const MissionScript& script, int streak, int clear);
    virtual ~CMission_winning_streak();
    virtual void _makeTotalMissionInfo(MissionInfo& info,
                                       const CUser& user) const;
    virtual bool _canUpdate(const MissionClearCondition_Parameter& param) const;
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_streak;   // +0x40
    int m_clear;    // +0x44
};

class CMission_join : public CMission
{
public:
    CMission_join(const MissionScript& script, int count);
    virtual ~CMission_join();
    virtual void _makeTotalMissionInfo(MissionInfo& info,
                                       const CUser& user) const;
    virtual bool _isAcceptCondition(const CUser& user) const;
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_joinCount;  // +0x40
};

class CMission_kill_count : public CMission
{
public:
    CMission_kill_count(const MissionScript& script, int count);
    virtual ~CMission_kill_count();
    virtual void _makeTotalMissionInfo(MissionInfo& info,
                                       const CUser& user) const;
    virtual bool _isAcceptCondition(const CUser& user) const;
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_killCount;  // +0x40
};

class CMission_hereafter_join : public CMission
{
public:
    CMission_hereafter_join(const MissionScript& script, int count);
    virtual ~CMission_hereafter_join();
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_joinCount;  // +0x40
};

class CMission_hereafter_winning_count : public CMission
{
public:
    CMission_hereafter_winning_count(const MissionScript& script, int count);
    virtual ~CMission_hereafter_winning_count();
    virtual bool _canUpdate(const MissionClearCondition_Parameter& param) const;
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_winCount;  // +0x40
};

class CMission_remain_hp : public CMission
{
public:
    CMission_remain_hp(const MissionScript& script, int hp, int clear);
    virtual ~CMission_remain_hp();
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_remainHp;  // +0x40
    int m_clear;     // +0x44
};

class CMission_revenge : public CMission
{
public:
    CMission_revenge(const MissionScript& script, int count);
    virtual ~CMission_revenge();
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_revengeCount;  // +0x40
};

class CMission_hereafter_kill_count : public CMission
{
public:
    CMission_hereafter_kill_count(const MissionScript& script, int count);
    virtual ~CMission_hereafter_kill_count();
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_killCount;  // +0x40
};

class CMission_kill : public CMission
{
public:
    CMission_kill(const MissionScript& script, int minKill, int clear);
    virtual ~CMission_kill();
    virtual bool _canUpdate(const MissionClearCondition_Parameter& param) const;
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_minKill;  // +0x40
    int m_clear;    // +0x44
};

class CMission_within : public CMission
{
public:
    CMission_within(const MissionScript& script, int round, int clear, int a);
    virtual ~CMission_within();
    virtual bool _isAcceptCondition(const CUser& user) const;
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_round;   // +0x40
    int m_clear;   // +0x44
    int m_field48; // +0x48
};

class CMission_remains_hp : public CMission
{
public:
    CMission_remains_hp(const MissionScript& script, int hp, int clear);
    virtual ~CMission_remains_hp();
    virtual bool _isAcceptCondition(const CUser& user) const;
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_remainHp;  // +0x40
    int m_clear;     // +0x44
};

class CMission_move_channel : public CMission
{
public:
    CMission_move_channel(const MissionScript& script, int target);
    virtual ~CMission_move_channel();
    virtual bool _isAcceptCondition(const CUser& user) const;
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_targetChannel;  // +0x40
};

class CMission_move_channel_total : public CMission
{
public:
    CMission_move_channel_total(const MissionScript& script, int target);
    virtual ~CMission_move_channel_total();
    virtual bool _isAcceptCondition(const CUser& user) const;
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;

    int m_targetChannel;  // +0x40
};

class CMission_combo_clear : public CMission
{
public:
    CMission_combo_clear(
        const MissionScript& script,
        const std::map<combo_script_key, combo_script_data>& comboMap);
    virtual ~CMission_combo_clear();
    virtual void _makeTotalMissionInfo(MissionInfo& info,
                                       const CUser& user) const;
    virtual bool _isAcceptCondition(const CUser& user) const;
    virtual bool isClearable(MissionInfo& info, bool& clear) const;
    virtual bool verifyMission(const MissionInfo& info) const;
    virtual bool isSeriesMission() const;
    virtual int Update(MissionInfo& info,
                       const MissionClearCondition_Parameter& param) const;
    virtual int getClearCondition() const;
    virtual int _Reward(CUser& user) const;

    std::map<combo_script_key, combo_script_data> m_comboMap;  // +0x40
};

#endif  // GAME_CMISSION_H_
