// df_game_r CGameManager（G2 单例对象池/房间管理器）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/CGameManager.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 依赖的池化对象（CTradeSpace/PvP_Room/WarRoom/塔/祭坛/快速队伍/蓝弹珠）与
// 管理器类权威头属后续批次：本 TU 以尺寸垫 + 桩 ctor/dtor，跨类方法以
// asm-label extern 对齐 ORIG 符号（链接桩由主 agent / 后续批次提供）。
#include <string.h>

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "CGameManager.h"
#include "CStreamGuard.h"
#include "CSystemTime.h"
#include "InterfacePacketBuf.h"
#include "LogManager.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"

// ---- GlobalData / 全局单例（CUserCharacInfo.h 已有 namespace GlobalData，
//      与 GlobalData.h 的 class 声明互斥，本 TU 以 namespace extern 对齐符号）----
class MsgQueueMgr;
class StreamPool;
class CSystemTime;
class CMonitorServerProxy;
class CGuildServerProxy;
class CDoubleConnCheckServerProxy;
class CSchoolMgr;
class CGuildAgitManager;
class TimerQueue;
namespace WongWork { class CLogGameChannel; }
template <class T> class CServerProxyMgr;
namespace GlobalData
{
extern MsgQueueMgr* s_msgq_mgr;
extern StreamPool* s_stream_pool;
extern CServerProxyMgr<CMonitorServerProxy>* s_monitor_proxy_mgr;
extern CServerProxyMgr<CGuildServerProxy>* s_guild_proxy_mgr;
extern CDoubleConnCheckServerProxy* s_double_check_proxy;
extern WongWork::CLogGameChannel* s_pLogGameChannel;
extern TimerQueue* s_timerQueue_;
}
extern CSchoolMgr* g_schoolMgr;
extern CGuildAgitManager* g_guildAgitMgr;
extern char* NumberToString(unsigned int value, int radix);

// ============================================================================
// SIG_UPDATE_LINK_CHARAC_CONNECT_STATE（CStreamGuard::GetInBuffer 模板参数，
// ORIG mangled 名 36 字符；真实 struct 属后续批次）
// ============================================================================
struct SIG_UPDATE_LINK_CHARAC_CONNECT_STATE
{
    int m_field0;
    int m_field4;
    int m_field8;
};

// ============================================================================
// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char；
//      构造/析构由 GameStubs 提供单例定义）
// ============================================================================
class PacketGuard : public InterfacePacketBuf
{
public:
    PacketGuard();
    ~PacketGuard();

private:
    int m_nField4;
    char m_cField8;
};

// ============================================================================
// 外部依赖 asm-label extern（ORIG 符号名；链接桩 = GameStubs / 后续批次）
// ============================================================================

// ---- 全局单例 ----
extern "C" void* sub_G_GameWorld() asm("_Z11G_GameWorldv");
extern "C" void* sub_G_TimerQueue() asm("_Z12G_TimerQueuev");
extern "C" void* sub_G_CDataManager() asm("_Z14G_CDataManagerv");
extern "C" void* sub_G_CEnvironment() asm("_Z14G_CEnvironmentv");
extern "C" void* sub_G_EpollHandler() asm("_Z14G_EpollHandlerv");
extern "C" void* sub_GetInstanceShutdowManager() asm("_Z25GetInstanceShutdowManagerv");
extern "C" void* sub_CUserGlobalInfoHandleInstance() asm("_Z29CUserGlobalInfoHandleInstancev");
extern "C" void* sub_CEventCharacterHandlerInstance() asm("_Z30CEventCharacterHandlerInstancev");

// ---- 全局函数 ----
extern "C" char* sub_NumberToString(unsigned int value, int radix) asm("_Z14NumberToStringji");
extern "C" int sub_CheckDailyScheduleTime(long long a, long long b, long long c)
    asm("_Z22CheckDailyScheduleTimeill");
extern "C" int sub_get_rand_int(int v) asm("_Z12get_rand_inti");

// ---- GameWorld ----
extern "C" void sub_GameWorld_send_all(void* world, void* pkt)
    asm("_ZN9GameWorld8send_allER11PacketGuard");
extern "C" void sub_GameWorld_send_all_user(void* world, void* pkt, void* user)
    asm("_ZN9GameWorld8send_allER11PacketGuardP5CUser");
extern "C" char sub_GameWorld_reach_game_world(void* world, void* user)
    asm("_ZN9GameWorld16reach_game_worldEP5CUser");
extern "C" void sub_GameWorld_EraseLoginUser(void* world, void* user)
    asm("_ZN9GameWorld14EraseLoginUserEP5CUser");
extern "C" void sub_GameWorld_leave_game_world(void* world, void* user)
    asm("_ZN9GameWorld16leave_game_worldEP5CUser");
extern "C" char sub_GameWorld_is_dungeon_tag_matching_channel(void* world, char* tag)
    asm("_ZN9GameWorld31is_dungeon_tag_matching_channelEPc");
extern "C" int sub_GameWorld_GetChannelType(void* world) asm("_ZNK9GameWorld14GetChannelTypeEv");
extern "C" char sub_GameWorld_IsEquipSlotSwitchChannel(void* world)
    asm("_ZNK9GameWorld24IsEquipSlotSwitchChannelEv");
extern "C" char sub_GameWorld_IsIntegratedPvPBaseChannel(void* world)
    asm("_ZNK9GameWorld26IsIntegratedPvPBaseChannelEv");

// ---- CEnvironment / channel_script_t ----
extern "C" int sub_channel_script_t_getChannelInfo(void* script, unsigned char a, unsigned int b)
    asm("_ZNK16channel_script_t14getChannelInfoEhj");

// ---- CDataManager ----
extern "C" char sub_CDataManager_get_dimensionInout(void* dm, int idx)
    asm("_ZN12CDataManager18get_dimensionInoutEi");
extern "C" void* sub_CDataManager_getBlueMarbleScript(void* dm)
    asm("_ZN12CDataManager19getBlueMarbleScriptEv");
extern "C" void* sub_CDataManager_GetChannelScript(void* dm)
    asm("_ZNK12CDataManager16GetChannelScriptEv");

// ---- WarAreaCounter ----
extern "C" int sub_WarAreaCounter_GetCurrenTimeTable(void* counter)
    asm("_ZN14WarAreaCounter18GetCurrenTimeTableEv");
extern "C" int sub_WarAreaCounter_GetWarRoomCountAtPeekTime(void* counter, int idx)
    asm("_ZN14WarAreaCounter25GetWarRoomCountAtPeekTimeEi");
extern "C" int sub_WarAreaCounter_GetWarRoomCountLastIndex(void* counter, int idx)
    asm("_ZN14WarAreaCounter24GetWarRoomCountLastIndexEi");
extern "C" int sub_WarAreaCounter_GetWarRoomCountFirstIndex(void* counter, int idx)
    asm("_ZN14WarAreaCounter25GetWarRoomCountFirstIndexEi");

// ---- CTradeSpace ----
extern "C" void sub_CTradeSpace_SetIDX(void* ts, int idx) asm("_ZN11CTradeSpace6SetIDXEi");
extern "C" void sub_CTradeSpace_reset(void* ts) asm("_ZN11CTradeSpace5resetEv");
extern "C" void sub_CTradeSpace_cancel_trade_by_dis(void* ts, void* user)
    asm("_ZN11CTradeSpace19cancel_trade_by_disEP5CUser");

// ---- PvP_Room ----
extern "C" int sub_PvP_Room_leave_room(void* room, void* user, bool* flag)
    asm("_ZN8PvP_Room10leave_roomEP5CUserRb");
extern "C" void sub_PvP_Room_destroy_room(void* room, int v)
    asm("_ZN8PvP_Room12destroy_roomEi");
extern "C" void sub_PvP_Room_send_pvp_end(void* room) asm("_ZN8PvP_Room12send_pvp_endEv");
extern "C" char sub_PvP_Room_IsEndPvpBattle(void* room) asm("_ZN8PvP_Room14IsEndPvpBattleEv");
extern "C" void sub_PvP_Room_make_room_info(void* room, void* buf)
    asm("_ZN8PvP_Room14make_room_infoEPc");
extern "C" void sub_PvP_Room_make_seat_info(void* room, void* buf, int seat)
    asm("_ZN8PvP_Room14make_seat_infoEPci");
extern "C" char sub_PvP_Room_IsExistPassword(void* room) asm("_ZN8PvP_Room15IsExistPasswordEv");
extern "C" void sub_PvP_Room_make_state_info(void* room, void* buf)
    asm("_ZN8PvP_Room15make_state_infoEPc");
extern "C" int sub_PvP_Room_get_pvp_battle_mode(void* room)
    asm("_ZN8PvP_Room19get_pvp_battle_modeEv");
extern "C" char sub_PvP_Room_IsQuickJoinablePVPRoom(void* room, void* user)
    asm("_ZN8PvP_Room22IsQuickJoinablePVPRoomEP5CUser");
extern "C" int sub_PvP_Room_get_recv_pvp_rank_count(void* room)
    asm("_ZN8PvP_Room23get_recv_pvp_rank_countEv");
extern "C" char sub_PvP_Room_IsInsertTimerRecvPvpRank(void* room)
    asm("_ZN8PvP_Room24IsInsertTimerRecvPvpRankEv");
extern "C" void sub_PvP_Room_SetIDX(void* room, int idx) asm("_ZN8PvP_Room6SetIDXEi");
extern "C" int sub_PvP_Room_get_index(void* room) asm("_ZN8PvP_Room9get_indexEv");
extern "C" int sub_PvP_Room_get_waiter_count(void* room)
    asm("_ZNK8PvP_Room16get_waiter_countEv");

// ---- WarRoom ----
extern "C" void sub_WarRoom_CheckState(void* room) asm("_ZN7WarRoom10CheckStateEv");
extern "C" int sub_WarRoom_IsJoinable(void* room, void* user) asm("_ZN7WarRoom10IsJoinableEP5CUser");
extern "C" int sub_WarRoom_WalkOutUser(void* room, void* user) asm("_ZN7WarRoom11WalkOutUserEP5CUser");
extern "C" void sub_WarRoom_MakeRoomInfo(void* room, void* buf) asm("_ZN7WarRoom12MakeRoomInfoEPc");
extern "C" void sub_WarRoom_MakeStateInfo(void* room, void* buf)
    asm("_ZN7WarRoom13MakeStateInfoEPc");
extern "C" void sub_WarRoom_Open(void* room) asm("_ZN7WarRoom4OpenEv");
extern "C" void sub_WarRoom_Prepare(void* room) asm("_ZN7WarRoom7PrepareEv");
extern "C" int sub_WarRoom_GetIndex(void* room) asm("_ZN7WarRoom8GetIndexEv");
extern "C" int sub_WarRoom_GetState(void* room) asm("_ZN7WarRoom8GetStateEv");
extern "C" void sub_WarRoom_SetIndex(void* room, int idx) asm("_ZN7WarRoom8SetIndexEi");
extern "C" void sub_WarRoom_SetState(void* room, int state) asm("_ZN7WarRoom8SetStateE13WARROOM_STATE");
extern "C" char sub_WarRoom_ShutDown(void* room) asm("_ZN7WarRoom8ShutDownEv");

// ---- 死亡塔 / 领主塔 ----
extern "C" void sub_CDeathTower_setIdx(void* tower, int idx)
    asm("_ZN8WongWork11CDeathTower6setIdxEi");
extern "C" int sub_CDeathTower_getIdx(void* tower) asm("_ZN8WongWork11CDeathTower6getIdxEv");
extern "C" void sub_CDeathTower_reset(void* tower) asm("_ZN8WongWork11CDeathTower5resetEv");
extern "C" char sub_CDeathTower_onLeaveUser(void* tower, void* user)
    asm("_ZN8WongWork11CDeathTower11onLeaveUserEP5CUser");
extern "C" void sub_CBossTower_setIdx(void* tower, int idx)
    asm("_ZN8WongWork10CBossTower6setIdxEi");
extern "C" int sub_CBossTower_getIdx(void* tower) asm("_ZN8WongWork10CBossTower6getIdxEv");
extern "C" void sub_CBossStage_reset(void* tower) asm("_ZN8WongWork10CBossStage5resetEv");
extern "C" char sub_CBossTower_onLeaveUser(void* tower, void* user)
    asm("_ZN8WongWork10CBossTower10onLeaveUserEP5CUser");

// ---- 前置祭坛 ----
extern "C" void sub_StageControl_setIndex(void* control, int idx)
    asm("_ZN12advancealtar12StageControl8setIndexEi");
extern "C" int sub_StageControl_getIndex(void* control)
    asm("_ZNK12advancealtar12StageControl8getIndexEv");
extern "C" void sub_StageControl_reset(void* control)
    asm("_ZN12advancealtar12StageControl5resetEv");
extern "C" char sub_StageControl_leaveUser(void* control)
    asm("_ZN12advancealtar12StageControl9leaveUserEv");
extern "C" char sub_StageControl_onTimerStageTick(void* control)
    asm("_ZN12advancealtar12StageControl16onTimerStageTickEv");

// ---- 快速队伍 ----
extern "C" void sub_CQuickParty_set_quick_party_index(void* qp, int idx)
    asm("_ZN10QuickParty11CQuickParty21set_quick_party_indexEi");
extern "C" int sub_CQuickParty_get_quick_party_index(void* qp)
    asm("_ZNK10QuickParty11CQuickParty21get_quick_party_indexEv");
extern "C" void sub_CQuickParty_init(void* qp) asm("_ZN10QuickParty11CQuickParty4initEv");

// ---- 蓝弹珠 ----
extern "C" void sub_BlueMarble_setMemoryPoolIndex(void* bm, int idx)
    asm("_ZN10BlueMarble18setMemoryPoolIndexEi");
extern "C" int sub_BlueMarble_getMemoryPoolIndex(void* bm)
    asm("_ZNK10BlueMarble18getMemoryPoolIndexEv");
extern "C" void sub_BlueMarble_setUserInfo(void* bm, void* script)
    asm("_ZN10BlueMarble11setUserInfoEP23BlueMarbleScriptManager");
