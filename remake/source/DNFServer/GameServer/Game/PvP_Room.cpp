#include "LogManager.h"
// ============================================================================
// df_game_r PvP_Room（G2-3 PvP 链，PvP 对战房间聚合根，池化 600）还原
// （2026-08-17）。
// 逐函数对照 docs/class_func_reports/PvP_Room.md（92 节）与 ORIG 反汇编实现；
// 布局依据 PvP_Room.h（0x6e8），子对象走 PvP_deps.h/PvP_deps.cpp。
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径：分支/调用目标→<T>、
// 大地址→<A>、去 @plt、剥尾部 nop）。
// 跨类缺失方法（CUser/GameWorld/统计/脚本等）以 asm-label extern 发调用，
// 链接桩由主 agent / 后续批次提供（PvP_deps.cpp 已定义其中大部分）。
// ============================================================================

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>

#include "PvP_Room.h"
#include "CSystemTime.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "TimerQueue.h"
#include "GameWorld.h"
#include "CDataManager.h"
#include "CGameManager.h"        // G_CGameManager / PutPvp / CheckOutPvp
#include "GlobalData.h"          // GlobalData::s_event_manager / s_statistic_proxy_mgr / s_ref_pvp_grade / s_systemTime_
#include "CGuildServerProxy.h"   // CEventManager::GetRepeatEvent
#include "CFairPvPScore.h"
#include "RefPvpGrade.h"
#include "CMissionList_Charac.h"
#include "CPvPLiveEventData.h"
#include "CStatisticServerProxy.h"
#include "CServerProxyMgr.h"
#include "CCommonStatisticsMgr.h"
#include "CDisconnectDetecter.h"
#include "Statistics.h"
#include "CUserProc.h"
#include "Packet_Server_Match_data.h"
#include "MatchRoomList.h"     // IMatch（m_matching 虚表 36 槽权威声明）

// ============================================================================
// ============================================================================
// CSwitchLog（PvP 日志；_ZN10CSwitchLogC1EPKciii / clEPKcz 由 GameStubs 提供）
// ============================================================================
class CSwitchLog
{
public:
    CSwitchLog(const char* name, int line, int a, int b);
    void operator()(const char* fmt, ...);

private:
    char m_pad[0x10];
};
// ============================================================================
// PvP 全局数据（ORIG 定义）：
//   PVP_NORMAL_BATTLE_TIME 0x93f4ee0 / PVP_DM_BATTLE_TIME 0x93f4ee8，各 [2] int
//   —— ORIG 中与 g_equipmentPartSetMap_/characterEquipmentTypeTags 同 TU，
//      定义归 InitScriptProducers_2.cpp（首个具名对象，键名 _GLOBAL__I_PVP_NORMAL_BATTLE_TIME）。
//   PVPMAP_DEATHMATCH_MODE 0x94f70e0 / PVPMAP_ONLY_NORMAL_EXPOSURE 0x94f7108
//   PVPMAP_ONLY_TOURNAMENT_RANDOM 0x94f711c，各 0x14 char（零初始化 BSS）
// ============================================================================
extern int PVP_NORMAL_BATTLE_TIME[2];
extern int PVP_DM_BATTLE_TIME[2];
// ---------------------------------------------------------------------------
// 【A 档 GI 顺序对齐 2026-09-09】本定义置于 TU 首个数据定义之前，使 `_GLOBAL__I_`
// 名等于 ORIG 的 `_Z15IsDeathMatchMapiRb`（P327）。数组改用 extern 前置声明，
// 定义仍在下方 ⇒ 符号与初始化语义零变化。语义与机器码零变化，仅发射序。
// ---------------------------------------------------------------------------
extern bool PVPMAP_DEATHMATCH_MODE[0x14];
#include "PvP_Room/IsDeathMatchMap.inc"

