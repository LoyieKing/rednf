#ifndef GAME_CMISSIONLIST_CHARAC_H_
#define GAME_CMISSIONLIST_CHARAC_H_

// df_game_r CMissionList_Charac（G3-1 批次，2026-08-17）。
// 布局依据 ORIG ctor 0x832af8a / dtor 0x85ec626 / _reset 0x85e476c /
// CDataMgr::CDataMgr new 尺寸 0x158：
//   charac_expand::CData 基类（pack(1)：vptr +0x00 / m_bIsSave +0x04，sizeof 5）
//   MissionInfo m_missionList[36]      +0x05（步长 8，kind 0..0x23）
//   std::bitset<256> m_clearedFlags    +0x128（32 字节）
//   short m_field148                   +0x148
//   short m_field14a                   +0x14a
//   short m_field14c                   +0x14c
//   char  m_field14e                   +0x14e
//   CUser* m_user                      +0x154
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
    int getIssue_mode(bool isDaily) const;
    int getIssue_all() const;
    int getWinningRate() const;
    bool empty_progress() const;
    bool isUserNewbie() const;
    bool isClearStealingSkillMission() const;

    // ---- 成员 ----
    MissionInfo m_missionList[36];       // +0x05
    std::bitset<256> m_clearedFlags;     // +0x128
    short m_field148;                    // +0x148
    short m_field14a;                    // +0x14a
    short m_field14c;                    // +0x14c
    char m_field14e;                     // +0x14e
    char m_pad14f[5];                    // +0x14f..0x153
    CUser* m_user;                       // +0x154
};

#pragma pack(pop)

#endif  // GAME_CMISSIONLIST_CHARAC_H_