extern "C" void sub_BlueMarble_setScript(void* bm, void* script)
    asm("_ZN10BlueMarble9setScriptEP23BlueMarbleScriptManager");
extern "C" char sub_BlueMarble_checkJoin(void* bm) asm("_ZN10BlueMarble9checkJoinEv");
extern "C" void sub_BlueMarble_leaveUser(void* bm, void* user) asm("_ZN10BlueMarble9leaveUserEP5CUser");
extern "C" int sub_BlueMarble_getWaitUserCount(void* bm)
    asm("_ZNK10BlueMarble16getWaitUserCountEv");

// ---- CParty 缺失方法 ----
extern "C" void* sub_CParty_getManager(void* party) asm("_ZN6CParty10getManagerEv");
extern "C" char sub_CParty_IsReturnUserParty(void* party) asm("_ZN6CParty17IsReturnUserPartyEv");
extern "C" char sub_CParty_IsEventCharacParty(void* party) asm("_ZN6CParty18IsEventCharacPartyEv");
extern "C" void sub_CParty_leave_user(void* party, void* user, int type)
    asm("_ZN6CParty10leave_userEP5CUser20ENUM_PARTY_INFO_TYPE");

// ---- CUser 缺失方法 ----
extern "C" char sub_CUser_checkInDeathTower(void* user) asm("_ZN5CUser17checkInDeathTowerEv");
extern "C" char sub_CUser_getSex(void* user) asm("_ZN5CUser6getSexEv");
extern "C" void sub_CUser_giveup_panalty(void* user) asm("_ZN5CUser14giveup_panaltyEv");
extern "C" void* sub_CUser_GetSsnString(void* user) asm("_ZN5CUser12GetSsnStringEv");
extern "C" void* sub_CUser_GetPICSMap(void* user) asm("_ZN5CUser10GetPICSMapEv");
extern "C" void sub_CUser_ReCalcChattingEmoticon(void* user)
    asm("_ZN5CUser22ReCalcChattingEmoticonEv");
extern "C" void sub_CUser_SendChattingEmoticon(void* user)
    asm("_ZN5CUser20SendChattingEmoticonEv");
extern "C" void sub_CUser_log_out(void* user) asm("_ZN5CUser7log_outEv");
extern "C" void sub_CUser_ResetItemByScript(void* user, void* vec)
    asm("_ZN5CUser17ResetItemByScriptERSt6vectorISt4pairIiiESaIS2_EE");
extern "C" void sub_CUser_deleteDailyStackableItem(void* user, void* vec, int a, int b)
    asm("_ZN5CUser24deleteDailyStackableItemERSt6vectorISt4pairIiiESaIS2_EEii");
extern "C" void sub_CUser_deleteSpecificItem(void* user, void* vecA, void* vecB)
    asm("_ZN5CUser18deleteSpecificItemERKSt6vectorISt4pairIiiESaIS2_EERS4_");
extern "C" void sub_CUser_prepareDisconnect(void* user) asm("_ZN5CUser17prepareDisconnectEv");
extern "C" void sub_CUser_SetSaveRentalInfoToExchange(void* user, bool flag)
    asm("_ZN5CUser27SetSaveRentalInfoToExchangeEb");
extern "C" void sub_CUser_SetGameMasterMode(void* user, bool flag)
    asm("_ZN5CUser17SetGameMasterModeEb");
extern "C" void sub_CUser_DeleteRentalItemInfo(void* user, int v)
    asm("_ZN5CUser20DeleteRentalItemInfoEi");
extern "C" void sub_CUser_doLinkCharacDisconnect(void* user)
    asm("_ZN5CUser22doLinkCharacDisconnectEv");
extern "C" void sub_CUser_checkLogOutCorrectly(void* user)
    asm("_ZN5CUser20checkLogOutCorrectlyEv");
extern "C" void sub_CUser_processReturnUserQuestAutoClear(void* user)
    asm("_ZN5CUser31processReturnUserQuestAutoClearEv");
extern "C" void sub_CUser_send_clear_quest_list(void* user)
    asm("_ZN5CUser21send_clear_quest_listEv");
extern "C" char sub_CUser_checkInAdvanceAltar(void* user) asm("_ZNK5CUser19checkInAdvanceAltarEv");
extern "C" char sub_CUser_send_itemspace(void* user, int space)
    asm("_ZN5CUser14send_itemspaceEi");

// ---- 角色扩展数据 / 事件角色 ----
extern "C" void sub_CEventCharacterHandler__makeEventCharacter(void* handler, void* user, int type)
    asm("_ZN22CEventCharacterHandler19_makeEventCharacterEP5CUseri");
extern "C" void sub_CExpandEquipslot_EquipslotSwitch(void* slot, void* user,
                                                     char a, char b, char c)
    asm("_ZN16CExpandEquipslot15EquipslotSwitchEP5CUserccc");
extern "C" void sub_CExpandEquipslot_Send_Expand_Equip_Info(void* slot, void* user)
    asm("_ZN16CExpandEquipslot22Send_Expand_Equip_InfoEP5CUser");
extern "C" void sub_CMissionList_Charac_Update_EnterWorld_event(void* mission, void* user)
    asm("_ZN19CMissionList_Charac23Update_EnterWorld_eventER5CUser");
extern "C" unsigned short sub_CUserGlobalInfoHandle_get_uniqueid(void* handle)
    asm("_ZN21CUserGlobalInfoHandle12get_uniqueidEv");
extern "C" char sub_CCharacterView_isSaveCharacView(void* view)
    asm("_ZN14CCharacterView16isSaveCharacViewEv");

// ---- 技能/称号/成就/物品锁/任务 ----
extern "C" void sub_CTitleBook_sendList(void* book) asm("_ZN10CTitleBook8sendListEv");
extern "C" void sub_CAchievement_sendList(void* ach) asm("_ZN12CAchievement8sendListEv");
extern "C" void sub_CItemLock_SendItemLockList(void* lock, void* user)
    asm("_ZN9item_lock9CItemLock16SendItemLockListEP5CUser");
extern "C" void sub_UserQuest_get_quest_info(void* quest, void* buf)
    asm("_ZNK9UserQuest14get_quest_infoEPc");

// ---- 公会/学校 ----
extern "C" void sub_CSchoolMgr_AddUser(void* mgr, void* user) asm("_ZN10CSchoolMgr7AddUserEP5CUser");
extern "C" void sub_CGuildAgitManager_AllocGuildAgitArea(void* mgr, unsigned int key)
    asm("_ZN17CGuildAgitManager18AllocGuildAgitAreaEj");
extern "C" void sub_DB_GuildExpBookDelete_makeRequest(unsigned int a, int b, int c)
    asm("_ZN21DB_GuildExpBookDelete11makeRequestEji17ENUM_SERVER_GROUP");

// ---- 监视/公会代理 ----
extern "C" int sub_CServerProxyMgrMonitor_GetStartIndex(void* mgr)
    asm("_ZN15CServerProxyMgrI19CMonitorServerProxyE13GetStartIndexEv");
extern "C" int sub_CServerProxyMgrMonitor_GetEndIndex(void* mgr)
    asm("_ZN15CServerProxyMgrI19CMonitorServerProxyE11GetEndIndexEv");
extern "C" int sub_CServerProxyMgrMonitor_GetNextIndex(void* mgr, int* idx)
    asm("_ZN15CServerProxyMgrI19CMonitorServerProxyE12GetNextIndexERi");
extern "C" void* sub_CServerProxyMgrMonitor_GetServerProxy(void* mgr, int group)
    asm("_ZN15CServerProxyMgrI19CMonitorServerProxyE14GetServerProxyE17ENUM_SERVER_GROUP");
extern "C" void* sub_CServerProxyMgrGuild_GetServerProxy(void* mgr, int group)
    asm("_ZN15CServerProxyMgrI17CGuildServerProxyE14GetServerProxyE17ENUM_SERVER_GROUP");
extern "C" void sub_CMonitorServerProxy_SendTcpPacket(void* proxy, void* pkt, int len)
    asm("_ZN19CMonitorServerProxy13SendTcpPacketEPci");
extern "C" void sub_CGuildServerProxy_SendTcpPacket(void* proxy, void* pkt, int len)
    asm("_ZN17CGuildServerProxy13SendTcpPacketEPci");
extern "C" void sub_CDoubleConnCheckServerProxy_SendPacket(void* proxy, void* pkt, int len)
    asm("_ZN27CDoubleConnCheckServerProxy10SendPacketEPci");

// ---- 日志/杂项 ----
extern "C" void sub_HistoryLog_LogClose(void* fp) asm("_ZN10HistoryLog8LogCloseEP8_IO_FILE");
extern "C" void sub_CLogGameChannel_IncInUser(void* log) asm("_ZN8WongWork15CLogGameChannel9IncInUserEv");
extern "C" void sub_CLogGameChannel_IncOutUser(void* log)
    asm("_ZN8WongWork15CLogGameChannel10IncOutUserEv");
extern "C" void sub_CDungeonClearTracer_Trace(void* tracer, const char* msg)
    asm("_ZN19CDungeonClearTracer5TraceEPKc");
extern "C" char sub_CDungeon_isTournamentDungeon(void* dungeon)
    asm("_ZNK8CDungeon19isTournamentDungeonEv");
extern "C" char sub_CDungeon_get_dimension_possible(void* dungeon)
    asm("_ZNK8CDungeon22get_dimension_possibleEv");
extern "C" char sub_CDungeon_isTowerOfDespairDungeon(void* dungeon)
    asm("_ZNK8CDungeon23isTowerOfDespairDungeonEv");
extern "C" void sub_CShutdowManager_SendLastMsgDBQueue(void* mgr)
    asm("_ZN15CShutdowManager18SendLastMsgDBQueueEP5CUser");
extern "C" int sub_EpollHandler_ResetEpoll(void* handler, int fd)
    asm("_ZN12EpollHandler10ResetEpollEi");
extern "C" long long sub_Inter_MonitorGuildPointDel_getLastDeleteTime()
    asm("_ZN26Inter_MonitorGuildPointDel17getLastDeleteTimeEv");

// ---- 快速队伍系统/奖励管理器 ----
extern "C" char sub_CQuickPartySystemManager_cancel_quick_party(
    void* mgr, void* party, bool flag, void* user)
    asm("_ZN10QuickParty24CQuickPartySystemManager18cancel_quick_partyEP6CPartybP5CUser");

// ---- 发包数据类 ctor ----
extern "C" void sub_Packet_Monitor_UDP_Reply_UserInfo_ctor(void* pkt)
    asm("_ZN33Packet_Monitor_UDP_Reply_UserInfoC1Ev");
extern "C" void sub_Packet_CutOff_UDP_Reply_UserInfo_ctor(void* pkt)
    asm("_ZN32Packet_CutOff_UDP_Reply_UserInfoC1Ev");
extern "C" void sub_Packet_Monitor_Char_Info_ctor(void* pkt)
    asm("_ZN24Packet_Monitor_Char_InfoC1Ev");
extern "C" void sub_Packet_ChannelType_ctor(void* pkt) asm("_ZN18Packet_ChannelTypeC1Ev");
extern "C" void sub_SendUserInfoToUpperServer(void* pkt, unsigned char type, void* user, int group)
    asm("_Z25SendUserInfoToUpperServerR33Packet_Monitor_UDP_Reply_UserInfohP5CUser17ENUM_SERVER_GROUP");

// ---- CSwitchLog（PvP 日志；桩由主 agent / 后续批次提供）----
class CSwitchLog
{
public:
    CSwitchLog(const char* name, int line, int a, int b);
    void operator()(const char* fmt, ...);
    char m_pad[0x10];
};

// ---- 管理器类 ----
extern "C" void sub_CQuickPartySystemManager_ctor(void* mgr)
    asm("_ZN10QuickParty24CQuickPartySystemManagerC1Ev");
extern "C" void sub_CQuickPartySystemManager_dtor(void* mgr)
    asm("_ZN10QuickParty24CQuickPartySystemManagerD1Ev");
extern "C" void sub_CQuickPartyRewardManager_ctor(void* mgr)
    asm("_ZN10QuickParty24CQuickPartyRewardManagerC1Ev");
extern "C" void sub_CQuickPartyRewardManager_dtor(void* mgr)
    asm("_ZN10QuickParty24CQuickPartyRewardManagerD1Ev");
extern "C" void sub_CPremiumLetheManager_ctor(void* mgr) asm("_ZN20CPremiumLetheManagerC1Ev");
extern "C" void sub_CConditionEventManager_ctor(void* mgr) asm("_ZN22CConditionEventManagerC1Ev");
extern "C" void sub_CSpecialItemRoutingManager_ctor(void* mgr)
    asm("_ZN26CSpecialItemRoutingManagerC1Ev");
extern "C" void sub_CSharedServerMessageManager_ctor(void* mgr, void* info)
    asm("_ZN27CSharedServerMessageManagerC1EP25stSharedServerMessageInfo");
extern "C" void sub_CAuctionAveragePrice_ctor(void* mgr) asm("_ZN20CAuctionAveragePriceC1Ev");
extern "C" void sub_CAuctionAveragePrice_dtor(void* mgr) asm("_ZN20CAuctionAveragePriceD1Ev");
extern "C" void sub_CraneMinigameManager_ctor(void* mgr) asm("_ZN20CraneMinigameManagerC1Ev");
extern "C" void sub_CraneMinigameManager_dtor(void* mgr) asm("_ZN20CraneMinigameManagerD1Ev");
extern "C" char sub_CraneMinigameManager_init(void* mgr) asm("_ZN20CraneMinigameManager4initEv");

// ============================================================================
// StaticPool<T,N> —— ORIG 真实实现（objdump 0x82ae390 起逐函数核对）
// ============================================================================
template <class T, int N>
StaticPool<T, N>::StaticPool()
{
    m_pNode = new CNode[N];
    if (m_pNode == 0)
    {
        cMyTrace tr("StaticPool.cpp", 0x6c, 5);
        tr("[StaticPool] %s(%d) new fail", "StaticPool.cpp", 0x6c);
    }
    for (int i = 0; i < N; ++i)
        m_Queue.push(&m_pNode[i]);
    m_nErrorCode = 0;
}

