// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPowerWarEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "CPowerWarEvent/CPowerWarEvent.inc"
{
}

// ORIG 0x082679ac / 0x082679dc：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CPowerWarEvent/dtor_CPowerWarEvent.inc"
{
}

// ---------------------------------------------------------------------------
// ORIG 0x08267dea：环境为 1/2 时向 monitor 发 Event_End 包；
// 否则事件未进行中则直接返回，进行中则开始力战（关终极副本、发配置、
// 挂三个定时器）。
// [b28d-solo-CPowerWarEvent] AE（446h47 路由后）。P221 全表达式临时链收口槽位/块序；
//   唯一曾存残差=虚调 IsEventing(0) 间接目标寄存器（ORIG 0x08267e80 `mov (%eax),%edx` +
//   0x08267e90 `call *%edx`、this 入 %eax；4.4.7 恒目标 %eax、this 入 %edx = N7）。
//   P230：本件属 N7 敏感族，Main 已将本 TU 纳入 446h47「仅换 4.4.6 后端」路由 ⇒
//   compare_tu_game_orig: identical=2 ae=11 near=0（本件由 NEAR 转 AE）；p167 CLEAR 50=50。
#include "CPowerWarEvent/StartEvent.inc"
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
// [b28d-solo-CPowerWarEvent] AE（446h47 路由后）。与 StartEvent(Word_Param) 同族：
//   P221 临时链后唯一曾存残差=虚调 IsEventing(0) 目标寄存器（ORIG 0x08267a90
//   `mov (%eax),%edx` + 0x08267aa0 `call *%edx`，this 入 %eax；4.4.7 恒 %eax/%edx 对调）。
//   P230：446h47 路由后本件由 NEAR 转 AE（compare_tu_game_orig near 3→0）；p167 CLEAR 50=50。
#include "CPowerWarEvent/StartEvent_1.inc"
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
// [b28d-solo-CPowerWarEvent] AE（446h47 路由后）。P221 临时链（含 0xde/0xe1/0xe4/0x110
//   四 trace）收口槽位/块序，唯一曾存残差=虚调 IsEventing(0) 目标寄存器（ORIG 0x08268226
//   `mov (%eax),%edx` + 0x08268236 `call *%edx`、this 入 %eax；4.4.7 恒目标 %eax = N7）。
//   P230：446h47 路由后本件由 NEAR 转 AE（compare_tu_game_orig near 3→0）；p167 CLEAR 34=34。
//   另修正真语义 bug：0xe4 trace 文本 ORIG .rodata 0x8bf0544 为 "…DUNGEON OPEN!!"（旧写 CLOSE）。
#include "CPowerWarEvent/EndEvent.inc"
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
                // [b28d-solo-CPowerWarEvent] 真语义 bug 修正：ORIG .rodata 0x8bf0544 实为
                // "…DUNGEON OPEN!!"（objdump -s 核验），旧文写 CLOSE 与 ORIG 不一致。
                ("ULTIMATE_LOG : ALLEADY ULTIMATE DUNGEON OPEN!!");
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
#include "CPowerWarEvent/CleanupPowerWarServerMessage.inc"
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
