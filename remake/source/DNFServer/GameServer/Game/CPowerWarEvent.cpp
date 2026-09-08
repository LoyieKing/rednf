// df_game_r 力战事件 CPowerWarEvent 实现。
// 报告：docs/class_func_reports/CPowerWarEvent/ 全部 7 个方法
// （CPowerWarEvent / ~CPowerWarEvent / ~CPowerWarEvent_082679dc /
//  StartEvent / StartEvent_08267dea / EndEvent / CleanupPowerWarServerMessage）。
#include "CPowerWarEvent.h"

#include "CEnvironment.h"          // G_CEnvironment / m_serverEnvir.m_serverType / m_channelNo
#include "CDataManager.h"          // G_CDataManager / m_serverParameter / m_powerParameter
#include "TimerDispatcher.h"       // Timer_DungeonInoutOpenTime::checkDungeonInoutTime
#include "GameWorld.h"             // G_GameWorld / send_all / OnEndPowerWar / CancelPowerUp
#include "TimerQueue.h"            // G_TimerQueue / TimerEntry / TIMER_MESSAGE
#include "CPowerManager.h"         // GlobalData::s_power_manager 方法
#include "CMonitorServerProxy.h"   // CMonitorServerProxy::SendTcpPacket
#include "CServerProxyMgr.h"       // CServerProxyMgr::GetServerProxy
#include "Packet_Monitor_Event_End.h"
#include "PacketGuard.h"           // PacketGuard / put_header / put_byte / put_int / put_str / finalize
#include "CGameManager.h"          // G_CGameManager / GetSharedServerMessageManager
#include "CSharedServerMessageManager.h"  // CSharedServerMessageManager::DeleteSpecificMessage
#include "CAssaultMgr.h"           // pvp_assault::GetInstanceAssaultMgr
#include "GameTypes.h"             // cMyTrace
#include <cstring>                 // strlen

// ---------------------------------------------------------------------------
// ORIG 0x08267990：仅基类构造 + vptr 置位（无自有成员初始化）。
CPowerWarEvent::CPowerWarEvent()
{
}