template <class T, int N>
StaticPool<T, N>::~StaticPool()
{
    delete[] m_pNode;
}

template <class T, int N>
T* StaticPool<T, N>::Acquire()
{
    if (m_Queue.empty())
        return 0;
    CNode* pNode = m_Queue.front();
    m_Queue.pop();
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        cMyTrace tr("StaticPool.cpp", 0x89, 0);
        tr("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 3;
    }
    if (pNode->bUse != 0)
    {
        register int use = pNode->bUse;
        cMyTrace tr("StaticPool.cpp", 0x8f, 0);
        tr("[StaticPool] bUse(%d) node(%p)", use, pNode);
        m_nErrorCode = 4;
    }
    pNode->bUse = pNode->bUse + 1;
    return &pNode->data;
}

template <class T, int N>
T* StaticPool<T, N>::Get(int index)
{
    if (index < 0 || index > N - 1)
        return 0;
    CNode* pNode = &m_pNode[index];
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        cMyTrace tr("StaticPool.cpp", 0xc3, 0);
        tr("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 7;
    }
    if (pNode->bUse != 1)
    {
        if (pNode->bUse != 0)
        {
            register int use = pNode->bUse;
            cMyTrace tr("StaticPool.cpp", 0xca, 0);
            tr("[StaticPool] bUse(%d) node(%p)", use, pNode);
            m_nErrorCode = 8;
        }
    }
    return &pNode->data;
}

template <class T, int N>
void StaticPool<T, N>::Free(T* p)
{
    if (p == 0)
    {
        cMyTrace tr("StaticPool.cpp", 0xa0, 5);
        tr("[StaticPool] %s(%d) free null", "StaticPool.cpp", 0xa0);
    }
    CNode* pNode = (CNode*)p;
    m_Queue.push(pNode);
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        cMyTrace tr("StaticPool.cpp", 0xaa, 0);
        tr("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 5;
    }
    if (pNode->bUse != 1)
    {
        register int use = pNode->bUse;
        cMyTrace tr("StaticPool.cpp", 0xb0, 0);
        tr("[StaticPool] bUse(%d) node(%p)", use, pNode);
        m_nErrorCode = 6;
        PrintBackTrace();
    }
    pNode->bUse = pNode->bUse - 1;
}

template <class T, int N>
int StaticPool<T, N>::GetIndex(T* p)
{
    if (p < (T*)m_pNode)
        return -1;
    int index = (int)((CNode*)p - m_pNode);
    CNode* pNode = (CNode*)p;
    if (pNode->magic != 0x8f21)
    {
        register int magic = pNode->magic;
        cMyTrace tr("StaticPool.cpp", 0x50, 0);
        tr("[StaticPool] magic(%d) node(%p)", magic, pNode);
        m_nErrorCode = 1;
    }
    if (pNode->bUse != 1)
    {
        if (pNode->bUse != 0)
        {
            register int use = pNode->bUse;
            cMyTrace tr("StaticPool.cpp", 0x56, 0);
            tr("[StaticPool] bUse(%d) node(%p)", use, pNode);
            m_nErrorCode = 2;
        }
    }
    if (index > N - 1)
        return -1;
    return index;
}

// ============================================================================
// 池化对象桩 ctor/dtor（权威实现属后续批次；CNode ctor/dtor 需非内联符号）
// ============================================================================
WongWork::CBossTower::CBossTower() { memset(m_pad, 0, sizeof(m_pad)); }
WongWork::CBossTower::~CBossTower() {}

// ---- WongWork::CMailBox ----
WongWork::CMailBox::CMailBox() {}
WongWork::CMailBox::~CMailBox() {}
int WongWork::CMailBox::AddNewMail(const stAddNewMailInput&) { return 0; }
void WongWork::CMailBox::ClearLetterKeepCount() {}
void WongWork::CMailBox::DecLoadedLetterCount() {}
int WongWork::CMailBox::DeleteLetterKeepCount(unsigned int) { return 0; }
int WongWork::CMailBox::FindPackageLoadLack(unsigned int) { return 0; }
int WongWork::CMailBox::GetLastLoadIdx() { return 0; }
int WongWork::CMailBox::GetLastLoadLetterIdx() { return 0; }
int WongWork::CMailBox::GetLetterKeepCount() { return 0; }
int WongWork::CMailBox::GetLoadedLetterCount() { return 0; }
WongWork::CMailBox::CMail* WongWork::CMailBox::GetMail(unsigned int) { return 0; }
int WongWork::CMailBox::GetNotLoadedMailCount() { return 0; }
int WongWork::CMailBox::GetPackageLoadLack(unsigned int*, unsigned int) { return 0; }
int WongWork::CMailBox::GetRecvSize() { return 0; }
int WongWork::CMailBox::GetRemainSize() { return 0; }
void WongWork::CMailBox::IncNotLoadedMailCount() {}
void WongWork::CMailBox::Init() {}
void WongWork::CMailBox::InsertLetterKeepCount(unsigned int) {}
bool WongWork::CMailBox::IsLoaded() { return false; }
int WongWork::CMailBox::RemoveMail(unsigned int) { return 0; }
void WongWork::CMailBox::SetLastLoadLetterIdx(unsigned int) {}
void WongWork::CMailBox::SetLoadState(bool, long) {}
void WongWork::CMailBox::SetLoadedLetterCount(int) {}
void WongWork::CMailBox::SetNotLoadedMailCount(int) {}
void WongWork::CMailBox::SetPackageLoadLack(const unsigned int*, int, std::set<unsigned int>&) {}
int WongWork::CMailBox::getMailLoadCount() { return 0; }
WongWork::CMailBox::CMail* WongWork::CMailBox::getNextMail() { return 0; }
void WongWork::CMailBox::incMailLoadCount() {}
void WongWork::CMailBox::reset() {}
void WongWork::CMailBox::setMailIterator() {}
void WongWork::CDungeonClear::clear() { m_map.clear(); }
advancealtar::StageControl::StageControl() { memset(m_pad, 0, sizeof(m_pad)); }
advancealtar::StageControl::~StageControl() {}
QuickParty::CQuickParty::CQuickParty() { memset(m_pad, 0, sizeof(m_pad)); }
QuickParty::CQuickParty::~CQuickParty() {}

// ============================================================================
// 显式实例化（本 TU 池成员使用）
// ============================================================================
template class StaticPool<CUser, 600>;
template class StaticPool<CTradeSpace, 300>;
template class StaticPool<CParty, 600>;
template class StaticPool<PvP_Room, 600>;
template class StaticPool<WarRoom, 40>;
template class StaticPool<WongWork::CDeathTower, 600>;
template class StaticPool<WongWork::CBossTower, 600>;
template class StaticPool<advancealtar::StageControl, 600>;
template class StaticPool<QuickParty::CQuickParty, 300>;
template class StaticPool<BlueMarble, 300>;
template class StaticPool<CACHE_CHARACTER_TYPE, 1000>;
template class StaticPool<_IO_FILE, 50>;
template class StaticPool<expert_job::CEnchanter, 300>;
template class StaticPool<expert_job::CDisjointer, 600>;
template class StaticPool<pvp_assault::CAssaultPlace, 128>;
template class StaticPool<private_store::CPrivateStore, 300>;
template class StaticPool<exchange_server::CSession, 300>;
template class StaticPool<online_preliminary::COnlinePreliminaryTeam, 600>;
template class StaticPool<WongWork::CMailBox, 600>;
template class StaticPool<WongWork::CMailBox::CMail, 20>;

// DynamicPool 显式实例化
template class DynamicPool<PacketBuf>;
template class DynamicPool<user_creature::CEgg>;
template class DynamicPool<user_creature::CCreature>;
template class DynamicPool<WongWork::Avatar_Item>;
template class DynamicPool<WongWork::IPG::SIPGData>;

// ============================================================================
// CGameManager 实现
// ============================================================================
CGameManager* G_CGameManager()
{
    return GlobalInstance<CGameManager>::inst_ptr();
}

bool CGameManager::init()
{
    void* pMgr = new (std::nothrow) char[0x6c];
    sub_CraneMinigameManager_ctor(pMgr);
    m_pCraneMinigameMgr = (CraneMinigameManager*)pMgr;
    pMgr = (void*)GetCraneMinigameManager();
    return sub_CraneMinigameManager_init(pMgr) == 1;
}

short CGameManager::getNextUID()
{
    m_uid = m_uid + 1;
    short v = m_uid;
    if (v == 0x7fff || v < 0)
        m_uid = 0;
    return v;
}

int CGameManager::GetIdx(CUser* user)
{
    return m_userPool.GetIndex(user);
}

CUser* CGameManager::GetUserByAccId(unsigned int accId)
{
    std::map<unsigned int, CUser*>::iterator it = m_userByAccId.find(accId);
    if (it == m_userByAccId.end())
        return 0;
    CUser* user = it->second;
    if (user->get_acc_id() != accId)
    {
        cMyTrace tr("CUser* CGameManager::GetUserByAccId(memberIdentificationNumber_t)", 0x9ab, 5);
        tr("[%s][%d][IN m_id: %s][Map m_id: %s]",
           "CUser* CGameManager::GetUserByAccId(memberIdentificationNumber_t)", 0x9ab,
           NumberToString(accId, 0), NumberToString(user->get_acc_id(), 1));
        user = 0;
    }
    return user;
}

CGameManager::FindUserByUnique::FindUserByUnique(short uniqueId)
{
    m_uniqueId = uniqueId;
}

bool CGameManager::FindUserByUnique::operator()(
    std::pair<const unsigned int, CUser*>& p) const
{
    return p.second != 0 &&
           (unsigned short)p.second->get_unique_id() == (short)m_uniqueId;
}

CUser* CGameManager::getUserByUnique(short uniqueId)
{
    std::map<unsigned int, CUser*>::iterator it = std::find_if(
        m_userByAccId.begin(), m_userByAccId.end(), FindUserByUnique(uniqueId));
    if (it == m_userByAccId.end())
        return 0;
    return it->second;
}

CUser* CGameManager::getUser(int slotId, int increId)
{
    short slot = (short)((unsigned int)increId >> 0x10);
    short id = (short)increId;
    Guard<Mutex> guard(&m_mutex);
    CUser* user = m_userPool.Get((int)slot);
    if (user == 0)
    {
        cMyTrace tr("CUser* CGameManager::getUser(int, int)", 0x95c, 5);
        tr(" No way! getUser , slot_id(%d) is abnormal!", (int)slot);
        return 0;
    }
    short incre = user->GetIncreID();
    if (incre == 0)
        return 0;
    if (incre != id && incre != 0 && id != 0)
        return 0;
    return user;
}

void CGameManager::insertUserByAccID(CUser* user)
{
    unsigned int accId = user->get_acc_id();
    m_userByAccId[accId] = user;
    unsigned int accId2 = user->get_acc_id();
    if (accId2 == 0)
    {
        cMyTrace tr("void CGameManager::insertUserByAccID(CUser*)", 0x91d, 5);
        tr("[QQID_ERROR]CGameManager::insertUserByAccID  qq_id:%s", user->get_acc_name());
    }
    else
    {
        m_userByAccId2[accId2] = user;
        sub_CLogGameChannel_IncInUser(GlobalData::s_pLogGameChannel);
    }
}

bool CGameManager::isConnectUserByAccId(unsigned int accId)
{
    for (int i = 0; i < 600; ++i)
    {
        CUser* user = m_userPool.Get(i);
        if (user->get_acc_id() == accId)
            return 1;
    }
    return 0;
}

CUser* CGameManager::createUser()
{
    Guard<Mutex> guard(&m_mutex);
    CUser* user = m_userPool.Acquire();
    if (user == 0)
    {
        cMyTrace tr("CUser* CGameManager::createUser()", 0x8e3, 5);
        tr("createUser Failed! need to restart!");
        return 0;
    }
    unsigned int uid = user->GetUID();
    unsigned int accId = user->get_acc_id();
    check_user_var(user);
    int idx = GetIdx(user);
    user->SetSlotIDX((short)idx);
    int err = m_userPool.GetLastErrorCode();
    if (err != 0)
    {
        cMyTrace tr("CUser* CGameManager::createUser()", 0x8f6, 0);
        tr("STATIC MEMORY_POOL createUser error(%d), user(%x), last user m_acc_id(%s), idx(%d), new user idx(%d)",
           err, user, NumberToString(accId, 0), uid, user->GetUID());
    }
    short nxt = getNextUID();
    user->SetIncreID(nxt);
    void* handle = sub_CUserGlobalInfoHandleInstance();
    user->set_unique_id(sub_CUserGlobalInfoHandle_get_uniqueid(handle));
    return user;
}

void CGameManager::check_user_var(CUser* user)
{
    bool b = false;
    if (user->get_charac_guildkey() != 0)
    {
        if (user->get_guild_exp() != 0)
            b = true;
    }
    if (b)
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x92c,
                              "[CHECK_USER_VAR] RESET GUILD EXP ERROR!");
    }
    if (user->get_charac_level_before_dungeon() != 0)
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x92f,
                              "[CHECK_USER_VAR] RESET CHARACTER LEVEL BEFORE DUNGEON ERROR!");
    }
    if (user->CheckInParty())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x932,
                              "[CHECK_USER_VAR] RESET PARTY ERROR!");
    }
    if (user->CheckInTrade())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x935,
                              "[CHECK_USER_VAR] RESET TRADE INDEX ERROR!");
    }
    if (user->CheckInPvp())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x938,
                              "[CHECK_USER_VAR] RESET PVP INDEX ERROR!");
    }
    if (user->CheckInWarRoom())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x93b,
                              "[CHECK_USER_VAR] RESET WARROOM INDEX ERROR!");
    }
    if (user->getPlayingDungeonIndex() != -1)
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x93e,
                              "[CHECK_USER_VAR] RESET DUNGEON ERROR!");
    }
    if (user->IsProgLogout())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x941,
                              "[CHECK_USER_VAR] RESET PROG LOGOUT ERROR!");
    }
    if (sub_CCharacterView_isSaveCharacView((char*)user + 0x796f4))
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x944,
                              "[CHECK_USER_VAR] RESET CHARACTER VIEW ERROR!");
    }
    if (user->checkInBlueMarble())
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::check_user_var(CUser*)", 0x948,
                              "[CHECK_USER_VAR] RESET BLUEMARBLE INDEX ERROR!");
    }
}

