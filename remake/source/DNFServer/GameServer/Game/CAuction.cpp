// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAuction/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

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
// 0xb54 case 使用（权威定义在 GlobalData.cpp，本 TU 局部声明保持一致，避免 ODR 冲突）
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

// ---- 拍卖服务器注册/就绪检查包（ORIG 弱符号 ctor，继承 PACKET_HEADER）----
// [DNF-FIX 2026-10-08 c30] ORIG 0x082f7b62/0x082f7c72 `mov %eax,-0xc(%ebp)`（packet@-0x1e）⇒ gc_no 在
//   +0x12（包长 0x16）；无 pack 时 int 4 对齐落 +0x14，只发出低 2 字节且错位。
#pragma pack(push, 1)
struct PCK_AUCTION_REGIST_GA : public nsl::PACKET_HEADER
{
#if !remake_verify
    // DNF-FIX 2026-09-18: ORIG 包长 0x16 = 0x12(头) + 4；转写时漏了尾部 4 字节成员，
    // 于是 ctor 的 memset(this,0,0x16) 写出结构外 4 字节 → 打坏调用方栈保护
    // （core: CAuctionServerProxy::SendRegistPacket → __stack_chk_fail）。
    int gc_no;
#endif
    PCK_AUCTION_REGIST_GA() { PACKET_CTOR_BODY(0, 0, 0x16); }
};
struct PCK_AUCTION_REGIST_GP : public nsl::PACKET_HEADER
{
#if !remake_verify
    // DNF-FIX 2026-09-18: ORIG 包长 0x16 = 0x12(头) + 4；转写时漏了尾部 4 字节成员，
    // 于是 ctor 的 memset(this,0,0x16) 写出结构外 4 字节 → 打坏调用方栈保护
    // （core: CAuctionServerProxy::SendRegistPacket → __stack_chk_fail）。
    int gc_no;
#endif
    PCK_AUCTION_REGIST_GP() { PACKET_CTOR_BODY(0x12, 0, 0x16); }
};
#pragma pack(pop)
#if !remake_verify
static_assert(sizeof(PCK_AUCTION_REGIST_GA) == 0x16 && sizeof(PCK_AUCTION_REGIST_GP) == 0x16,
              "PCK_AUCTION_REGIST_GA/GP size (c30)");
#endif
struct PCK_AUCTION_CHECK_AUCTION_READY_GA : public nsl::PACKET_HEADER
{
    PCK_AUCTION_CHECK_AUCTION_READY_GA() { PACKET_CTOR_BODY(0, 0xd, 0x12); }
};
struct PCK_AUCTION_CHECK_AUCTION_READY_GP : public nsl::PACKET_HEADER
{
    PCK_AUCTION_CHECK_AUCTION_READY_GP() { PACKET_CTOR_BODY(0x12, 0xd, 0x12); }
};

// ============================================================================
// CAuctionServerProxy（ORIG 0x82f7a90 起，BaseServerProxy 派生）
// ============================================================================
class CAuctionServerProxy : public BaseServerProxy
{
public:
    CAuctionServerProxy(char* ip, int port, char* name);
    ~CAuctionServerProxy();

    virtual void OnDisconnect();
    virtual bool Parsing(int recvLen);

    void Init();
    void SendPacket(nsl::PACKET_HEADER* packet);
    void SendRegistPacket();
    void SendCheckAuctionReadyPacket();
    bool IsRunning();
    void SetRunning(bool flag);

    bool m_running;   // +0x30e0c
};

#include "CAuction/CAuctionServerProxy.inc"


#include "CAuction/dtor_CAuctionServerProxy.inc"


#include "CAuction/Init.inc"


#include "CAuction/OnDisconnect.inc"


#include "CAuction/Parsing.inc"


#include "CAuction/SendPacket.inc"


#include "CAuction/SendRegistPacket.inc"


#include "CAuction/SendCheckAuctionReadyPacket.inc"
{
    PCK_AUCTION_CHECK_AUCTION_READY_GA packet;
    SendPacket((nsl::PACKET_HEADER*)&packet);
}

#include "CAuction/IsRunning.inc"
{
    return m_running;
}

#include "CAuction/SetRunning.inc"
{
    m_running = flag;
}

// ============================================================================
// CCeraAuctionServerProxy（ORIG 0x82f7ba0 起）
// ============================================================================
class CCeraAuctionServerProxy : public BaseServerProxy
{
public:
    CCeraAuctionServerProxy(char* ip, int port, char* name);
    ~CCeraAuctionServerProxy();

