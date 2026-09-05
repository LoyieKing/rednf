#ifndef GAME_CMISSIONLIST_CHARAC_H_
#define GAME_CMISSIONLIST_CHARAC_H_

// df_game_r CMissionList_Charac（G3-1 批次，2026-08-17）。
// 布局依据 ORIG ctor 0x832af8a / dtor 0x85ec626 / _reset 0x85e476c /
// CDataMgr::CDataMgr new 尺寸 0x158 与 ORIG getIssue_all/getIssue_mode/
// isClearMission 字段偏移（m_field148@0x148 / m_field14c@0x14c）：
//   charac_expand::CData 基类（pack(1)：vptr +0x00 / m_bIsSave +0x04，sizeof 5）
//   MissionInfo m_missionList[36]     +0x05（步长 8，kind 0..0x23；ORIG _reset
//                                      memset 0x120 起 +0x5 实证）
//   char m_pad125[3]                  +0x125
//   std::bitset<256> m_clearedFlags   +0x128（32 字节）
//   short m_field148                  +0x148
//   short m_field14a                  +0x14a
//   short m_field14c                  +0x14c
//   char  m_field14e                  +0x14e
//   CUser* m_user                     +0x154
//   sizeof = 0x158（pack(1)）。
// 虚表 0x8cc3c08 槽位：ResetDailyMidnight @0（基类）/ loadData @1 / getData @2 /
//   ResetDaily @3 / ~ @4,5 / _saveData @6 / _reset @7。

#include <bitset>

#include "CBoosterGage.h"   // charac_expand::CData（pack(1) 基类）
#include "MissionTypes.h"   // MissionInfo / MissionClearCondition_Parameter

class CUser;

#pragma pack(push, 1)

class CMissionList_Charac : public charac_expand::CData
{
public:
    CMissionList_Charac();
    virtual ~CMissionList_Charac();

    virtual bool loadData(CUser* pUser, char* pData);
    virtual void getData(char* pData) const;
    virtual bool _saveData(CUser* pUser);
    virtual void _reset();

    // ---- 事件入口 ----
    void Update_EnterWorld_event(CUser& user);
    void Update_Kill_event(CUser& user,
                           const MissionClearCondition_Parameter& param);
    void Update_Win_event(CUser& user,
                          const MissionClearCondition_Parameter& param,
                          bool& outClear);
    void Update_Revenge_event(CUser& user, PVP_BATTLE_MODE mode, bool win);
    void Update_GameOver_event(CUser& user,
                               const MissionClearCondition_Parameter& param);
    void Update_RecvPacket_event(CUser& user, std::vector<short>& kinds,
                                 std::vector<int>& values);
    void Update_ComboMission_Event(CUser& user, int comboIdx, short count);
    void Update_Within_Mission_event(CUser& user,
                                     const MissionClearCondition_Parameter& param,
                                     bool& outClear);
    void check_mission_kind(unsigned int kind, CUser& user,
                            const MissionClearCondition_Parameter& param,
                            std::vector<int>& outClearList);

    // ---- 列表管理 ----
    void MakeMissionList_forNewbies(const CUser& user);
    void MakeMissionList_forOldUser(const CUser& user);
    bool MakeMissionList_JustKind(const CUser& user, unsigned int kind);
    void addNewMission(const MissionInfo& info);
    void Remove_Mission(int kind);
    void ClearMission(CUser& user, short index);
    void ClearMissionKind(CUser& user, short kind);
    void ResetMissionList(CUser& user);
    void ResetDaily();
    void Reset_DailyMission();
    void GM_SetMissionValue(int kind, int value);
    int getNewMission(CUser& user, int kind);

    // ---- 发送 ----
    void Send_MissionList(CUser& user);
    void Send_Cleared_Mission_List(CUser& user,
                                   const std::vector<int>& clearList);
    void Send_Fail_Mission(CUser& user, short index);
    void setIssue(bool isNewbie, bool isDaily);

    // ---- 查询 ----
    bool isClearMission(int index) const;
    short getIndex_byKind(int kind) const;
    unsigned short getIssue_mode(bool isDaily) const;   // ORIG 单 movzwl 0x148/0x14a（返回 unsigned short）
    unsigned short getIssue_all() const;   // ORIG 单 movzwl（读 m_field14c unsigned short）
    int getWinningRate() const;
    bool empty_progress() const;
    bool isUserNewbie() const;
    bool isClearStealingSkillMission() const;

    // ---- 成员 ----
    MissionInfo m_missionList[36];       // +0x05
    char m_pad125[3];             // +0x125（ORIG memset 0x120 起 +0x5；bitset 对齐 0x128）
    std::bitset<256> m_clearedFlags;     // +0x128
    unsigned short m_field148;                 // +0x148
    unsigned short m_field14a;           // +0x14a
    unsigned short m_field14c;           // +0x14c
    bool m_field14e;                     // +0x14e
    // +0x150 计数字段化（2026-09-05 BossMission 批次，ORIG 证据：
    //   getData T 0x085e4aee `mov 0x150(%eax),%edx` + `mov %edx,(%eax)` ——
    //   本对象 +0x150 的 int 整体导出到 SIG_REQUEST_PVP_MISSION_LIST::m_characNo
    //   （+0x00，_saveData 侧同槽已具名为 charac no），故本槽建模为 m_characNo；
    //   本类不写该槽（ORIG ctor 0x832af90 仅隐式构造，槽位 new 后 memset(0,0x158)
    //   清零 → 恒 0，属 ORIG 未还原写入点【推断】）。
    // union 保留旧基名 m_pad14f（唯一消费者 CMissionList_Charac::getData 已同步迁移）。
    union
    {
        char m_pad14f[5];                // +0x14f..0x153（旧基名保留）
        struct
        {
            char m_pad14f0;              // +0x14f（ORIG 未访问）
            int  m_characNo;             // +0x150
        } m_tail14f;
    };
    CUser* m_user;                       // +0x154
};

#pragma pack(pop)

// 布局自检：字段化仅拆 pad，sizeof 与既有成员偏移零漂移。
static_assert(sizeof(CMissionList_Charac) == 0x158, "CMissionList_Charac size drift");
static_assert(offsetof(CMissionList_Charac, m_missionList) == 0x05, "missionList offset drift");
static_assert(offsetof(CMissionList_Charac, m_clearedFlags) == 0x128, "clearedFlags offset drift");
static_assert(offsetof(CMissionList_Charac, m_tail14f.m_characNo) == 0x150, "characNo offset drift");
static_assert(offsetof(CMissionList_Charac, m_user) == 0x154, "user offset drift");

#endif  // GAME_CMISSIONLIST_CHARAC_H_