void CGameManager::returnUserPool(CUser* user)
{
    if (user == 0)
        return;
    unsigned int accId = user->get_acc_id();
    std::map<unsigned int, CUser*>::iterator it = m_userByAccId.find(accId);
    if (it != m_userByAccId.end())
    {
        m_userByAccId.erase(it);
        sub_CLogGameChannel_IncOutUser(GlobalData::s_pLogGameChannel);
    }
    unsigned int accId2 = user->get_acc_id();
    std::map<unsigned int, CUser*>::iterator it2 = m_userByAccId2.find(accId2);
    if (it2 != m_userByAccId2.end())
    {
        m_userByAccId2.erase(it2);
    }
    unsigned int uid = user->GetUID();
    unsigned int accId3 = user->get_acc_id();
    sub_CUser_log_out(user);
    Guard<Mutex> guard(&m_mutex);
    m_userPool.Free(user);
    int err = m_userPool.GetLastErrorCode();
    if (err != 0)
    {
        cMyTrace tr("void CGameManager::returnUserPool(CUser*)", 0xa0e, 0);
        tr("STATIC MEMORY_POOL returnUserPool error(%d), user(%x), last user m_acc_id(%s), idx(%d)",
           err, user, NumberToString(accId3, 0), uid);
    }
}

// ============================================================================
// 队伍
// ============================================================================
CParty* CGameManager::GetParty()
{
    CParty* party = m_partyPool.Acquire();
    if (party == 0)
        return 0;
    int idx = m_partyPool.GetIndex(party);
    party->SetIDX(idx);
    int partyIndex = party->GetPartyIndex();
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it = m_partyMap.find(partyIndex);
    if (it == m_partyMap.end())
    {
        int key = party->GetPartyIndex();
        m_partyMap[key] = party;
    }
    else
    {
        LogManager::logFormat(1, "App.cpp", "CParty* CGameManager::GetParty()", 0xa5f,
                              "party index(%d) already alloc", party->GetPartyIndex());
    }
    return party;
}

CParty* CGameManager::GetParty(int index)
{
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it = m_partyMap.find(index);
    if (it == m_partyMap.end())
        return 0;
    return it->second;
}

int CGameManager::GetPartyIdx(CParty* party)
{
    return m_partyPool.GetIndex(party);
}

void CGameManager::PutParty(CParty* party)
{
    if (party == 0)
        return;
    int key = party->GetPartyIndex();
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it = m_partyMap.find(key);
    if (it != m_partyMap.end())
    {
        int eraseKey = party->GetPartyIndex();
        m_partyMap.erase(eraseKey);
        m_partyPool.Free(party);
    }
}

void CGameManager::CheckOutParty(CUser* user, bool flag)
{
    if (user->CheckInParty() != 1)
        return;
    user->GetPartyIndex();
    CParty* party = GetParty();
    if (party == 0)
        return;
    bool bVar1 = false;
    void* dungeon = *(void**)((char*)party + 0xcac);
    if (dungeon != 0)
    {
        bVar1 = *(char*)((char*)dungeon + 0x89f) > 0;
        if (sub_CDungeon_isTowerOfDespairDungeon(dungeon))
            bVar1 = true;
    }
    if (dungeon != 0)
    {
        char c = sub_CDungeon_get_dimension_possible(dungeon);
        if (c < 1 && *(char*)((char*)dungeon + 0x89c) == 0 &&
            !sub_CDungeon_isTournamentDungeon(dungeon))
            bVar1 = bVar1 || false;
        else
            bVar1 = true;
    }
    sub_CDungeonClearTracer_Trace((char*)party + 0xc7c,
                                  "void CGameManager::CheckOutParty(CUser*, bool)");
    if (*(int*)((char*)party + 0xcd8) != 1 && party->get_state() == 2)
    {
        bool bVar2 = false;
        if (dungeon != 0 && *(char*)((char*)dungeon + 0x85c) != 0)
            bVar2 = true;
        bool bVar3 = false;
        if (dungeon != 0 && *(char*)((char*)dungeon + 0x87a) != 0)
            bVar3 = true;
        if (!bVar1 && bVar2 && bVar3)
            sub_CUser_giveup_panalty(user);
    }
    sub_CParty_leave_user(party, user, 2);
}

unsigned int CGameManager::CheckOutQuickParty(CParty* party, bool flag)
{
    unsigned int ret = 0;
    if (party != 0)
    {
        void* mgr = (void*)GetQuickPartySystemManager();
        ret = (unsigned int)sub_CQuickPartySystemManager_cancel_quick_party(mgr, party, flag, 0);
        ret = ret ^ 1;
    }
    return ret;
}

// ============================================================================
// 交易
// ============================================================================
CTradeSpace* CGameManager::GetTradeSpace()
{
    CTradeSpace* tradeSpace = m_tradeSpacePool.Acquire();
    if (tradeSpace == 0)
        return 0;
    int idx = m_tradeSpacePool.GetIndex(tradeSpace);
    sub_CTradeSpace_SetIDX(tradeSpace, idx);
    return tradeSpace;
}

CTradeSpace* CGameManager::GetTradeSpace(int index)
{
    CTradeSpace* tradeSpace = m_tradeSpacePool.Get(index);
    if (tradeSpace == 0)
        return 0;
    return tradeSpace;
}

int CGameManager::GetTradeIdx(CTradeSpace* tradeSpace)
{
    return m_tradeSpacePool.GetIndex(tradeSpace);
}

void CGameManager::PutTradeSpace(CTradeSpace* tradeSpace)
{
    if (tradeSpace == 0)
        return;
    sub_CTradeSpace_reset(tradeSpace);
    m_tradeSpacePool.Free(tradeSpace);
}

void CGameManager::CheckOutTrade(CUser* user)
{
    if (user->CheckInTrade() != 1)
        return;
    int idx = user->GetTradeSpace();
    CTradeSpace* tradeSpace = m_tradeSpacePool.Get(idx);
    sub_CTradeSpace_cancel_trade_by_dis(tradeSpace, user);
    PutTradeSpace(tradeSpace);
}

// ============================================================================
// PvP
// ============================================================================
PvP_Room* CGameManager::GetPvp()
{
    PvP_Room* room = m_pvpRoomPool.Acquire();
    if (room == 0)
        return 0;
    int idx = m_pvpRoomPool.GetIndex(room);
    sub_PvP_Room_SetIDX(room, idx);
    int roomIndex = sub_PvP_Room_get_index(room);
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.find(roomIndex);
    if (it == m_pvpRoomMap.end())
    {
        {
            CSwitchLog log("PvP_Room* CGameManager::GetPvp()", 0xaa9, 0, 0);
            log("pvp@log room(%d) new alloc", sub_PvP_Room_get_index(room));
        }
        int key = sub_PvP_Room_get_index(room);
        m_pvpRoomMap[key] = room;
    }
    else
    {
        {
            CSwitchLog log("PvP_Room* CGameManager::GetPvp()", 0xab2, 0, 0);
            log("pvp@log room(%d) already alloc", sub_PvP_Room_get_index(room));
        }
        room = 0;
    }
    return room;
}

PvP_Room* CGameManager::GetPvp(int index, CUser* user, int param)
{
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.find(index);
    if (it == m_pvpRoomMap.end())
        return 0;
    const char* name;
    const char* accName;
    if (user == 0)
    {
        name = 0;
        accName = 0;
    }
    else
    {
        name = user->getCurCharacName();
        accName = user->get_acc_name();
    }
    {
        CSwitchLog log("PvP_Room* CGameManager::GetPvp(int, CUser*, int)", 0xacc, 0, 0);
        log("pvp@log %s,%s, room(%d), %d",
            accName, name, sub_PvP_Room_get_index(it->second), param);
    }
    return it->second;
}

int CGameManager::GetPvpIdx(PvP_Room* room)
{
    return m_pvpRoomPool.GetIndex(room);
}

void CGameManager::PutPvp(PvP_Room* room)
{
    if (room == 0)
        return;
    {
        CSwitchLog log("void CGameManager::PutPvp(PvP_Room*)", 0xae4, 0, 0);
        log("pvp@log room(%d)", sub_PvP_Room_get_index(room));
    }
    int key = sub_PvP_Room_get_index(room);
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.find(key);
    if (it != m_pvpRoomMap.end())
    {
        int eraseKey = sub_PvP_Room_get_index(room);
        m_pvpRoomMap.erase(eraseKey);
        m_pvpRoomPool.Free(room);
    }
}

void CGameManager::CheckOutPvp(CUser* user, bool flag)
{
    if (user->CheckInPvp() != 1)
        return;
    short pvpIdx = user->GetPvpIndex();
    PvP_Room* room = GetPvp((int)pvpIdx, user, 0);
    if (room == 0)
        return;
    PacketGuard guard;
    bool outFlag = false;
    int ret = sub_PvP_Room_leave_room(room, user, &outFlag);
    {
        CSwitchLog log("void CGameManager::CheckOutPvp(CUser*, bool)", 0x11a1, 0, 0);
        log("pvp@log %s,%s, room(%d),%d,%d",
            user->get_acc_name(), user->getCurCharacName(),
            sub_PvP_Room_get_index(room), sub_PvP_Room_get_pvp_battle_mode(room), outFlag);
    }
    if (ret < 0)
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::CheckOutPvp(CUser*, bool)", 0x11bd,
                              "pvp@log room(%d) user(%s) leave fail",
                              sub_PvP_Room_get_index(room),
                              NumberToString(user->get_acc_id(), 0));
    }
    else
    {
        guard.clear();
        sub_PvP_Room_make_seat_info(room, &guard, ret);
        sub_GameWorld_send_all(sub_G_GameWorld(), &guard);
        if (outFlag)
        {
            guard.clear();
            sub_PvP_Room_make_state_info(room, &guard);
            sub_GameWorld_send_all(sub_G_GameWorld(), &guard);
        }
        if (sub_PvP_Room_get_waiter_count(room) == 0)
        {
            sub_PvP_Room_destroy_room(room, 0);
            PutPvp(room);
            guard.clear();
            sub_PvP_Room_make_state_info(room, &guard);
            sub_GameWorld_send_all(sub_G_GameWorld(), &guard);
        }
    }
    if (!flag && sub_PvP_Room_get_recv_pvp_rank_count(room) > 0)
    {
        if (sub_PvP_Room_IsInsertTimerRecvPvpRank(room) != 1 &&
            sub_PvP_Room_IsEndPvpBattle(room) != 1)
        {
            sub_PvP_Room_send_pvp_end(room);
        }
    }
}

PvP_Room* CGameManager::FindQuickJoinablePvPRoom(bool quick, CUser* user)
{
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.begin();
    for (;;)
    {
        if (it == m_pvpRoomMap.end())
            return 0;
        PvP_Room* room = it->second;
        if (room != 0 && sub_PvP_Room_get_pvp_battle_mode(room) != 6)
        {
            bool ok = true;
            if ((quick && sub_PvP_Room_IsExistPassword(room) == 1) ||
                sub_PvP_Room_IsQuickJoinablePVPRoom(room, user) == 0)
                ok = false;
            if (ok)
                return room;
        }
        ++it;
    }
}

// ============================================================================
// 快速队伍
// ============================================================================
QuickParty::CQuickParty* CGameManager::GetQuickParty()
{
    QuickParty::CQuickParty* qp = m_quickPartyPool.Acquire();
    if (qp == 0)
        return 0;
    int idx = m_quickPartyPool.GetIndex(qp);
    sub_CQuickParty_set_quick_party_index(qp, idx);
    int qpIndex = sub_CQuickParty_get_quick_party_index(qp);
    __gnu_cxx::hash_map<int, QuickParty::CQuickParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<QuickParty::CQuickParty*> >::iterator it =
            m_quickPartyMap.find(qpIndex);
    if (it == m_quickPartyMap.end())
    {
        int key = sub_CQuickParty_get_quick_party_index(qp);
        m_quickPartyMap[key] = qp;
    }
    else
    {
        LogManager::logFormat(1, "App.cpp",
                              "QuickParty::CQuickParty* CGameManager::GetQuickParty()", 0xb64,
                              "quick party index(%d) already alloc",
                              sub_CQuickParty_get_quick_party_index(qp));
    }
    return qp;
}

QuickParty::CQuickParty* CGameManager::GetQuickParty(int index)
{
    __gnu_cxx::hash_map<int, QuickParty::CQuickParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<QuickParty::CQuickParty*> >::iterator it =
            m_quickPartyMap.find(index);
    if (it == m_quickPartyMap.end())
        return 0;
    return it->second;
}

int CGameManager::GetQuickPartyIdx(QuickParty::CQuickParty* qp)
{
    return m_quickPartyPool.GetIndex(qp);
}

void CGameManager::PutQuickParty(QuickParty::CQuickParty* qp)
{
    if (qp == 0)
        return;
    int key = sub_CQuickParty_get_quick_party_index(qp);
    __gnu_cxx::hash_map<int, QuickParty::CQuickParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<QuickParty::CQuickParty*> >::iterator it =
            m_quickPartyMap.find(key);
    if (it != m_quickPartyMap.end())
    {
        int eraseKey = sub_CQuickParty_get_quick_party_index(qp);
        m_quickPartyMap.erase(eraseKey);
        m_quickPartyPool.Free(qp);
        sub_CQuickParty_init(qp);
    }
}

// ============================================================================
// 公会战房间
// ============================================================================
WarRoom* CGameManager::GetWarRoom()
{
    WarRoom* room = m_warRoomPool.Acquire();
    if (room == 0)
        return 0;
    int idx = m_warRoomPool.GetIndex(room);
    sub_WarRoom_SetIndex(room, idx);
    int roomIndex = sub_WarRoom_GetIndex(room);
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.find(roomIndex);
    if (it == m_warRoomMap.end())
    {
        int key = sub_WarRoom_GetIndex(room);
        m_warRoomMap[key] = room;
    }
    else
    {
        room = 0;
    }
    return room;
}

