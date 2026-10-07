// ============================================================================
// [SplitC 2026-09-11 batch3 · TU 迁移（ORIG GI 锚点）] 本文件由 CAuction.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN21CDispatchServerPacket14dispatchPacketEPc
//   ORIG TU     ：DispatchServerPacket.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include <cstring>
#include <cstdio>
#include "BaseServerProxy.h"
#include "CNetwork.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "CVillageMonsterMgr.h"
#include "CEnvironment.h"
#include "PacketDesign.h"
#include "SigTypes.h"     // SIG_AUCTION_* 权威布局（本 TU GetInBuffer 实例化）
#include "TimerDispatcher.h"  // TimerSig / Timer_CheckAuctionReady 权威声明（独占本 TU 实现）
#include "LogManager.h"   // LogManager::logFormat
#include "LogManager.h"   // LogManager::logFormat
#include "Arad_InternalDispatcher.h"  // ARAD::DISPATCHER::make_internal_stream_jpn
#include "RestrictGeolocation.h"      // 0x27fe case：initGeolocation
#include "ServiceRestrictManager.h"   // 0x27ff case：loadPolicy
#include "CHadesServerProxy.h"       // 0xfe7 case：ConnTcpHadesSvr
#include "CSwitchLog.h"              // 0x2717 case：SetLogWriteFlag
#include "../../../shared/packet/include/Packet_Guild_Apply_Origial_Power_Side.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Guild_Create_Guild_Agit.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Monitor_UDP_Login.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Broadcast_Msg.h"  // 0x… case 包结构（shared/packet/include）
#include "Packet_Change_Guild_Name.h"
#include "Packet_CutOff_UDP_Reply_UserInfo.h"
#include "Packet_Delete_PvP_Buddy.h"  // 0x… case 包结构（shared/packet/include）
#include "../../../shared/packet/include/Packet_Delete_To_BlackList.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Exchange_Server_Info.h"
#include "../../../shared/packet/include/Packet_Find_Charac_Name_UseUID.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Forbid_Chat_By_Monitor.h"
#include "../../../shared/packet/include/Packet_GM_Request_Mid.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Guild_Call_Guild_Invite.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Guild_Cargo_History_Request.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Guild_Cargo_Requst.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Guild_Check_Guild_Cargo_Push_Item.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Guild_Delete_Guild_Agit.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Guild_Exp_Limit.h"
#include "../../../shared/packet/include/Packet_Guild_Guild_Cargo_Move_Item.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Guild_Guild_Cargo_Pop_Item.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Guild_Guild_Cargo_Upgrade.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Guild_Power_War_Process_Info.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Guild_Reply_Guild_Invite_From_Invited.h"
#include "../../../shared/packet/include/Packet_Guild_Request_Guild_Master_Delegate.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Guild_Request_Guild_Secede.h"
#include "Packet_Insert_PvP_Buddy.h"  // 0x… case 包结构（shared/packet/include）
#include "Packet_MiniCraneSeed.h"
#include "Packet_Monitor_Other_Channel_Chat_ToUser.h"  // dispatchPacket case 0x3f3 字段化
#include "Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link.h"  // case 0x2719 字段化
#include "Packet_Monitor_Call_Guild_Members.h"  // case 0x401 字段化
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel.h"  // case 0x424 字段化
#include "Packet_Notice_Request_Add_PvP_Buddy.h"  // case 0x1b5d 字段化
#include "Packet_Notice_Add_PvP_Buddy_Result.h"  // case 0x1b5f 字段化
#include "Packet_Response_Remove_PvP_Buddy.h"  // case 0x1b61 字段化
#include "Packet_Check_Overlapped_Accusation.h"  // case 0x1b66 字段化
#include "../../../shared/packet/include/Packet_Register_To_BlackList.h"  // case 0x5dc：显式 shared（Game 版为成员组合式同名类）
#include "../../../shared/packet/include/Packet_Monitor_Add_Buddy.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Guild_Guild_Cargo_Push_Item.h"  // dispatchPacket case 0x70d 字段化
#include "Packet_Monitor_Call_Guild_All_Members_ToChannel_Next.h"  // 0x… case 包结构（shared/packet/include）
#include "../../../shared/packet/include/Packet_Monitor_Call_Guild_Info.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Monitor_Call_Guild_Members_ToChannel_Next.h"  // 0x… case 包结构（shared/packet/include）
#include "../../../shared/packet/include/Packet_Monitor_Call_Member_List.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Monitor_Del_Buddy.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Monitor_Event_End.h"
#include "Packet_Monitor_Event_Start.h"
#include "../../../shared/packet/include/Packet_Monitor_Guild_Chat.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Monitor_Guild_Chat_Hyper_Link.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Monitor_Max_Level_BroadCast.h"
#include "Packet_Monitor_MegaPhone.h"
#include "Packet_Monitor_MegaPhone_Hyper_Link.h"
#include "../../../shared/packet/include/Packet_Monitor_Member_Chat.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Monitor_Member_Chat_Hyper_Link.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Monitor_Member_Enter_Reply.h"
#include "../../../shared/packet/include/Packet_Monitor_Member_Pay_Tax.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Monitor_Member_Secede.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Monitor_Notice_Guild_Dismiss.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Monitor_Notice_Message.h"
#include "../../../shared/packet/include/Packet_Monitor_Notify_New_Mail.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Monitor_Reply_Charac_Info.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Monitor_Request_Member_Enter.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Monitor_Send_Guild_Mail.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Monitor_Server_Message_Info.h"
#include "../../../shared/packet/include/Packet_Monitor_Set_Sub_Guild_Master.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Monitor_UDP_Double_Connect.h"  // 0x… case 包结构（shared/packet/include）
#include "Packet_Monitor_UDP_Reply_UserInfo.h"
#include "Packet_No_Cache.h"
#include "Packet_Notice_Add_PvP_Buddy_Result.h"  // 0x… case 包结构（shared/packet/include）
#include "../../../shared/packet/include/Packet_Notice_Guild_War_Info.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Notice_PvP_Buddy_In_Out.h"  // 0x… case 包结构（shared/packet/include）
#include "Packet_Notice_Request_Add_PvP_Buddy.h"  // 0x… case 包结构（shared/packet/include）
#include "Packet_Pcroom_Response.h"  // 0x… case 包结构（shared/packet/include）
#include "../../../shared/packet/include/Packet_PvPChannelInfo.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_PvPChannelUserCount.h"  // 0x… case 包结构（shared/packet/include）
#include "Packet_PvPResult.h"  // 0x… case 包结构（shared/packet/include）
#include "../../../shared/packet/include/Packet_Register_To_BlackList.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "../../../shared/packet/include/Packet_Request_BlackList.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I
#include "Packet_Request_Guild_Create.h"
#include "Packet_Request_Join_Power.h"
#include "Packet_Request_Secede_Power.h"
#include "Packet_Response_PvP_Buddy_Conn_List.h"  // 0x… case 包结构（shared/packet/include）
#include "Packet_Response_Remove_PvP_Buddy.h"  // 0x… case 包结构（shared/packet/include）
#include "Packet_TOD_DoRandomSelect.h"
#include "Packet_Update_PvP_Buddy.h"  // 0x… case 包结构（shared/packet/include）
#include "Packet_Web_Notice_InGame_Advertisement.h"  // 0x… case 包结构（shared/packet/include）
#include "Packet_Web_Notice_Single.h"  // 0x… case 包结构（shared/packet/include）
#include "../../../shared/packet/include/Packet_Web_Notify_Message_To_Guild.h"  // 显式路径:Game 同名空壳优先级会遮蔽 -I