    virtual void OnDisconnect();
    virtual bool Parsing(int recvLen);

    void Init();
    void SendPacket(nsl::PACKET_HEADER* packet);
    void SendRegistPacket();
    void SendCheckCeraAuctionReadyPacket();
    bool IsRunning();
    void SetRunning(bool flag);

    bool m_running;   // +0x30e0c
};

#include "CAuction/CCeraAuctionServerProxy.inc"


#include "CAuction/dtor_CCeraAuctionServerProxy.inc"


#include "CAuction/Init_1.inc"


#include "CAuction/OnDisconnect_1.inc"


#include "CAuction/Parsing_1.inc"


#include "CAuction/SendPacket_1.inc"


#include "CAuction/SendRegistPacket_1.inc"


#include "CAuction/SendCheckCeraAuctionReadyPacket.inc"


#include "CAuction/IsRunning_1.inc"


#include "CAuction/SetRunning_1.inc"


#include "CAuction/RegistNextTimer.inc"


#include "CAuction/dispatch_sig.inc"


#include "CAuction/dtor_Timer_CheckAuctionReady.inc"

// ---------------------------------------------------------------------------
// dispatchPacket 只读布局视图：这些上游包类的权威布局在 Guild/Monitor 域头
// （DNFServer/GameServer/{Guild,Monitor}/*.h）里，与本 TU 已 include 的 Game 域
// 头存在同名类冲突（如 hyperlink_item_info / STBlackUserDBType / CPacketTranslater
// 重定义，实测编译失败），故沿用本文件 GAxxxView 先例：pack(1) 局部视图，字段
// 名/类型/偏移逐字抄自权威头（证据见各注释），ORIG 指令为 `mov 0xN(%reg)` 4B 整字
// 读取（docs/class_func_reports/CDispatchServerPacket/dispatchPacket.md）。
// ---------------------------------------------------------------------------
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
struct GuildCreateToUserView : PacketHeader  {        // = Packet_Monitor_Notice_Guild_Create_ToUser（0x40c，Guild/DNFPacketTranslater.h）
    char m_afterHdr[0x1f];
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
struct GuildFundResultView : PacketHeader  {          // = Packet_Add_Guild_Fund_Result（0x1bbd，Guild/DNFPacketTranslater.h）
    char m_afterHdr[0x5];
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
struct GuildBoardReplyView : PacketHeader  {          // = Packet_Guild_Reply_Guild_Board（0x2328，Guild/PacketCounter.h）
    char m_afterHdr[0x5];
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
struct BlackListNoticeView : PacketHeader  {          // = Packet_Monitor_Notice_Black_List（0x5e2，Monitor/DNFPacketTranslater.h）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_idByChannel;  // +0xe
};
#pragma pack(pop)

// [SplitC 2026-09-11 batch3 · TU 迁移] dispatchPacket 已迁出至 DispatchServerPacket.cpp（ORIG GI 键对齐）。



// ---------------------------------------------------------------------------
// dispatchAuctionPacket / dispatchPointPacket 载荷字段视图（pack(1)）。
// 偏移与宽度以 ORIG 读取指令为准（movl/movzwl/movzbl + disp，报告
// docs/class_func_reports/CDispatchServerPacket/dispatch{Auction,Point}Packet.md）。
// ORIG(gcc4.1) 将具名成员访问折叠为单条 `mov disp(%eax),%reg`；-O0 gcc4.4 对
// 裸 *(T*)(data+K) 指针算术固定产生 add+load 两步（P30），故此处以视图字段
// 表达线格式载荷（§4.3 具名字段方向；P82/P99 先例）。视图仅描述协议字节布局，
// 不参与对象生命周期。
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
struct GARegistResultView : PacketHeader  {           // case 0（GA/GP 共用）：+0x0e 结果码
    unsigned int m_fieldA;  // +0xa
    int m_result;           // +0xe
};
struct GAAskPriceView : PacketHeader  {               // case 2：+0x12/+0x16/+0x1a
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    int m_msgSeq;
    int m_characNo;
    int m_itemIdx;
};
struct GAPriceSrcView : PacketHeader  {               // case 2 价格源：data+0xe+4k
    unsigned int m_fieldA;  // +0xa
    int m_src[10];                    // priceCur[i]=m_src[i+4]（+0x1e+4i）
                                      // priceTop[i]=m_src[i+7]（ORIG 读 +0x2a+4i 实证）
};
struct GARegistedNumView : PacketHeader  {            // case 3：+0x1a ushort/+0x1c byte/+0x1d ushort/+0x1f nums
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    int m_msgSeq;
    int m_characNo;
    unsigned short m_count;
    unsigned char m_flagA;
    unsigned short m_flagB;
    int m_nums[8];                    // AG 读 8 条；PG 仅前 4 条（ORIG 实测）
};
struct GARegistView : PacketHeader  {                 // case 4 / case 6：+0x1a/+0x1b 单字节
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    int m_msgSeq;
    int m_characNo;
    unsigned char m_okFlag;
    unsigned char m_errCode;
};
struct GABiddingView : PacketHeader  {                // case 5（PG 使用 m_seller/m_field48）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    int m_msgSeq;
    int m_characNo;
    int m_money;
    unsigned char m_flagA;
    unsigned char m_flagB;
    int m_field20;
    int m_bidPrice;
    char m_seller[0x20];              // +0x28（strncpy 源，仅 PG 路径）
    int m_field48;                    // +0x48（仅 PG 路径）
};
struct GAItemListHeadView : PacketHeader  {           // case 7：+0x1a byte/+0x1b uint/+0x1f ushort 计数
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    int m_msgSeq;
    int m_characNo;
    unsigned char m_b1a;
    unsigned int m_u1b;
    unsigned short m_cnt1f;
    char m_items[1];                  // +0x21 条目区基址
};
struct GAMyRegistHeadView : PacketHeader  {           // case 8：+0x1b byte 计数（PG 拷贝源含该字节）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    int m_msgSeq;
    int m_characNo;
    unsigned char m_b1a;
    unsigned char m_cnt1b;
    char m_items[1];                  // +0x1c 条目区基址
};
struct GAMyBiddingHeadView : PacketHeader  {          // case 9：+0x1b ushort 计数
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    int m_msgSeq;
    int m_characNo;
    unsigned char m_b1a;
    unsigned short m_cnt1b;
    char m_items[1];                  // +0x1d 条目区基址
};
struct GALogMsgView : PacketHeader  {                 // case 0xb：1 字节型 + 8B 主键（两段 4B 写）
    unsigned int m_fieldA;  // +0xa
    unsigned int m_fieldE;  // +0xe
    int m_msgSeq;                     // +0x12
    unsigned char m_type;             // +0x16
    unsigned int m_keyLo;             // +0x17（同型化：dst m_keyWords[] 为 unsigned）
    unsigned int m_keyHi;             // +0x1b
    unsigned int m_a1f;               // 同型化：ORIG `mov 0x1f(%eax),%edx` 一段直传（P211）
    int m_a23;
    unsigned int m_a27;               // 同型化：ORIG `mov 0x27(%eax),%edx` 一段直传
    unsigned char m_b2b;              // +0x2b
    int m_i2c;
    unsigned short m_w30;             // +0x30
    int m_i32;                        // +0x32
    unsigned char m_b36;              // +0x36（仅 AG 路径写）
};
struct GAReadyFlagView : PacketHeader  {              // case 0xd：+0x12 单字节就绪标志
    unsigned int m_fieldA;
    unsigned int m_fieldE;
    unsigned char m_ready;
};
struct GABuyOneView : PacketHeader  {                 // case 0xe（AG 单件购买回执）
    unsigned int m_fieldA;
    unsigned int m_fieldE;
    int m_s12;
    int m_s16;
    int m_s1a;
    unsigned char m_b1e;
    unsigned char m_b1f;
    int m_s20;
    int m_s24;
};
struct GAVipView : PacketHeader  {                    // case 0xe（PG OWNER_IS_VIP）
    unsigned int m_fieldA;
    unsigned int m_fieldE;
    int m_s12;
    int m_s16;
    int m_s1a;
    unsigned char m_b1e;
    int m_s1f;
    int m_s23;
    char m_owner[0xd];                // +0x27（strcpy 源）
    int m_s34;
    int m_s38;
};
struct GAPassThruView : PacketHeader { // case 0x2718..0x271b 转发
    int m_s0a;                        // +0xa
    char m_padE;                      // +0xe
    int m_s0f;                        // +0xf
};
#pragma pack(pop)

#include "CAuction/dispatchAuctionPacket.inc"


#include "CAuction/dispatchPointPacket.inc"