WarRoom* CGameManager::GetWarRoom(int index)
{
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.find(index);
    if (it == m_warRoomMap.end())
        return 0;
    return it->second;
}

int CGameManager::GetWarRoomIdx(WarRoom* room)
{
    return m_warRoomPool.GetIndex(room);
}

void CGameManager::PutWarRoom(WarRoom* room)
{
    if (room == 0)
        return;
    int key = sub_WarRoom_GetIndex(room);
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.find(key);
    if (it != m_warRoomMap.end())
    {
        int eraseKey = sub_WarRoom_GetIndex(room);
        m_warRoomMap.erase(eraseKey);
        m_warRoomPool.Free(room);
    }
}

void CGameManager::CheckOutWarRoom(CUser* user)
{
    if (user->CheckInWarRoom() != 1)
        return;
    user->GetWarRoomIndex();
    WarRoom* room = GetWarRoom();
    if (room == 0)
        return;
    int ret = sub_WarRoom_WalkOutUser(room, user);
    if (ret == 4)
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::CheckOutWarRoom(CUser*)", 0x117b,
                              "CGameManager::CheckOutWarRoom Can't find user charno(%d)",
                              user->get_charac_no(-1));
    }
    sub_WarRoom_CheckState(room);
}

WarRoom* CGameManager::FindJoinableWarRoom(CUser* user)
{
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.begin();
    for (;;)
    {
        if (it == m_warRoomMap.end())
            return 0;
        WarRoom* room = it->second;
        if (room != 0 && sub_WarRoom_IsJoinable(room, user) == 0)
            return room;
        ++it;
    }
}

// ============================================================================
// 死亡塔 / 领主塔 / 前置祭坛 / 蓝弹珠
// ============================================================================
WongWork::CDeathTower* CGameManager::getDeathTower()
{
    WongWork::CDeathTower* tower = m_deathTowerPool.Acquire();
    if (tower == 0)
        return 0;
    int idx = m_deathTowerPool.GetIndex(tower);
    sub_CDeathTower_setIdx(tower, idx);
    int towerIdx = sub_CDeathTower_getIdx(tower);
    __gnu_cxx::hash_map<int, WongWork::CDeathTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CDeathTower*> >::iterator it =
            m_deathTowerMap.find(towerIdx);
    if (it == m_deathTowerMap.end())
    {
        int key = sub_CDeathTower_getIdx(tower);
        m_deathTowerMap[key] = tower;
        sub_CDeathTower_reset(tower);
    }
    else
    {
        tower = 0;
    }
    return tower;
}

WongWork::CDeathTower* CGameManager::getDeathTower(int index)
{
    __gnu_cxx::hash_map<int, WongWork::CDeathTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CDeathTower*> >::iterator it =
            m_deathTowerMap.find(index);
    if (it == m_deathTowerMap.end())
        return 0;
    return it->second;
}

int CGameManager::getDeathTowerIdx(WongWork::CDeathTower* tower)
{
    return m_deathTowerPool.GetIndex(tower);
}

void CGameManager::returnDeathTower(WongWork::CDeathTower* tower)
{
    if (tower == 0)
        return;
    int key = sub_CDeathTower_getIdx(tower);
    __gnu_cxx::hash_map<int, WongWork::CDeathTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CDeathTower*> >::iterator it =
            m_deathTowerMap.find(key);
    if (it != m_deathTowerMap.end())
    {
        int eraseKey = sub_CDeathTower_getIdx(tower);
        m_deathTowerMap.erase(eraseKey);
        m_deathTowerPool.Free(tower);
    }
}

void CGameManager::checkOutDeathTower(CUser* user)
{
    if (sub_CUser_checkInDeathTower(user) != 1)
        return;
    user->getDeathTowerIndex();
    WongWork::CDeathTower* tower = getDeathTower();
    if (tower != 0 &&
        sub_CDeathTower_onLeaveUser(tower, user) != 1)
    {
        LogManager::logFormat(1, "App.cpp", "void CGameManager::checkOutDeathTower(CUser*)", 0x123a,
                              "DeathTower checkOutDeathTower ID[%d] ACCID[%s]",
                              sub_CDeathTower_getIdx(tower),
                              NumberToString(user->get_acc_id(), 0));
    }
}

WongWork::CBossTower* CGameManager::getBossTower()
{
    WongWork::CBossTower* tower = m_bossTowerPool.Acquire();
    if (tower == 0)
        return 0;
    int idx = m_bossTowerPool.GetIndex(tower);
    sub_CBossTower_setIdx(tower, idx);
    int towerIdx = sub_CBossTower_getIdx(tower);
    __gnu_cxx::hash_map<int, WongWork::CBossTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CBossTower*> >::iterator it =
            m_bossTowerMap.find(towerIdx);
    if (it == m_bossTowerMap.end())
    {
        int key = sub_CBossTower_getIdx(tower);
        m_bossTowerMap[key] = tower;
        sub_CBossStage_reset(tower);
    }
    else
    {
        tower = 0;
    }
    return tower;
}

WongWork::CBossTower* CGameManager::getBossTower(int index)
{
    __gnu_cxx::hash_map<int, WongWork::CBossTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CBossTower*> >::iterator it =
            m_bossTowerMap.find(index);
    if (it == m_bossTowerMap.end())
        return 0;
    return it->second;
}

int CGameManager::getBossTowerIdx(WongWork::CBossTower* tower)
{
    return m_bossTowerPool.GetIndex(tower);
}

void CGameManager::returnBossTower(WongWork::CBossTower* tower)
{
    if (tower == 0)
        return;
    int key = sub_CBossTower_getIdx(tower);
    __gnu_cxx::hash_map<int, WongWork::CBossTower*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WongWork::CBossTower*> >::iterator it =
            m_bossTowerMap.find(key);
    if (it != m_bossTowerMap.end())
    {
        int eraseKey = sub_CBossTower_getIdx(tower);
        m_bossTowerMap.erase(eraseKey);
        m_bossTowerPool.Free(tower);
    }
}

void CGameManager::checkOutBossTower(CUser* user)
{
    if (user->checkInBossTower() != 1)
        return;
    user->getBossTowerIndex();
    WongWork::CBossTower* tower = getBossTower();
    if (tower != 0)
    {
        typedef char (*LeaveFn)(void*, void*);
        LeaveFn leave = *(LeaveFn*)(*(void**)tower + 0x14);
        if (leave(tower, user) != 1)
        {
            LogManager::logFormat(1, "App.cpp", "void CGameManager::checkOutBossTower(CUser*)", 0x11d9,
                                  "BossTower checkOutBossTower ID[%d] ACCID[%s]",
                                  sub_CBossTower_getIdx(tower),
                                  NumberToString(user->get_acc_id(), 0));
        }
    }
}

advancealtar::StageControl* CGameManager::getAdvanceAltar()
{
    advancealtar::StageControl* control = m_stagePool.Acquire();
    if (control == 0)
        return 0;
    int idx = m_stagePool.GetIndex(control);
    sub_StageControl_setIndex(control, idx);
    int ctrlIdx = sub_StageControl_getIndex(control);
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.find(ctrlIdx);
    if (it == m_stageMap.end())
    {
        int key = sub_StageControl_getIndex(control);
        m_stageMap[key] = control;
        sub_StageControl_reset(control);
    }
    else
    {
        control = 0;
    }
    return control;
}

advancealtar::StageControl* CGameManager::getAdvanceAltar(int index)
{
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.find(index);
    if (it == m_stageMap.end())
        return 0;
    return it->second;
}

int CGameManager::getAdvanceAltarIndex(advancealtar::StageControl* control)
{
    return m_stagePool.GetIndex(control);
}

void CGameManager::returnAdvanceAltar(advancealtar::StageControl* control)
{
    if (control == 0)
        return;
    int key = sub_StageControl_getIndex(control);
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.find(key);
    if (it != m_stageMap.end())
    {
        int eraseKey = sub_StageControl_getIndex(control);
        m_stageMap.erase(eraseKey);
        m_stagePool.Free(control);
    }
}

void CGameManager::checkOutAdvanceAltar(CUser* user)
{
    if (user != 0 && user->checkInAdvanceAltar() == 1)
    {
        user->getAdvanceAltarIndex();
        advancealtar::StageControl* control = getAdvanceAltar();
        if (control != 0 && sub_StageControl_leaveUser(control) != 1)
        {
            LogManager::logFormat(1, "App.cpp",
                                  "void CGameManager::checkOutAdvanceAltar(CUser*)", 0x11ef,
                                  "invalid user : advance altar logout ID[%d] ACCID[%s]",
                                  sub_StageControl_getIndex(control),
                                  NumberToString(user->get_acc_id(), 0));
        }
    }
}

void CGameManager::onTimeAdvanceAltar()
{
    __gnu_cxx::hash_map<int, advancealtar::StageControl*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<advancealtar::StageControl*> >::iterator it =
            m_stageMap.begin();
    while (it != m_stageMap.end())
    {
        advancealtar::StageControl* control = it->second;
        if (sub_StageControl_onTimerStageTick(control) == 1)
        {
            ++it;
        }
        else
        {
            __gnu_cxx::hash_map<int, advancealtar::StageControl*,
                __gnu_cxx::hash<int>, std::equal_to<int>,
                std::allocator<advancealtar::StageControl*> >::iterator eraseIt = it;
            ++it;
            m_stageMap.erase(eraseIt);
            m_stagePool.Free(control);
        }
    }
}

BlueMarble* CGameManager::getBlueMarble()
{
    BlueMarble* marble = m_blueMarblePool.Acquire();
    if (marble == 0)
        return 0;
    int idx = m_blueMarblePool.GetIndex(marble);
    sub_BlueMarble_setMemoryPoolIndex(marble, idx);
    int marbleIdx = sub_BlueMarble_getMemoryPoolIndex(marble);
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it =
            m_blueMarbleMap.find(marbleIdx);
    if (it == m_blueMarbleMap.end())
    {
        int key = sub_BlueMarble_getMemoryPoolIndex(marble);
        m_blueMarbleMap[key] = marble;
    }
    else
    {
        marble = 0;
    }
    return marble;
}

BlueMarble* CGameManager::getBlueMarble(int index)
{
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it =
            m_blueMarbleMap.find(index);
    if (it == m_blueMarbleMap.end())
        return 0;
    return it->second;
}

int CGameManager::getBlueMarbleIdx(BlueMarble* marble)
{
    return m_blueMarblePool.GetIndex(marble);
}

void CGameManager::putBlueMarble(BlueMarble* marble)
{
    if (marble == 0)
        return;
    int key = sub_BlueMarble_getMemoryPoolIndex(marble);
    __gnu_cxx::hash_map<int, BlueMarble*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<BlueMarble*> >::iterator it =
            m_blueMarbleMap.find(key);
    if (it != m_blueMarbleMap.end())
    {
        int eraseKey = sub_BlueMarble_getMemoryPoolIndex(marble);
        m_blueMarbleMap.erase(eraseKey);
        m_blueMarblePool.Free(marble);
    }
}

void CGameManager::checkOutBlueMarble(CUser* user)
{
    if (user->checkInBlueMarble() != 1)
        return;
    user->getBlueMarbleIndex();
    BlueMarble* marble = getBlueMarble();
    if (marble != 0)
    {
        sub_BlueMarble_leaveUser(marble, user);
    }
}

void CGameManager::allocBlueMarble()
{
    void* dm = sub_G_CDataManager();
    void* script = sub_CDataManager_getBlueMarbleScript(dm);
    for (int i = 0; i < 300; ++i)
    {
        BlueMarble* marble = getBlueMarble();
        sub_BlueMarble_getMemoryPoolIndex(marble);
        for (int j = 0; j < *(int*)((char*)script + 0x30); ++j)
        {
            sub_BlueMarble_setUserInfo(marble, script);
        }
        sub_BlueMarble_setScript(marble, script);
    }
}

BlueMarble* CGameManager::findJoinableBlueMarble()
{
    void* dm = sub_G_CDataManager();
    void* script = sub_CDataManager_getBlueMarbleScript(dm);
    if (script == 0)
        return 0;
    int count = *(int*)((char*)script + 0x30);
    BlueMarble* result = 0;
    for (int n = 0; n < count; ++n)
    {
        __gnu_cxx::hash_map<int, BlueMarble*,
            __gnu_cxx::hash<int>, std::equal_to<int>,
            std::allocator<BlueMarble*> >::iterator it =
                m_blueMarbleMap.begin();
        for (; it != m_blueMarbleMap.end(); ++it)
        {
            BlueMarble* marble = it->second;
            if (marble != 0)
            {
                if (sub_BlueMarble_checkJoin(marble) &&
                    sub_BlueMarble_getWaitUserCount(marble) == count - 1)
                    return marble;
            }
        }
    }
    return 0;
}