bool PVPMAP_DEATHMATCH_MODE[0x14] = {0};
// [b28d-solo-PvP_Room] P248 档②：ORIG GetCandidateMapIndexList 0x085d88xx 对
// `!arr[i]` 发 `movzbl <A>(%eax),%eax; xor $1,%eax; test %al; je/jne`（取反
// 物化）⇒ 元素类型 bool（char 元素时 4.4 恒 `test; jne` 丢 xor）。宽度/符号
// 尺寸零变化（各 0x14 字节）；写侧 InitScriptProducers_8.cpp 以 char 语义写
// 0/1，bool 表示兼容。
bool PVPMAP_ONLY_NORMAL_EXPOSURE[0x14] = {0};
bool PVPMAP_ONLY_TOURNAMENT_RANDOM[0x14] = {0};
#include "PvP_Room/GetMatchingType.inc"
struct PSeatsView { CUser* m_arr[24]; };
#include "PvP_Room/PvpResultRecvFlag_IsRecv.inc"
// ORIG 直用成员 m_users[i]（+0x30）/m_teams[i]（+0x50）：偏移为 4 倍数，
// gcc 折叠为 (i + off/4)*4 scale-index（0x30/4=0xc、0x50/4=0x14）。
#define P_SEATS(i)   (m_users[i])
#define P_TEAMS(i)   (m_teams[i])
#define P_SEATSTATE(i) (m_seatState[i])
#define P_WIN(i)     (m_winCheck[i])
#define P_TEAMWIN(i) (m_teamWin[i])
#define P_USERS(i)   (m_users[i])
#include "PvP_Room/PvP_Room_ctor.inc"
#include "PvP_Room/PvP_Room_dtor.inc"
#include "PvP_Room/SetIDX.inc"
#include "PvP_Room/get_index.inc"
#include "PvP_Room/get_state.inc"
#include "PvP_Room/set_state.inc"
#include "PvP_Room/get_pvp_battle_mode.inc"
#include "PvP_Room/GetManager.inc"
#include "PvP_Room/GetManagerSeat.inc"
#include "PvP_Room/isRematchable.inc"
#include "PvP_Room/setReamtchable.inc"
#include "PvP_Room/GetRequestedMapNo.inc"
#include "PvP_Room/SetRequestedMapNo.inc"
#include "PvP_Room/GetTeamCount.inc"
#include "PvP_Room/lock.inc"
#include "PvP_Room/unlock.inc"
// CheckTimerKey 0x863bccc，报告缺失，objdump 取证）：计数数组即成员
// m_field22c（+0x22c）；ORIG 寻址折叠为 0xc(%this,(idx+0x88)*4)（0x22c=
// 0xc+0x88*4，gcc scale-index 折叠），索引 (int)msg-0x30 越界区为相邻数组带。
#define P_TIMER_KEY(m)   (m_field22c[(int)(m) - 0x30])
#include "PvP_Room/gen_timer_key.inc"
#include "PvP_Room/get_timer_key.inc"
#include "PvP_Room/CheckTimerKey.inc"
#include "PvP_Room/IsExistPassword.inc"
#include "PvP_Room/IsEndPvpBattle.inc"
#include "PvP_Room/IsInsertTimerRecvPvpRank.inc"
#include "PvP_Room/CalculateVictoryPointOfSinglePlay.inc"
#include "PvP_Room/CalculateVictoryPointOfTeamPlay.inc"
#include "PvP_Room/CalcuateVictoryPoint.inc"
#include "PvP_Room/CheckPassword.inc"
#include "PvP_Room/check_start_state.inc"
#include "PvP_Room/get_pvp_team_cnt.inc"
#include "PvP_Room/get_player_count.inc"
#include "PvP_Room/is_closeable_seat_state.inc"
#include "PvP_Room/IsPvpObserver.inc"
#include "PvP_Room/check_joinable.inc"
#include "PvP_Room/IsQuickJoinablePVPRoom.inc"
#include "PvP_Room/get_team.inc"
#include "PvP_Room/get_manager_team.inc"
#include "PvP_Room/get_user_seat_CUser.inc"
#include "PvP_Room/get_user_seat_int.inc"
#include "PvP_Room/GetTeamIndex.inc"
#include "PvP_Room/check_team_pvp.inc"
#include "PvP_Room/get_remain_seat.inc"
#include "PvP_Room/get_waiter_count.inc"
#include "PvP_Room/count_pvp_user.inc"
#include "PvP_Room/check_authority.inc"
#include "PvP_Room/get_recv_pvp_rank_count.inc"
#include "PvP_Room/get_recv_pvp_end_result_count.inc"
#include "PvP_Room/recv_pvp_rank_all.inc"
#include "PvP_Room/recv_pvp_end_result_all.inc"
#include "PvP_Room/recv_pvp_end_result_team.inc"
#include "PvP_Room/set_recv_pvp_rank_flag.inc"
#include "PvP_Room/set_recv_pvp_end_result_flag.inc"
#include "PvP_Room/is_last_win.inc"
#include "PvP_Room/IsAllMoreThanGrade.inc"
#include "PvP_Room/check_acquire_guild_point.inc"
#include "PvP_Room/check_winner.inc"
#include "PvP_Room/create_room.inc"
#include "PvP_Room/join_room.inc"
#include "PvP_Room/leave_room.inc"
#include "PvP_Room/destroy_room.inc"
#include "PvP_Room/Disconnect.inc"
#include "PvP_Room/SendWalkOut.inc"
#include "PvP_Room/WalkOut.inc"
#include "PvP_Room/OnSetPVPSeatState.inc"
#include "PvP_Room/set_seat_state.inc"
#include "PvP_Room/set_ready_state.inc"
#include "PvP_Room/make_ready_info.inc"
#include "PvP_Room/make_room_info.inc"
#include "PvP_Room/make_seat_info.inc"
#include "PvP_Room/make_state_info.inc"
#include "PvP_Room/get_room_info.inc"
#include "PvP_Room/get_user_list.inc"
#include "PvP_Room/send_to_pvp.inc"
#include "PvP_Room/send_to_pvp_team.inc"
#include "PvP_Room/ClearPvPRoom.inc"
#include "PvP_Room/change_team.inc"
#include "PvP_Room/select_new_manager.inc"
#include "PvP_Room/change_manager_to_winner.inc"
#include "PvP_Room/GiveOuterPenalty.inc"
#include "PvP_Room/VoteLackUser.inc"
#include "PvP_Room/OnConnectP2PPvPTimeout.inc"
#include "PvP_Room/OnTimerWalkOutLackUser.inc"
#include "PvP_Room/HandleTimerUserRevive.inc"
#include "PvP_Room/SkipLastRankResultTimer.inc"
#include "PvP_Room/CancelAllTimer.inc"
#include "PvP_Room/end_pvp.inc"
#include "PvP_Room/end_pvp_result.inc"
#include "PvP_Room/send_pvp_end.inc"
#include "PvP_Room/pvp_request_pvp_rank.inc"
#include "PvP_Room/request_fight.inc"
#include "PvP_Room/revengeMission.inc"
#include "PvP_Room/ChangePvPMode.inc"
#include "PvP_Room/CheckPvPMapMode.inc"
#include "PvP_Room/set_pvp_mode.inc"
#include "PvP_Room/GetCandidateMapIndexList.inc"
#include "PvP_Room/insert_timer_recv_pvp_rank.inc"
#include "PvP_Room/insert_timer_recv_pvp_end_result.inc"
#include "PvP_Room/check_recv_pvp_rank.inc"
#include "PvP_Room/IsThereSameGuildMember.inc"
#include "PvP_Room/get_extra_win_point.inc"
#include "PvP_Room/battle_reset.inc"
#include "PvP_Room/reset.inc"
#include "PvP_Room/calculate_pvp_point.inc"
#include "PvP_Room/update_pvp_point.inc"
#include "PvP_Room/start_pvp.inc"
#include "PvP_Room/die_charac.inc"
#include "PvP_Room/time_out.inc"
#include "PvP_Room/pvp_reward.inc"
#include "PvP_Room/DrawRandomMapIndex.inc"
#include "PvP_Room/SelectRandomMap.inc"
#include "PvP_Room/GetAce.inc"
#include "PvP_Room/GetRanking.inc"
#include "PvP_Room/SetPVPResult.inc"