#include "../../../shared/packet/include/PacketHeader.h"
#pragma pack(push, 1)
struct GuildKeyIntView : PacketHeader {              // 0x3fc 等：+0xa 公会键 int 视角（字段 unsigned，ORIG 直落 int 实参）
    int m_guildKey;
};
struct IntView0a : PacketHeader {
    int m_v;
};
struct IntView0b : PacketHeader { char m_afterHdr; int m_v; };
struct IntView0d : MSG_BASE { int m_v; };
struct IntView0e : PacketHeader  { unsigned int m_fieldA; int m_v; };
struct IntView0f : PacketHeader { char m_afterHdr[0x5]; int m_v; };
struct IntView12 : PacketHeader  { unsigned int m_fieldA; unsigned int m_fieldE; int m_v; };
struct IntView14 : PacketHeader { char m_afterHdr[0xa]; int m_v; };
struct IntView29 : PacketHeader { char m_afterHdr[31]; int m_v; };
struct UIntView0a : PacketHeader {
    unsigned int m_v;
};
struct UIntView0e : PacketHeader  { unsigned int m_fieldA; unsigned int m_v; };
struct UIntView0f : PacketHeader { char m_afterHdr[0x5]; unsigned int m_v; };
struct UIntView12 : PacketHeader  { unsigned int m_fieldA; unsigned int m_fieldE; unsigned int m_v; };
struct UIntView14 : PacketHeader { char m_afterHdr[0xa]; unsigned int m_v; };
// [DNF-FIX 2026-10-08] 0f/14 视图原缺 PacketHeader 基类 ⇒ m_v 落 +0x5/+0xa；
//   ORIG dispatchPacket 0x08476b8f（234 键 +0x14）/0x08478975（436 键 +0xf）/
//   0x08479903（532 键 +0xf）。补基类使 m_v 回到 +0xf/+0x14。
static_assert(sizeof(IntView0f) == 0x13 && sizeof(UIntView0f) == 0x13, "m_v @+0xf");
static_assert(sizeof(IntView14) == 0x18 && sizeof(UIntView14) == 0x18, "m_v @+0x14");
struct UCharView0a : PacketHeader {
    unsigned char m_v;
};