// ============================================================================
// 列表发送
// ============================================================================
void CGameManager::SendPartyList(CUser* user)
{
    PacketGuard guard;
    guard.put_header(0, 9);
    int count = 0;
    int index = guard.get_index();
    guard.put_short((int)m_partyMap.size());
    char type = 0;
    __gnu_cxx::hash_map<int, CParty*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<CParty*> >::iterator it = m_partyMap.begin();
    while (it != m_partyMap.end())
    {
        CParty* party = it->second;
        if (party != 0 && !party->IsAutoCreated())
        {
            count = count + 1;
            guard.put_short(party->GetPartyIndex());
            guard.put_byte((int)type);
            if (type == 0 || type == 1)
            {
                guard.put_byte(party->getTitleIndex() & 0xff);
                if (party->getTitleIndex() == 0)
                {
                    guard.put_int((int)strlen(party->getTitle()));
                    guard.put_str(party->getTitle(), (int)strlen(party->getTitle()));
                }
                guard.put_byte(sub_CParty_IsReturnUserParty(party) ? 1 : 0);
                guard.put_byte(party->getUserMax() & 0xff);
                guard.put_short(party->getDungIndex() & 0xffff);
                guard.put_byte(party->getDungDiffi() & 0xff);
                guard.put_byte(sub_CParty_IsEventCharacParty(party) ? 1 : 0);
            }
            if (type == 0 || type == 2)
            {
                int managerIdx = 0;
                for (int i = 0; i < 4; ++i)
                {
                    CUser* member = party->get_user(i);
                    if (member == 0)
                    {
                        guard.put_short(0xffff);
                        guard.put_byte(0xff);
                    }
                    else
                    {
                        guard.put_short(member->get_unique_id() & 0xffff);
                        if (sub_CParty_getManager(party) == member)
                            managerIdx = i;
                        char sex = sub_CUser_getSex(member);
                        if (sex == -1)
                            sex = 1;
                        guard.put_byte((int)sex);
                    }
                }
                guard.put_byte(sub_CParty_IsReturnUserParty(party) ? 1 : 0);
                guard.put_byte(managerIdx);
                guard.put_byte(party->is_quick_party() & 0xff);
                guard.put_byte(sub_CParty_IsEventCharacParty(party) ? 1 : 0);
            }
            if (type == 0 || type == 1 || type == 2)
            {
                char matching = sub_GameWorld_is_dungeon_tag_matching_channel(
                    sub_G_GameWorld(), "[impossible]");
                guard.put_byte((int)matching);
                if (matching != 0)
                {
                    for (int i = 0; i < 4; ++i)
                    {
                        CUser* member = party->get_user(i);
                        if (member == 0)
                        {
                            guard.put_byte(0);
                        }
                        else
                        {
                            guard.put_byte(6);
                            for (int j = 0; j < 6; ++j)
                            {
                                char dim = sub_CDataManager_get_dimensionInout(
                                    sub_G_CDataManager(), j);
                                guard.put_byte((int)dim);
                                const void* characR = member->getCurCharacR();
                                guard.put_byte(*(const char*)((const char*)characR + 0xeb9 + j));
                            }
                        }
                    }
                }
            }
        }
        ++it;
    }
    guard.put_short(index, count);
    guard.finalize(true);
    user->Send(guard);
}

void CGameManager::SendPvpList(CUser* user)
{
    PacketGuard guard;
    guard.put_header(0, 0x29);
    guard.put_short((int)m_pvpRoomMap.size());
    __gnu_cxx::hash_map<int, PvP_Room*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<PvP_Room*> >::iterator it = m_pvpRoomMap.begin();
    while (it != m_pvpRoomMap.end())
    {
        PvP_Room* room = it->second;
        if (room != 0)
        {
            if (sub_PvP_Room_get_waiter_count(room) > 0)
            {
                sub_PvP_Room_make_room_info(room, &guard);
            }
        }
        ++it;
    }
    guard.finalize(true);
    user->Send(guard);
}

void CGameManager::SendWarRoomList(CUser* user)
{
    PacketGuard guard;
    guard.put_header(0, 0x57);
    int index = guard.get_index();
    guard.put_short(0);
    int count = 0;
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.begin();
    while (it != m_warRoomMap.end())
    {
        WarRoom* room = it->second;
        if (room != 0)
        {
            sub_WarRoom_MakeRoomInfo(room, &guard);
            count = count + 1;
        }
        ++it;
    }
    guard.put_short(index, count);
    guard.finalize(true);
    user->Send(guard);
}

void CGameManager::SendWarRoomToAll(int index)
{
    PacketGuard guard;
    guard.put_header(0, 0x57);
    guard.put_short(1);
    __gnu_cxx::hash_map<int, WarRoom*,
        __gnu_cxx::hash<int>, std::equal_to<int>,
        std::allocator<WarRoom*> >::iterator it = m_warRoomMap.find(index);
    if (it == m_warRoomMap.end())
        return;
    WarRoom* room = it->second;
    sub_WarRoom_MakeRoomInfo(room, &guard);
    guard.finalize(true);
    sub_GameWorld_send_all(sub_G_GameWorld(), &guard);
}

void CGameManager::PrintWarRoomList()
{
}

// ============================================================================
// WarRoom 分配 / 计数管理
// ============================================================================
void CGameManager::WarRoomAlloc()
{
    void* dm = sub_G_CDataManager();
    int hourTable = sub_WarAreaCounter_GetCurrenTimeTable((char*)dm + 0x87b4);
    for (int area = 0; area < 10; ++area)
    {
        int count = 0;
        for (;;)
        {
            void* dm2 = sub_G_CDataManager();
            int need = sub_WarAreaCounter_GetWarRoomCountAtPeekTime(
                (char*)dm2 + 0x87b4, area);
            if (need <= count)
                break;
            WarRoom* room = GetWarRoom();
            *(char*)room = (char)area;
            *((char*)room + 1) = (char)count;
            sub_WarRoom_Prepare(room);
            void* dm3 = sub_G_CDataManager();
            if (count < *(int*)((char*)dm3 + 8 + (hourTable * 10 + area + 0x223c) * 4))
                sub_WarRoom_SetState(room, 0);
            else
                sub_WarRoom_SetState(room, -1);
            count = count + 1;
        }
    }
    PrintWarRoomList();
}

void CGameManager::WarRoomCountAdjustByChannelInfo()
{
    void* dm = sub_G_CDataManager();
    void* script = sub_CDataManager_GetChannelScript(dm);
    void* env = sub_G_CEnvironment();
    unsigned int channelNo = ((CEnvironment*)env)->get_channel_no();
    int info = sub_channel_script_t_getChannelInfo(
        script, *(unsigned char*)((char*)env + 0x378), channelNo);
    for (int a = 0; a < 10; ++a)
    {
        for (int b = 0; b < 0x18; ++b)
        {
            void* dm2 = sub_G_CDataManager();
            float f = *(float*)((char*)info + 8 + (a + 4) * 4);
            void* dm3 = sub_G_CDataManager();
            *(int*)((char*)dm2 + 8 + (b * 10 + a + 0x223c) * 4) =
                (int)(*(float*)((char*)dm3 + 4 + (b + 0x21ec) * 4) * f);
        }
    }
}

void CGameManager::WarRoomCountManage(int hourIndex)
{
    cMyTrace tr0("void CGameManager::WarRoomCountManage(int)", 0x306f, 0);
    tr0("WarRoomCountManage Start");
    int current[10];
    for (int i = 0; i < 10; ++i)
        current[i] = 0;
    int total = 0;
    for (int area = 0; area < 10; ++area)
    {
        int n = 0;
        for (;;)
        {
            void* dm = sub_G_CDataManager();
            int need = sub_WarAreaCounter_GetWarRoomCountAtPeekTime(
                (char*)dm + 0x87b4, area);
            if (need <= n)
                break;
            total = total + 1;
            WarRoom* room = GetWarRoom();
            if (room == 0)
            {
                LogManager::logFormat(1, "App.cpp",
                                      "void CGameManager::WarRoomCountManage(int)", 0x307d,
                                      "WarRoomCountManage Error : WarRoom does not exist %d", total);
            }
            else
            {
                if (sub_WarRoom_GetState(room) != -1)
                    current[area] = current[area] + 1;
            }
            n = n + 1;
        }
    }
    int hourTable;
    if (hourIndex == -1)
    {
        void* dm = sub_G_CDataManager();
        hourTable = sub_WarAreaCounter_GetCurrenTimeTable((char*)dm + 0x87b4);
    }
    else
    {
        hourTable = hourIndex;
    }
    cMyTrace tr1("void CGameManager::WarRoomCountManage(int)", 0x3088, 0);
    tr1("WarRoomCountManage currentHourIndex %d", hourTable);
    PacketGuard guard;
    guard.put_header(0, 0x58);
    int index = guard.get_index();
    guard.put_short(0);
    int totalDiff = 0;
    int area = 0;
    for (;;)
    {
        if (area > 9)
        {
            if (totalDiff != 0)
            {
                guard.put_short(index, totalDiff);
                guard.finalize(true);
                sub_GameWorld_send_all(sub_G_GameWorld(), &guard);
                PrintWarRoomList();
                cMyTrace tr3("void CGameManager::WarRoomCountManage(int)", 0x30cf, 0);
                tr3("WarRoomCountManage end");
            }
            return;
        }
        void* dm = sub_G_CDataManager();
        int diff = *(int*)((char*)dm + 8 + (hourTable * 10 + area + 0x223c) * 4) -
                   current[area];
        cMyTrace tr2("void CGameManager::WarRoomCountManage(int)", 0x3097, 0);
        tr2("WarRoomCountManage diff(%d) currentWarRoomCount(lev%d)", diff, current[area]);
        if (diff < 0)
        {
            int need = diff < 0 ? -diff : diff;
            void* dm2 = sub_G_CDataManager();
            int lastIdx = sub_WarAreaCounter_GetWarRoomCountLastIndex(
                (char*)dm2 + 0x87b4, area);
            while (need != 0)
            {
                lastIdx = lastIdx - 1;
                WarRoom* room = G_CGameManager()->GetWarRoom();
                if (room == 0)
                    break;
                if (room == 0 || sub_WarRoom_GetState(room) == -1)
                    ;
                else
                {
                    char shut = sub_WarRoom_ShutDown(room);
                    if (shut != 0)
                        sub_WarRoom_MakeStateInfo(room, &guard);
                    need = need - 1;
                }
            }
        }
        else if (diff > 0)
        {
            unsigned int need = (unsigned int)diff;
            void* dm2 = sub_G_CDataManager();
            int firstIdx = sub_WarAreaCounter_GetWarRoomCountFirstIndex(
                (char*)dm2 + 0x87b4, area);
            while (need != 0)
            {
                firstIdx = firstIdx + 1;
                WarRoom* room = G_CGameManager()->GetWarRoom();
                if (room == 0)
                    break;
                if (room == 0 || sub_WarRoom_GetState(room) != -1)
                    ;
                else
                {
                    sub_WarRoom_Open(room);
                    sub_WarRoom_MakeStateInfo(room, &guard);
                    need = need - 1;
                }
            }
        }
        totalDiff = totalDiff + (diff < 0 ? -diff : diff);
        area = area + 1;
    }
}

void CGameManager::WarRoomCountManageTest(int hourIndex)
{
    int current[10];
    for (int i = 0; i < 10; ++i)
        current[i] = 0;
    int total = 0;
    for (int area = 0; area < 10; ++area)
    {
        int n = 0;
        for (;;)
        {
            void* dm = sub_G_CDataManager();
            int need = sub_WarAreaCounter_GetWarRoomCountAtPeekTime(
                (char*)dm + 0x87b4, area);
            if (need <= n)
                break;
            total = total + 1;
            WarRoom* room = GetWarRoom();
            if (room == 0)
            {
                LogManager::logFormat(1, "App.cpp",
                                      "void CGameManager::WarRoomCountManageTest(int)", 0x30e2,
                                      "WarRoomCountManage Error : WarRoom does not exist %d", total);
            }
            else
            {
                if (sub_WarRoom_GetState(room) != -1)
                    current[area] = current[area] + 1;
            }
            n = n + 1;
        }
    }
    int hourTable = hourIndex;
    PacketGuard guard;
    guard.put_header(0, 0x58);
    int index = guard.get_index();
    guard.put_short(0);
    int totalDiff = 0;
    for (int area = 0; area < 10; ++area)
    {
        void* dm = sub_G_CDataManager();
        int diff = *(int*)((char*)dm + 8 + (hourTable * 10 + area + 0x223c) * 4) -
                   current[area];
        if (diff < 0)
        {
            int need = diff < 0 ? -diff : diff;
            void* dm2 = sub_G_CDataManager();
            int lastIdx = sub_WarAreaCounter_GetWarRoomCountLastIndex(
                (char*)dm2 + 0x87b4, area);
            while (need != 0)
            {
                lastIdx = lastIdx - 1;
                WarRoom* room = G_CGameManager()->GetWarRoom();
                if (room == 0 || sub_WarRoom_GetState(room) == -1)
                    ;
                else
                {
                    char shut = sub_WarRoom_ShutDown(room);
                    if (shut != 0)
                        sub_WarRoom_MakeStateInfo(room, &guard);
                    need = need - 1;
                }
            }
        }
        else if (diff > 0)
        {
            unsigned int need = (unsigned int)diff;
            void* dm2 = sub_G_CDataManager();
            int firstIdx = sub_WarAreaCounter_GetWarRoomCountFirstIndex(
                (char*)dm2 + 0x87b4, area);
            while (need != 0)
            {
                firstIdx = firstIdx + 1;
                WarRoom* room = G_CGameManager()->GetWarRoom();
                if (room == 0 || sub_WarRoom_GetState(room) != -1)
                    ;
                else
                {
                    sub_WarRoom_Open(room);
                    sub_WarRoom_MakeStateInfo(room, &guard);
                    need = need - 1;
                }
            }
        }
        totalDiff = totalDiff + (diff < 0 ? -diff : diff);
    }
    if (totalDiff != 0)
    {
        guard.put_short(index, totalDiff);
        guard.finalize(true);
        sub_GameWorld_send_all(sub_G_GameWorld(), &guard);
        PrintWarRoomList();
    }
}