// ORIG 0x082679ac / 0x082679dc：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
CPowerWarEvent::~CPowerWarEvent()
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08267dea：环境为 1/2 时向 monitor 发 Event_End 包；
// 否则事件未进行中则直接返回，进行中则开始力战（关终极副本、发配置、
// 挂三个定时器）。
// [b28d-26] NEAR 收敛至 1 处：P221 全表达式临时链（cMyTrace(...)(fmt)）后
//   槽位/块序全对齐，仅余 N7 间接 call 目标寄存器（ORIG `call *%edx` vs
//   4.4.7 `call *%eax`，0x08267e26 段；P230：4.4.6 后端可消，本 TU 未路由）⇒ N7 封顶。
void CPowerWarEvent::StartEvent(Word_Param param)
{
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
    {
        Packet_Monitor_Event_End monEnd;
        monEnd.m_eventCode = 0x1e;
        int group = G_CEnvironment()->get_server_group();
        GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)group)
            ->SendTcpPacket(reinterpret_cast<char*>(&monEnd), 0xe);
    }
    else
    {
        if (!IsEventing(0))
        {
            SetEventFlag(true);
            m_param = param;
            if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff) != 0)
            {
                G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, false);
                G_GameWorld()->send_all_dungeon_inout_message(0x2aff, false);
                // [b28d-26] P221：具名 tr 改构造即调用（临时链），复现 ORIG 槽序
                cMyTrace(__PRETTY_FUNCTION__, 0x8f, 0)
                    ("ULTIMATE_LOG : POWER WAR START!! ULTIMATE DUNGEON CLOSE!!");
            }
            else
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x92, 0)
                    ("ULTIMATE_LOG : ALLEADY ULTIMATE DUNGEON CLOSE!!");
            }
            GlobalData::s_power_manager->Init();
            G_GameWorld()->CleanupPowerWarVillToStart();
            GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendPowerWarCfg(
                    static_cast<unsigned short>(
                        G_CDataManager()->m_powerParameter.m_killPointToEndPowerWar),   // +0xa634
                    G_CDataManager()->m_powerParameter.m_basicPowerWarPoint,            // +0xa6a4
                    G_CDataManager()->m_powerParameter.m_firstGradeBonusPoint,          // +0xa6a8
                    G_CDataManager()->m_powerParameter.m_rankingBonusDecreasePoint,     // +0xa6b0
                    G_CDataManager()->m_powerParameter.m_rankingBonusGrade);            // +0xa6ac
            PacketGuard packet;
            packet.put_header(0, 0x9f);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, 0, (TIMER_MESSAGE)0x7b,
                G_CDataManager()->m_powerParameter.m_refreshProcessInfo, 0, 0);        // +0xa658
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x7c,
                G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c,
                0, 0);                                                                  // +0xa670
            GlobalData::s_power_manager->SetPowerUpRemainTime(
                G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c);
            GlobalData::s_power_manager->SetPowerUpTimerPeriod(0);
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x7d,
                                        5, 0, 0);
            register int serverNo = G_CEnvironment()->m_serverEnvir.m_channelNo;       // +0x1b0
            cMyTrace(__PRETTY_FUNCTION__, 0xc6, 0)
                ("Power War Event Start! server(%d)", serverNo);
        }
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x082679fa：同 StartEvent(Word_Param)，但不写 m_param。
// [b28d-26] NEAR 同族收敛：P221 临时链后仅余 N7（`call *%edx` vs `call *%eax`）⇒ N7 封顶。
void CPowerWarEvent::StartEvent()
{
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
    {
        Packet_Monitor_Event_End monEnd;
        monEnd.m_eventCode = 0x1e;
        int group = G_CEnvironment()->get_server_group();
        GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)group)
            ->SendTcpPacket(reinterpret_cast<char*>(&monEnd), 0xe);
    }
    else
    {
        if (!IsEventing(0))
        {
            SetEventFlag(true);
            if (G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff) != 0)
            {
                G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, false);
                G_GameWorld()->send_all_dungeon_inout_message(0x2aff, false);
                cMyTrace(__PRETTY_FUNCTION__, 0x3a, 0)
                    ("ULTIMATE_LOG : POWER WAR START!! ULTIMATE DUNGEON CLOSE!!");
            }
            else
            {
                cMyTrace(__PRETTY_FUNCTION__, 0x3d, 0)
                    ("ULTIMATE_LOG : ALLEADY ULTIMATE DUNGEON CLOSE!!");
            }
            GlobalData::s_power_manager->Init();
            G_GameWorld()->CleanupPowerWarVillToStart();
            GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)0)
                ->SendPowerWarCfg(
                    static_cast<unsigned short>(
                        G_CDataManager()->m_powerParameter.m_killPointToEndPowerWar),
                    G_CDataManager()->m_powerParameter.m_basicPowerWarPoint,
                    G_CDataManager()->m_powerParameter.m_firstGradeBonusPoint,
                    G_CDataManager()->m_powerParameter.m_rankingBonusDecreasePoint,
                    G_CDataManager()->m_powerParameter.m_rankingBonusGrade);
            PacketGuard packet;
            packet.put_header(0, 0x9f);
            packet.finalize(true);
            G_GameWorld()->send_all(packet);
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_0, 0, (TIMER_MESSAGE)0x7b,
                G_CDataManager()->m_powerParameter.m_refreshProcessInfo, 0, 0);
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x7c,
                G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c,
                0, 0);
            GlobalData::s_power_manager->SetPowerUpRemainTime(
                G_CDataManager()->m_powerParameter.m_powerUpGeneratePeriod * 0x3c);
            GlobalData::s_power_manager->SetPowerUpTimerPeriod(0);
            G_TimerQueue()->InsertTimer(TimerEntry::OBJ_TYPE_2, 0, (TIMER_MESSAGE)0x7d,
                                        5, 0, 0);
            register int serverNo = G_CEnvironment()->m_serverEnvir.m_channelNo;
            cMyTrace(__PRETTY_FUNCTION__, 0x70, 0)
                ("Power War Event Start! server(%d)", serverNo);
        }
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x082681e2：环境为 1/2 时直接返回；事件未进行中直接返回；
// 进行中则关力战（开终极副本、OnEndPowerWar/CancelPowerUp 广播、
// 清共享服务器消息、强制关闭 assault、触发 0x35 事件）。
// [b28d-26] NEAR 收敛至 1 处：P221 临时链（含 0xde/0xe1/0xe4/0x110 四 trace）后
//   槽位/块序全对齐，仅余 N7 间接 call 寄存器（0x08268226 段 `call *%edx` vs `*%eax`）⇒ N7 封顶。
void CPowerWarEvent::EndEvent()
{
    if (G_CEnvironment()->m_serverEnvir.m_serverType == 1 ||
        G_CEnvironment()->m_serverEnvir.m_serverType == 2)
    {
        return;
    }
    if (IsEventing(0) != 0)
    {
        SetEventFlag(false);
        m_param.m_duration = 0;
        m_param.m_field2 = 0;
        if (!G_CDataManager()->m_serverParameter.isDungeonOpen(0x2aff))
        {
            if (Timer_DungeonInoutOpenTime::checkDungeonInoutTime(0x2aff) != 0)
            {
                G_CDataManager()->m_serverParameter.setDungeonOpen(0x2aff, true);
                G_GameWorld()->send_all_dungeon_inout_message(0x2aff, true);
                cMyTrace(__PRETTY_FUNCTION__, 0xde, 0)
                    ("ULTIMATE_LOG : POWER WAR EVENT END!! ULTIMATE DUNGEON OPEN!!");
            }
            else
            {
                cMyTrace(__PRETTY_FUNCTION__, 0xe1, 0)
                    ("ULTIMATE_LOG : NOT UTIMATE DUNGEON OPEN TIME!!");
            }
        }
        else
        {
            cMyTrace(__PRETTY_FUNCTION__, 0xe4, 0)   // ORIG 0x0826834d 行号 0xe4
                ("ULTIMATE_LOG : ALLEADY ULTIMATE DUNGEON CLOSE!!");
        }
        PacketGuard packet;
        packet.put_header(0, 0xa0);
        G_GameWorld()->OnEndPowerWar(packet);
        G_GameWorld()->CancelPowerUp();
        packet.finalize(true);
        G_GameWorld()->send_all(packet);
        CleanupPowerWarServerMessage();
        pvp_assault::GetInstanceAssaultMgr()->FrocedCloseAssaults();
        Word_Param param;
        param.m_duration = 0;
        param.m_field2 = 0;
        GlobalData::s_event_manager->TriggerEventStart(0x35, param);
        cMyTrace(__PRETTY_FUNCTION__, 0x110, 0)
            ("Power War Event End");
    }
}

// ---------------------------------------------------------------------------
// ORIG 0x08268492：删共享消息 1/2/3 后广播三条空串公告（send_all_with_state 3）。
void CPowerWarEvent::CleanupPowerWarServerMessage()
{
    CSharedServerMessageManager* mgr =
        G_CGameManager()->GetSharedServerMessageManager();
    if (mgr != 0)
    {
        mgr->DeleteSpecificMessage(1);
        mgr->DeleteSpecificMessage(2);
        mgr->DeleteSpecificMessage(3);
        PacketGuard packet;
        packet.put_header(0, 0xc0);
        packet.put_byte(2);
        packet.put_byte(3);
        char* msg = "null string";
        int len = static_cast<int>(strlen(msg));
        for (int i = 0; i <= 2; ++i)
        {
            packet.put_byte(i + 1);
            packet.put_int(len);
            packet.put_str(msg, len);
            packet.put_int(len);
            packet.put_str(msg, len);
        }
        packet.finalize(true);
        G_GameWorld()->send_all_with_state(packet, 3);
    }
}