struct RevengeView : PacketHeader {                  // 0x1b6a：+0xa 奖励类型 int，+0xe 关闭时间 uint
    int m_reward;   unsigned int m_closeTime;
};
struct NoticeGuildSecedeView : PacketHeader {        // = Packet_Monitor_Notice_Guild_Secede_ToUser（0x3fe，Guild/DNFGuild.h）
    unsigned int m_channel;   // +0xa（GetIdByChannel）
};
struct GuildMemberLoginOutView : PacketHeader  {      // = Packet_Monitor_Notice_Guild_Member_Login_out（0x407，Guild/DNFGuild.h）
    char m_afterHdr;
    int m_channel;            // +0xb
};
struct NoticeGuildInfoView : PacketHeader {          // = Packet_Monitor_Notice_Guild_Info（0x40a，Guild/DNFGuild.h）
    unsigned int m_channel;   // +0xa（GetIdByChannel）
};
struct GuildCreateToUserView : PacketHeader  {        // = Packet_Monitor_Notice_Guild_Create_ToUser
    char m_info[0x1f];        // +0xa ST_GuildCreateFromWeb
    unsigned int m_member;    // +0x29
};
struct GuildMemberGradeView : PacketHeader {         // = Packet_Monitor_Notify_GuildMemberGrade（0x42c，Guild/DNFPacketTranslater.h）
    unsigned int m_channel;   // +0xa（GetIdByChannel）
};
struct GuildMemberInfoView : PacketHeader {          // = Packet_Monitor_Notice_Guild_Member_Info（0x431，Guild/DNFUser.h）
    int m_channel;            // +0xa
};
struct GuildInviteToInvitedView : PacketHeader  {     // = Packet_Guild_Call_Guild_Invite_To_Invited（0x435，Guild/DNFPacketTranslater.h）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_e;         // +0xe
};
struct GuildInviteToCallerView : PacketHeader  {      // = Packet_Guild_Reply_Guild_Invite_To_Caller（0x437，Guild/DNFPacketTranslater.h）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_e;         // +0xe
};
struct GuildMasterDelegateView : PacketHeader  {      // = Packet_Guild_Notice_Guild_Master_Delegate（0x43d，Guild/DNFGuild.h）
    unsigned int m_fieldA;  // +0xa
    int m_channel;            // +0xe
};
struct HasBeenGuildMasterView : PacketHeader  {       // = Packet_Notice_Has_Been_Guild_Master（0x43e，Guild/DNFPacketTranslater.h）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_channel;   // +0xe
};
struct GuildNameChangeView : PacketHeader  {          // = Packet_Guild_Notice_Guild_Name_Change（0x446，Guild/DNFGuild.h）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    unsigned int m_channel;   // +0x12
};
struct AllUserMinForGuildView : PacketHeader  {       // = Packet_Send_All_User_Info_Minimum_For_Guild_System（0x447，Guild/DNFUser.h）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    int m_channel;            // +0x12
};
struct MonitorMemberLoginOutView : PacketHeader  {    // = Packet_Monitor_Notice_Member_Member_Login_out（0x4b6，Monitor/DNFPacketTranslater.h）
    char m_afterHdr;
    unsigned int m_idByChannel;  // +0xb
};
struct MemberEnterReplyView : MSG_BASE {  // Packet_Monitor_Member_Enter_Reply_ToResponser 0x4b9
    unsigned int m_idByChannel;  // +0xd
};
struct MemberEnterOkView : MSG_BASE {  // Packet_Monitor_Notice_Member_Enter_Ok 0x4ba
    unsigned int m_idByChannel;  // +0xd
};
struct MemberSecedeView : PacketHeader {             // = Packet_Monitor_Member_Secede_To_Seceder（0x4bc，Monitor/DNFPacketTranslater.h）
    unsigned int m_idByChannel;  // +0xa
};
struct DeleteMemberIdView : PacketHeader {           // = Packet_Monitor_Notice_Delete_Member_Id（0x4bd，Monitor/DNFPacketTranslater.h）
    unsigned int m_idByChannel;  // +0xa
};
struct ReqMemberEnterToRespView : PacketHeader {     // = Packet_Monitor_Request_Member_Enter_To_Responser（0x4b8，Monitor/DNFPacketTranslater.h）
    unsigned int m_idByChannel;  // +0xa
};
struct UserRepelView : PacketHeader {                // = Packet_Monitor_User_Repel（0x4c1，Monitor/DNFPacketTranslater.h）
    unsigned int m_idByChannel;  // +0xa
};
struct TowerRankingView : PacketHeader  {             // = Packet_Request_Charac_Tower_Ranking（0x4cb，Monitor/DNFPacketTranslater.h）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_idByChannel;  // +0xe
};
struct MemberExpLevelUpView : PacketHeader {         // = Packet_Monitor_Notice_MemberExp_LevelUp（0x642，Monitor/DNFPacketTranslater.h）
    unsigned int m_idByChannel;  // +0xa
};
struct PowerWarEndView : PacketHeader {              // = Packet_Notice_Power_war_End_Info（0x6b0，Guild/PowerManager.h）
    unsigned int m_idByChannel;  // +0xa
};
struct ChannelGuildAgitView : PacketHeader {         // = Packet_Channel_Guild_Agit_Info（0x6e2，Guild/DNFGuild.h）
    unsigned int m_channel;      // +0xa（GetIdByChannel）
};
struct NoticeGuildCargoView : PacketHeader {         // = Packet_Notice_Guild_Cargo（0x712，Guild/GuildCargo.h）
    unsigned int m_a;            // +0xa
};
struct CleanPadPointView : PacketHeader {            // = Packet_Set_CleanPad_Point（0xb60，Monitor/DNFPacketTranslater.h）
    unsigned int m_idByChannel;  // +0xa
};
struct PunishCancelView : PacketHeader {             // = Packet_Punish_Cancel（0xb64，Monitor/DNFPacketTranslater.h）
    unsigned int m_idByChannel;  // +0xa
};
struct SecuConnectWebView : PacketHeader {           // = Packet_SecuService_Connect_Web（0xb65，Monitor/DNFPacketTranslater.h）
    unsigned int m_idByChannel;  // +0xa
};
struct FactoryHubUserView : PacketHeader {           // = Packet_Notice_Find_Factory_Hub_User（0x100f，Monitor/DNFPacketTranslater.h）
    unsigned int m_idByChannel;  // +0xa
};
struct VillageRewardView : PacketHeader {            // = Packet_VillageAttackedReward（0x1775，Monitor/DNFPacketTranslater.h）
    unsigned int m_idByChannel;  // +0xa
};
struct VillageScoreView : PacketHeader {             // = Packet_VillageAttackedScore（0x1778，Monitor/DNFPacketTranslater.h）
    unsigned int m_idByChannel;  // +0xa
};
struct GuildFundResultView : PacketHeader  {          // = Packet_Add_Guild_Fund_Result
    unsigned char ma;            // +0xa
    unsigned int mb;             // +0xb
    unsigned int mf;             // +0xf
};
struct TodayGuildMemberView : PacketHeader  {         // = Packet_Notify_Today_Guild_Member（0x1bc1，Guild/DNFGuild.h）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    unsigned int m_channel;      // +0x12
};
struct ApproveJoinResultView : PacketHeader  {        // = Packet_Approve_Join_Guild_Result（0x1bc3，Guild/DNFPacketTranslater.h）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    unsigned int m12;            // +0x12
};
struct AttendanceReplyView : PacketHeader  {          // = Packet_Guild_Attendance_Info_Reply（0x1bc7，Guild/PacketCounter.h）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_b;            // +0xe
};
struct AchieveAttendanceView : PacketHeader  {        // = Packet_Achieve_Guild_Attendance（0x1bc8，Guild/DNFGuild.h）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_channel;      // +0xe（接收者 GetIdByChannel）
};
struct TimeSyncLoginView : PacketHeader  {            // = Packet_Send_Time_Sync_For_Login（0x1f4c，Monitor/DNFPacketTranslater.h）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_idByChannel;  // +0xe
};
struct GuildBoardReplyView : PacketHeader  {          // = Packet_Guild_Reply_Guild_Board
    unsigned char m_a;           // +0xa
    unsigned int m_b;            // +0xb
    unsigned int m_f;            // +0xf
};
struct CollectRewardView : PacketHeader {            // = Packet_CollectItemsReward（0x27e8，Monitor/DNFPacketTranslater.h）
    unsigned int m_idByChannel;  // +0xa
};
struct GuildMarkChangeView : PacketHeader {          // = Packet_Monitor_Notice_Guild_Mark_Change_ToUser（0x3ff，Guild/DNFGuild.h）
    int m_channel;               // +0xa（GetIdByChannel）
    unsigned int m_charNo;             // +0xe（GetUniqCharNo）
    unsigned int m_charNo2;            // +0x12（变更标记角色 charNo；ORIG 日志误标 guildkey）
};
struct SetGuildKeyView : PacketHeader {              // = Packet_Monitor_Set_Guild_Key（0x40b，Guild/DNFUser.h）
    int m_channel;               // +0xa
};
struct GuildMemberMemoView : PacketHeader {          // = Packet_Guild_Notify_Guild_Member_Memo（0x4d2，Guild/DNFGuild.h）
    int m_channel;               // +0xa（GetIdByChannel）
};
struct BlackListNoticeView : PacketHeader  {          // = Packet_Monitor_Notice_Black_List
    unsigned int m_dbid;         // +0xa
    unsigned int m_idByChannel;  // +0xe
};
#pragma pack(pop)

class DB_ReloadAutoPunishRuleBackIP
{
public:
    static void makeRequest();
};

class DB_ReloadAutoPunishRuleHackType
{
public:
    static void makeRequest();
};

#include "DispatchServerPacket/dispatchPacket.inc"