// ============================================================================
// DB 连接状态同步 / 免 GameGuard 名单
// ============================================================================
void CGameManager::updateDBLinkCharacConnectState(
    unsigned int accId, unsigned int characNo,
    ENUM_CHARAC_LINK_ACTION_TYPE action)
{
    Stream* stream = GlobalData::s_stream_pool->Acquire("App.cpp", 0x125b);
    CStreamGuard guard(stream, true);
    (**guard) << 0x1e5;
    (**guard) << -1;
    SIG_UPDATE_LINK_CHARAC_CONNECT_STATE* info =
        guard->GetInBuffer<SIG_UPDATE_LINK_CHARAC_CONNECT_STATE>();
    memset(info, 0, 0xc);
    *(unsigned int*)info = accId;
    *(unsigned int*)((char*)info + 4) = characNo;
    *(unsigned int*)((char*)info + 8) = (unsigned int)action;
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

bool CGameManager::IsNoGameGuardApplingUser(std::string& name, char* addr)
{
    std::set<std::string>::iterator it = m_noGameGuard.find(name);
    return it != m_noGameGuard.end();
}

// ============================================================================
// 管理器
// ============================================================================
QuickParty::CQuickPartySystemManager* CGameManager::GetQuickPartySystemManager()
{
    if (m_pQuickPartySystemMgr == 0)
    {
        void* pMgr = new (std::nothrow) char[0x18];
        sub_CQuickPartySystemManager_ctor(pMgr);
        m_pQuickPartySystemMgr = (QuickParty::CQuickPartySystemManager*)pMgr;
    }
    return m_pQuickPartySystemMgr;
}

QuickParty::CQuickPartyRewardManager* CGameManager::GetQuickPartyRewardManager()
{
    if (m_pQuickPartyRewardMgr == 0)
    {
        void* pMgr = new (std::nothrow) char[0x228];
        sub_CQuickPartyRewardManager_ctor(pMgr);
        m_pQuickPartyRewardMgr = (QuickParty::CQuickPartyRewardManager*)pMgr;
    }
    return m_pQuickPartyRewardMgr;
}

CPremiumLetheManager* CGameManager::GetPremiumLetheManager()
{
    if (m_pPremiumLetheMgr == 0)
    {
        void* pMgr = new (std::nothrow) char[1];
        sub_CPremiumLetheManager_ctor(pMgr);
        m_pPremiumLetheMgr = (CPremiumLetheManager*)pMgr;
    }
    return m_pPremiumLetheMgr;
}

CSharedServerMessageManager* CGameManager::GetSharedServerMessageManager()
{
    if (m_pSharedServerMessageMgr == 0)
    {
        void* dm = sub_G_CDataManager();
        void* pMgr = new (std::nothrow) char[0x18];
        sub_CSharedServerMessageManager_ctor(pMgr, (char*)dm + 0x7d8);
        m_pSharedServerMessageMgr = (CSharedServerMessageManager*)pMgr;
    }
    return m_pSharedServerMessageMgr;
}

CSpecialItemRoutingManager* CGameManager::GetSpecialItemRoutingManager()
{
    if (m_pSpecialItemRoutingMgr == 0)
    {
        void* pMgr = new (std::nothrow) char[1];
        sub_CSpecialItemRoutingManager_ctor(pMgr);
        m_pSpecialItemRoutingMgr = (CSpecialItemRoutingManager*)pMgr;
    }
    return m_pSpecialItemRoutingMgr;
}

CConditionEventManager* CGameManager::GetConditionEventManager()
{
    if (m_pConditionEventMgr == 0)
    {
        void* pMgr = new (std::nothrow) char[1];
        sub_CConditionEventManager_ctor(pMgr);
        m_pConditionEventMgr = (CConditionEventManager*)pMgr;
    }
    return m_pConditionEventMgr;
}

CAuctionAveragePrice* CGameManager::GetAuctionAveragePriceManager()
{
    if (m_pAuctionAveragePriceMgr == 0)
    {
        void* pMgr = new (std::nothrow) char[0x18];
        sub_CAuctionAveragePrice_ctor(pMgr);
        m_pAuctionAveragePriceMgr = (CAuctionAveragePrice*)pMgr;
    }
    return m_pAuctionAveragePriceMgr;
}

CraneMinigameManager* CGameManager::GetCraneMinigameManager()
{
    return m_pCraneMinigameMgr;
}

// ============================================================================
// 用户信息上报
// ============================================================================
struct Packet_Monitor_UDP_Reply_UserInfo
{
    char m_buf[0x16e6];
};

struct Packet_CutOff_UDP_Reply_UserInfo
{
    char m_buf[0xb10];
};

struct Packet_ChannelType
{
    char m_buf[0x14];
};

void CGameManager::Send_userinfos_to_upper_server(unsigned char channelType)
{
    int userCount = 0;
    Packet_Monitor_UDP_Reply_UserInfo pkt;
    sub_Packet_Monitor_UDP_Reply_UserInfo_ctor(&pkt);
    char* p = pkt.m_buf;
    p[11] = (char)((CEnvironment*)sub_G_CEnvironment())->get_channel_no();
    if (sub_GameWorld_IsIntegratedPvPBaseChannel(sub_G_GameWorld()) && channelType == 0xc9)
    {
        int idx = 0;
        int group = sub_CServerProxyMgrMonitor_GetStartIndex(GlobalData::s_monitor_proxy_mgr);
        while (idx < sub_CServerProxyMgrMonitor_GetEndIndex(GlobalData::s_monitor_proxy_mgr))
        {
            Packet_ChannelType ctype;
            sub_Packet_ChannelType_ctor(&ctype);
            *(unsigned int*)(ctype.m_buf + 0xc) =
                (unsigned int)((CEnvironment*)sub_G_CEnvironment())->get_channel_no();
            *(unsigned int*)(ctype.m_buf + 0x10) =
                (unsigned int)sub_GameWorld_GetChannelType(sub_G_GameWorld());
            void* proxy = sub_CServerProxyMgrMonitor_GetServerProxy(
                GlobalData::s_monitor_proxy_mgr, group);
            sub_CMonitorServerProxy_SendTcpPacket(
                proxy, &ctype, *(unsigned short*)(ctype.m_buf + 8));
            group = sub_CServerProxyMgrMonitor_GetNextIndex(
                GlobalData::s_monitor_proxy_mgr, &idx);
        }
    }
    if (!m_userByAccId.empty())
    {
        int groupIdx = 0;
        int group = sub_CServerProxyMgrMonitor_GetStartIndex(GlobalData::s_monitor_proxy_mgr);
        while (groupIdx < sub_CServerProxyMgrMonitor_GetEndIndex(GlobalData::s_monitor_proxy_mgr))
        {
            CUser* lastUser = 0;
            std::map<unsigned int, CUser*>::iterator it = m_userByAccId.begin();
            for (; it != m_userByAccId.end(); ++it)
            {
                CUser* user = it->second;
                if (user->get_state() >= 2 &&
                    (int)(size_t)user->GetServerGroup() == group)
                {
                    int n = userCount;
                    *(unsigned int*)(p + 12 + n * 0x4e) = user->get_acc_id();
                    *(unsigned int*)(p + 16 + n * 0x4e) = user->get_charac_no(-1);
                    *(unsigned int*)(p + 20 + n * 0x4e) = user->GetUID();
                    *(unsigned int*)(p + 24 + n * 0x4e) = user->get_charac_guildkey();
                    p[28 + n * 0x4e] = user->get_charac_job();
                    p[29 + n * 0x4e] = user->getCurCharacGrowType();
                    *(unsigned short*)(p + 30 + n * 0x4e) = user->get_charac_level();
                    *(unsigned int*)(p + 62 + n * 0x4e) = user->get_charac_memberkey();
                    strcpy(p + 32 + n * 0x4e, user->getCurCharacName());
                    p[82 + n * 0x4e] = sub_CUser_getSex(user);
                    memcpy(p + 83 + n * 0x4e, sub_CUser_GetSsnString(user), 6);
                    char ipBuf[16];
                    ((CNetwork<4096, 450000>*)((char*)user + 0xe0))
                        ->GetPeerIP2(ipBuf, 0x10);
                    strcpy(p + 66 + n * 0x4e, ipBuf);
                    lastUser = user;
                    userCount = userCount + 1;
                    if (userCount > 0x4a)
                    {
                        *(unsigned short*)(p + 2) = 0x16e6;
                        p[10] = (char)userCount;
                        sub_SendUserInfoToUpperServer(&pkt, channelType, user, group);
                        userCount = 0;
                    }
                }
            }
            if (userCount != 0)
            {
                *(unsigned short*)(p + 2) = (short)(userCount * 0x4e + 0xc);
                p[10] = (char)userCount;
                sub_SendUserInfoToUpperServer(&pkt, channelType, lastUser, group);
                userCount = 0;
            }
            group = sub_CServerProxyMgrMonitor_GetNextIndex(
                GlobalData::s_monitor_proxy_mgr, &groupIdx);
        }
    }
}

void CGameManager::send_userinfos_to_cutoff_server()
{
    struct STTempUsers
    {
        unsigned short m_count;
        int m_ids[600];
        STTempUsers() { m_count = 0; }
    };
    STTempUsers buckets[10];
    for (int i = 9; i != -1; --i)
        new (&buckets[i]) STTempUsers();
    Packet_CutOff_UDP_Reply_UserInfo pkt;
    sub_Packet_CutOff_UDP_Reply_UserInfo_ctor(&pkt);
    pkt.m_buf[12] = (char)((CEnvironment*)sub_G_CEnvironment())->get_channel_no();
    pkt.m_buf[13] = *(char*)((char*)sub_G_CEnvironment() + 0x378);
    if (!m_userByAccId.empty())
    {
        std::map<unsigned int, CUser*>::iterator it = m_userByAccId.begin();
        for (; it != m_userByAccId.end(); ++it)
        {
            CUser* user = it->second;
            if (user->get_state() > 1)
            {
                unsigned int accId = user->get_acc_id();
                unsigned int bucketIdx = accId % 10;
                STTempUsers* bucket = &buckets[bucketIdx];
                unsigned short n = bucket->m_count;
                ((int*)buckets)[bucketIdx * 0x259 + n] = (int)accId;
                bucket->m_count = bucket->m_count + 1;
            }
        }
        for (int i = 0; i < 10; ++i)
        {
            if (*(unsigned short*)((char*)buckets + i * 0x964) != 0)
            {
                *(unsigned short*)(pkt.m_buf + 2) =
                    *(unsigned short*)((char*)buckets + i * 0x964) * 4 + 0xe;
                *(unsigned int*)(pkt.m_buf + 6) =
                    *(unsigned int*)((char*)buckets + 4 + i * 0x964);
                *(unsigned short*)(pkt.m_buf + 10) =
                    *(unsigned short*)((char*)buckets + i * 0x964);
                memcpy(pkt.m_buf + 16,
                       (char*)buckets + 4 + i * 0x964,
                       (unsigned int)*(unsigned short*)((char*)buckets + i * 0x964) * 4);
                sub_CDoubleConnCheckServerProxy_SendPacket(
                    GlobalData::s_double_check_proxy, &pkt,
                    (unsigned int)*(unsigned short*)((char*)buckets + i * 0x964) * 4 + 0xe);
            }
        }
    }
}

// ============================================================================
// 进入游戏世界
// ============================================================================
enum ENUM_HARDCODE_ITEM
{
    ENUM_HARDCODE_ITEM_0 = 0
};

unsigned int CGameManager::insert_game_world(CUser* user)
{
    if (user == 0)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf20,
                              "insert_waiting_room pUser parameter is null.");
        return 0;
    }
    const void* characR = user->getCurCharacR();
    if (characR == 0)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf27,
                              "Not selected character(a reason is fast returning to character screen on client)");
        return 0;
    }
    int curSec = GlobalData::s_systemTime_.getCurSec();
    std::vector<std::pair<int, int> > vecA;
    std::vector<std::pair<int, int> > vecB;
    bool bGuildPointDel = false;
    if (curSec < sub_Inter_MonitorGuildPointDel_getLastDeleteTime())
    {
        bGuildPointDel = false;
    }
    else if (sub_Inter_MonitorGuildPointDel_getLastDeleteTime() <=
             user->getCurCharacLastPlayTick())
    {
        bGuildPointDel = false;
    }
    else
    {
        bGuildPointDel = true;
    }
    if (bGuildPointDel)
    {
        int a = 0;
        int b = 0xc84;
        vecA.push_back(std::make_pair(a, b));
    }
    if (user->getCurCharacGuildSecede() != 0)
    {
        std::pair<int, int> p =
            std::make_pair((ENUM_HARDCODE_ITEM)0, 0x4b0);
        vecA.push_back(std::move(p));
        user->setCurCharacGuildSecede(0);
        sub_DB_GuildExpBookDelete_makeRequest(
            (unsigned int)user->get_charac_no(-1), 0,
            (int)(size_t)user->GetServerGroup());
    }
    sub_CUser_ResetItemByScript(user, &vecA);
    int curSec2 = GlobalData::s_systemTime_.getCurSec();
    const void* characR2 = user->getCurCharacR();
    long lastPlay = *(long*)((char*)characR2 + 0x7b);
    void* env = sub_G_CEnvironment();
    if (sub_CheckDailyScheduleTime(
            *(int*)((char*)env + 0x37c), lastPlay, curSec2))
    {
        std::vector<std::pair<int, int> > vecC;
        sub_CUser_deleteDailyStackableItem(user, &vecC, 3, 8);
        sub_CUser_deleteDailyStackableItem(user, &vecC, 0x39, 0x68);
    }
    sub_CUser_deleteSpecificItem(user, &vecA, &vecB);
    if (sub_CUser_send_itemspace(user, 0xc) != 1)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf58,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_ACCOUNT_CARGO ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (sub_CUser_send_itemspace(user, 0) != 1)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf5f,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_INVENTORY ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (sub_CUser_send_itemspace(user, 1) != 1)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf64,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_AVATAR ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (sub_CUser_send_itemspace(user, 2) != 1)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf69,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_CARGO ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (sub_CUser_send_itemspace(user, 7) != 1)
    {
        LogManager::logFormat(1, "App.cpp",
                              "bool CGameManager::insert_game_world(CUser*)", 0xf7a,
                              "CGameManager::insert_game_world, pUser(%s)->send_itemspace( ENUM_ITEMSPACE_CREATURE ) return FAILED",
                              NumberToString(user->get_acc_id(), 0));
        return 0;
    }
    if (sub_GameWorld_GetChannelType(sub_G_GameWorld()) == 0xe)
    {
        user->set_charac_guildkey(0);
    }
    if (sub_GameWorld_reach_game_world(sub_G_GameWorld(), user) != 1)
    {
        cMyTrace tr("bool CGameManager::insert_game_world(CUser*)", 0xfa2, 0);
        tr("CGameManager::insert_game_world , !G_GameWorld()->reach_game_world( pUser ) , USER : %d(%s)",
           user->get_unique_id() & 0xffff, NumberToString(user->get_acc_id(), 0));
        user->SendCmdErrorPacket((ENUM_CMDPACKET)1, 4);
        cMyTrace tr2("bool CGameManager::insert_game_world(CUser*)", 0xfab, 0);
        tr2("from (%d) m_id[%s] uid[%d]", 1,
            NumberToString(user->get_acc_id(), 0), user->get_unique_id() & 0xffff);
        return 0;
    }
    int eventGrow = (int)user->getCurCharacEventCharacterGrowtype();
    if (eventGrow > 0)
    {
        void* handler = sub_CEventCharacterHandlerInstance();
        sub_CEventCharacterHandler__makeEventCharacter(handler, user, 0x28);
    }
    CExpandEquipslot* expandSlot = user->GetCharacExpandData(ENUM_CHARAC_EXPAND_TYPE_9);
    if (sub_GameWorld_IsEquipSlotSwitchChannel(sub_G_GameWorld()) == 0)
    {
        if (user->getCurChannelEquipslotSwitch() == 1)
        {
            char c = user->getCurExpandEquipslotSwitch();
            char d = user->getCurChannelEquipslotSwitch();
            sub_CExpandEquipslot_EquipslotSwitch(expandSlot, user, 0, d, c);
        }
    }
    else
    {
        if (user->getCurChannelEquipslotSwitch() == 0)
        {
            char c = user->getCurExpandEquipslotSwitch();
            char d = user->getCurChannelEquipslotSwitch();
            sub_CExpandEquipslot_EquipslotSwitch(expandSlot, user, 0, d, c);
        }
    }
    int channelType = sub_GameWorld_GetChannelType(sub_G_GameWorld());
    if (channelType == 0xf || channelType == 0x10)
    {
        void* mission = user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)8);
        sub_CMissionList_Charac_Update_EnterWorld_event(mission, user);
    }
    PacketGuard guard;
    guard.put_header(0, 2);
    guard.put_byte(0);
    guard.put_short(1);
    user->make_basic_info((char*)&guard, 0);
    guard.finalize(true);
    sub_GameWorld_send_all_user(sub_G_GameWorld(), &guard, user);
    user->send_skill_info();
    guard.clear();
    guard.put_header(0, 2);
    guard.put_byte(1);
    guard.put_short(1);
    user->make_basic_info((char*)&guard, 1);
    guard.finalize(true);
    user->Send(guard);
    sub_CExpandEquipslot_Send_Expand_Equip_Info(expandSlot, user);
    sub_CUser_processReturnUserQuestAutoClear(user);
    sub_CUser_send_clear_quest_list(user);
    guard.clear();
    sub_UserQuest_get_quest_info(user->getCurCharacQuestR(), &guard);
    user->Send(guard);
    sub_CTitleBook_sendList(user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)14));
    sub_CAchievement_sendList(user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)15));
    sub_CItemLock_SendItemLockList(user->GetCharacExpandData((ENUM_CHARAC_EXPAND_TYPE)2), user);
    if (sub_GameWorld_GetChannelType(sub_G_GameWorld()) != 0xf)
    {
        SendPvpList(user);
    }
    SendWarRoomList(user);
    user->send_pvp_record();
    SendPartyList(user);
    short punish = user->getPunishTradeAlert();
    if (punish == 0xe7 || punish == 0xe8)
    {
        guard.clear();
        guard.put_header(0, 0xff);
        guard.put_int((int)user->getPunishTradeAlert());
        guard.finalize(true);
        user->Send(guard);
    }
    guard.clear();
    guard.put_header(0, 0x7c);
    guard.finalize(true);
    user->Send(guard);
    std::map<std::pair<char, char>, int>* pics =
        (std::map<std::pair<char, char>, int>*)sub_CUser_GetPICSMap(user);
    pics->clear();
    sub_CUser_ReCalcChattingEmoticon(user);
    sub_CUser_SendChattingEmoticon(user);
    char monitor[0x3f];
    sub_Packet_Monitor_Char_Info_ctor(monitor);
    *(unsigned int*)(monitor + 0xa) = user->get_acc_id();
    monitor[0xe] = (char)((CEnvironment*)sub_G_CEnvironment())->get_channel_no();
    monitor[0x3d] = (char)sub_GameWorld_GetChannelType(sub_G_GameWorld());
    *(unsigned int*)(monitor + 0xf) = (unsigned int)user->get_charac_no(-1);
    *(unsigned int*)(monitor + 0x13) = user->get_charac_guildkey();
    monitor[0x17] = user->get_charac_job();
    monitor[0x18] = user->getCurCharacGrowType();
    *(unsigned short*)(monitor + 0x19) = user->get_charac_level();
    *(unsigned int*)(monitor + 0x1b) = user->get_charac_memberkey();
    const char* name = user->getCurCharacName();
    int nameLen = (int)strlen(name);
    memcpy(monitor + 0x1f, user->getCurCharacName(), nameLen);
    monitor[0x3e] = user->IsReturnUserFirstLogin();
    int serverGroup = (int)(size_t)user->GetServerGroup();
    void* monitorProxy = sub_CServerProxyMgrMonitor_GetServerProxy(
        GlobalData::s_monitor_proxy_mgr, serverGroup);
    sub_CMonitorServerProxy_SendTcpPacket(monitorProxy, monitor, 0x3f);
    int guildGroup = (int)(size_t)user->GetServerGroup();
    void* guildProxy = sub_CServerProxyMgrGuild_GetServerProxy(
        GlobalData::s_guild_proxy_mgr, guildGroup);
    sub_CGuildServerProxy_SendTcpPacket(guildProxy, monitor, 0x3f);
    sub_CSchoolMgr_AddUser(g_schoolMgr, user);
    if (user->get_charac_guildkey() != 0)
    {
        sub_CGuildAgitManager_AllocGuildAgitArea(
            g_guildAgitMgr, user->get_charac_guildkey());
    }
    int characNo = user->getCurCharacNo();
    int loginTick = user->GetLoginTick();
    unsigned int uid = user->GetUID();
    GlobalData::s_timerQueue_->InsertTimer(
        TimerEntry::OBJ_TYPE_0, 0, (TIMER_MESSAGE)2, 0x3c, loginTick, uid);
    int characNo2 = user->getCurCharacNo();
    int loginTick2 = user->GetLoginTick();
    int randSec = sub_get_rand_int(300) + 600;
    unsigned int uid2 = user->GetUID();
    GlobalData::s_timerQueue_->InsertTimer(
        TimerEntry::OBJ_TYPE_0, 0, (TIMER_MESSAGE)1, randSec, loginTick2, uid2);
    int characNo3 = user->getCurCharacNo();
    int loginTick3 = user->GetLoginTick();
    unsigned int uid3 = user->GetUID();
    GlobalData::s_timerQueue_->InsertTimer(
        TimerEntry::OBJ_TYPE_0, 0, (TIMER_MESSAGE)0x5f, 300, loginTick3, uid3);
    int characNo4 = user->getCurCharacNo();
    int loginTick4 = user->GetLoginTick();
    unsigned int uid4 = user->GetUID();
    GlobalData::s_timerQueue_->InsertTimer(
        TimerEntry::OBJ_TYPE_0, 0, (TIMER_MESSAGE)0x60, 600, loginTick4, uid4);
    user->send_ontime_reward_start_notify();
    return 1;
}

