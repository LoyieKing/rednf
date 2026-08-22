#ifndef DNF_GAME_CUSERPROC_H_
#define DNF_GAME_CUSERPROC_H_

// ============================================================================
// df_game_r APSystem::CUserProc（动作点/奖励 UI 流程，独立 TU 实现于
// CUserProc.cpp）。唯一声明点（原 CUserProc.cpp 本地类迁出）。
// ORIG 符号：ClearActionAndSendtoUser 0x08122390（报告
// docs/class_func_reports/APSystem__CUserProc/ClearActionAndSendtoUser.md）。
// _SIG_LOAD_ACTION_POINT 的权威声明在 APSystemTypes.h（CDataManager.h 依赖）。
// ============================================================================

#include "APSystemTypes.h"   // APSystem::_SIG_LOAD_ACTION_POINT
#include "GameEnums.h"       // ENUM_PACKETCLASS

class CUser;

namespace APSystem
{

class _ActionGroupIndex
{
public:
    int m_value;  // +0x00
};

class _MSG_ACTION_POINT_ACTION_CLEAR
{
public:
    _MSG_ACTION_POINT_ACTION_CLEAR();

    int m_group;       // +0x00
    int m_field4;      // +0x04（未初始化区）
    unsigned short m_actionNo;  // +0x08
    char m_flag;       // +0x0a
};

class CUserProc
{
public:
    static void ClearActionAndSendtoUser(CUser* user, _ActionGroupIndex group,
                                         int value, ENUM_PACKETCLASS packetClass);
    static void SendActionClear(CUser& user, unsigned short actionNo,
                                _ActionGroupIndex group, bool flag,
                                ENUM_PACKETCLASS packetClass);
    static void SendActionListState(CUser* user);
    static void SendActionListState(CUser& user);
    static void SendGetRewardItem(CUser& user,
                                  _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward,
                                  bool flag);
    static void SetTodayActionAndCheckMedalReward(CUser* user);
    static void UpdateActionPoint(CUser* user);
    static void GetTodayRewardAndSendPostal(
        CUser* user, _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward);
    static void CheckAndSendToMessageAllUser(unsigned int a, unsigned int b,
                                             const char* msg);
    static void SendToMessageAllUser(const char* msg, const char* fmt);
    static void GM_Reset(CUser* user);
    static void GM_Set(CUser* user, unsigned int value);
};

}  // namespace APSystem

#endif  // DNF_GAME_CUSERPROC_H_
