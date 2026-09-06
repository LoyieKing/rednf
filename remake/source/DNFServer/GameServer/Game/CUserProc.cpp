// ============================================================================
// df_game_r 还原 —— APSystem::CUserProc（G2-4 批次，活动点/奖励动作处理）。
// 依据 docs/class_func_reports/APSystem__CUserProc.md + ORIG 反汇编。
// 多数方法 ORIG 即为空实现（奖励/广播在目标构建中禁用）；这里按 ORIG
// 语义转录，保留真实调用与消息构造。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "CUserProc.h"   // APSystem::CUserProc / _ActionGroupIndex（唯一声明点）
#include "CUser.h"       // CUser 权威定义（m_actionPoint @+0x8d264）
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"

void APSystem::CUserProc::ClearActionAndSendtoUser(CUser* user,
                                                   _ActionGroupIndex group,
                                                   int value,
                                                   ENUM_PACKETCLASS packetClass)
{
}

void APSystem::CUserProc::SendActionClear(CUser& user, unsigned short actionNo,
                                          _ActionGroupIndex group, bool flag,
                                          ENUM_PACKETCLASS packetClass)
{
    _MSG_ACTION_POINT_ACTION_CLEAR msg;
    msg.m_group = (int)group;
    msg.m_actionNo = actionNo;
    msg.m_flag = flag ? 1 : 0;

    PacketGuard guard;
    if (packetClass == ENUM_PACKETCLASS_0)
    {
        ((InterfacePacketBuf*)&guard)->put_header(0, 0x1f3);
    }
    else
    {
        ((InterfacePacketBuf*)&guard)->put_header(1, 0x228);
    }
    ((InterfacePacketBuf*)&guard)->put_binary((char*)&msg, 0xb);
    ((InterfacePacketBuf*)&guard)->finalize(true);
    user.Send(guard);
}

void APSystem::CUserProc::SendActionListState(CUser* user)
{
    if (user != 0)
    {
        SendActionListState(*user);
    }
}

void APSystem::CUserProc::SendActionListState(CUser& user)
{
}

void APSystem::CUserProc::SendGetRewardItem(CUser& user,
                                            _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward,
                                            bool flag)
{
    PacketGuard guard;
    ((InterfacePacketBuf*)&guard)->put_header(1, 0x229);

    char localFlag;
    int localItem;
    localItem = reward.m_itemIdx;
    localFlag = flag ? 1 : 0;
    ((InterfacePacketBuf*)&guard)->put_binary(&localFlag, 5);
    ((InterfacePacketBuf*)&guard)->finalize(true);
    user.Send(guard);
}

void APSystem::CUserProc::SetTodayActionAndCheckMedalReward(CUser* user)
{
}

void APSystem::CUserProc::UpdateActionPoint(CUser* user)
{
}

void APSystem::CUserProc::GetTodayRewardAndSendPostal(
    CUser* user, _SIG_LOAD_ACTION_POINT::_TodayRewardItem reward)
{
}

void APSystem::CUserProc::CheckAndSendToMessageAllUser(unsigned int a,
                                                       unsigned int b,
                                                       const char* msg)
{
}

void APSystem::CUserProc::SendToMessageAllUser(const char* msg, const char* fmt)
{
    if (msg != 0 && fmt != 0)
    {
        char buf[1024];
        sprintf(buf, fmt, msg);
        const char* pMsgName = "chn_game_server_msg_82";
        int msgLen = strlen(buf);
        int a = 0;
        int b = 0;
    }
}

void APSystem::CUserProc::GM_Reset(CUser* user)
{
    if (user != 0)
    {
        SetTodayActionAndCheckMedalReward(user);
    }
}

void APSystem::CUserProc::GM_Set(CUser* user, unsigned int value)
{
    if (user != 0)
    {
        APSystem::CActionPointManager* mgr = &user->m_actionPoint;
        mgr->GM_Set(*user, value);
        SendActionListState(*user);
    }
}