// ============================================================================
// 断开 / 退出
// ============================================================================
void CGameManager::user_disconnect(CUser* user)
{
    if (user == 0)
    {
        cMyTrace tr("void CGameManager::user_disconnect(CUser*)", 0x138b, 5);
        tr("USER DISCONNECT [%s][%d]",
           "void CGameManager::user_disconnect(CUser*)", 0x138b);
        return;
    }
    sub_HistoryLog_LogClose(*(void**)((char*)user + 0x796f8));
    if (user->get_state() != 0)
    {
        sub_CUser_prepareDisconnect(user);
        sub_GameWorld_EraseLoginUser(sub_G_GameWorld(), user);
        sub_CUser_SetSaveRentalInfoToExchange(user, false);
        if (user->get_state() < 3)
        {
            sub_CUser_SetSaveRentalInfoToExchange(user, false);
            user->LogoutCachedCharacter(0);
            user->UpdateLogout(true);
            returnUserPool(user);
        }
        else
        {
            CheckOutTrade(user);
            CheckOutParty(user, false);
            CheckOutPvp(user, false);
            CheckOutWarRoom(user);
            checkOutDeathTower(user);
            checkOutBossTower(user);
            checkOutAdvanceAltar(user);
            checkOutBlueMarble(user);
            sub_GameWorld_leave_game_world(sub_G_GameWorld(), user);
            sub_CUser_SetGameMasterMode(user, false);
            user->UpdateData();
            user->LogoutCachedCharacter(0);
            user->UpdateLogout(true);
            sub_CUser_DeleteRentalItemInfo(user, 1);
            sub_CUser_DeleteRentalItemInfo(user, 2);
            user->ResetCurCharac();
            sub_CUser_doLinkCharacDisconnect(user);
            returnUserPool(user);
        }
        void* mgr = sub_GetInstanceShutdowManager();
        sub_CShutdowManager_SendLastMsgDBQueue(mgr);
    }
}

bool CGameManager::user_exit(CUser* user)
{
    if (user == 0)
    {
        cMyTrace tr("bool CGameManager::user_exit(CUser*)", 0x1271, 5);
        tr("USER_EXIT [%s][%d]", "bool CGameManager::user_exit(CUser*)", 0x1271);
        return 0;
    }
    if (user->get_state() == 0)
    {
        cMyTrace tr("bool CGameManager::user_exit(CUser*)", 0x127a, 5);
        tr("USER_EXIT STATE NONE [%s][%d]mid(%d)",
           "bool CGameManager::user_exit(CUser*)", 0x127a, user->get_acc_id());
        return 0;
    }
    sub_HistoryLog_LogClose(*(void**)((char*)user + 0x796f8));
    sub_CUser_prepareDisconnect(user);
    sub_GameWorld_EraseLoginUser(sub_G_GameWorld(), user);
    if (user->get_state() < 3)
    {
        sub_CUser_SetSaveRentalInfoToExchange(user, false);
        user->LogoutCachedCharacter(0);
        user->UpdateLogout(false);
    }
    else
    {
        CheckOutTrade(user);
        CheckOutParty(user, false);
        CheckOutPvp(user, false);
        CheckOutWarRoom(user);
        checkOutDeathTower(user);
        checkOutBossTower(user);
        checkOutAdvanceAltar(user);
        checkOutBlueMarble(user);
        sub_GameWorld_leave_game_world(sub_G_GameWorld(), user);
        sub_CUser_SetGameMasterMode(user, false);
        user->UpdateData();
        user->LogoutCachedCharacter(0);
        user->UpdateLogout(false);
        cMyTrace tr("bool CGameManager::user_exit(CUser*)", 0x12dc, 0);
        tr("ID: %s - UID: %d from Logout",
           user->getCurCharacName(), user->get_unique_id() & 0xffff);
        int err = 0;
        int sock = ((CNetwork<4096, 450000>*)((char*)user + 0xe0))->get_socket();
        err = sub_EpollHandler_ResetEpoll(sub_G_EpollHandler(), sock);
        if (err != 0)
        {
            LogManager::logFormat(
                1, "App.cpp", "bool CGameManager::user_exit(CUser*)", 0x12e3,
                "(m_id: %s) G_EpollHandler()->ResetEpoll(pUser->get_socket(%d)) %d(%s)",
                NumberToString(user->get_acc_id(), 0),
                ((CNetwork<4096, 450000>*)((char*)user + 0xe0))->get_socket(),
                err, strerror(err));
        }
        user->ResetCurCharac();
        sub_CUser_doLinkCharacDisconnect(user);
        returnUserPool(user);
        sub_CUser_checkLogOutCorrectly(user);
    }
    return 1;
}

// ============================================================================
// 构造 / 析构
// ============================================================================
CGameManager::CGameManager()
{
    m_uid = 0;
    m_noGameGuard.insert(std::string("testman820"));
    m_gameGuard2.insert(std::string("htilil"));
    m_noGameGuard.insert(std::string("azzuman"));
    m_gameGuard2.insert(std::string("cuwaki"));
    m_noGameGuard.insert(std::string("slainer"));
    m_gameGuard2.insert(std::string("mrsbscom"));
    m_noGameGuard.insert(std::string("arandra"));
    m_gameGuard2.insert(std::string("krucef"));
    m_noGameGuard.insert(std::string("dnftest126"));
    m_gameGuard2.insert(std::string("dnftest127"));
    m_noGameGuard.insert(std::string("dnftest128"));
    m_gameGuard2.insert(std::string("59.10.138.244"));
    m_noGameGuard.insert(std::string("218.145.141.85"));
    m_gameGuard2.insert(std::string("121.134.32.42"));
    m_pQuickPartySystemMgr = 0;
    m_pQuickPartyRewardMgr = 0;
    m_pPremiumLetheMgr = 0;
    m_pSharedServerMessageMgr = 0;
    m_pConditionEventMgr = 0;
    m_pSpecialItemRoutingMgr = 0;
    m_pAuctionAveragePriceMgr = 0;
    m_pCraneMinigameMgr = 0;
}

CGameManager::~CGameManager()
{
    if (m_pQuickPartySystemMgr != 0)
    {
        sub_CQuickPartySystemManager_dtor(m_pQuickPartySystemMgr);
        operator delete(m_pQuickPartySystemMgr);
    }
    m_pQuickPartySystemMgr = 0;
    if (m_pQuickPartyRewardMgr != 0)
    {
        sub_CQuickPartyRewardManager_dtor(m_pQuickPartyRewardMgr);
        operator delete(m_pQuickPartyRewardMgr);
    }
    m_pQuickPartyRewardMgr = 0;
    if (m_pAuctionAveragePriceMgr != 0)
    {
        sub_CAuctionAveragePrice_dtor(m_pAuctionAveragePriceMgr);
        operator delete(m_pAuctionAveragePriceMgr);
        m_pAuctionAveragePriceMgr = 0;
    }
    if (m_pCraneMinigameMgr != 0)
    {
        sub_CraneMinigameManager_dtor(m_pCraneMinigameMgr);
        operator delete(m_pCraneMinigameMgr);
    }
}
