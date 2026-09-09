// ============================================================================
// df_game_r 还原 —— 拍卖域代理/发包（G4 商城/交易批次）
// 参照 docs/class_func_reports/CAuctionServerProxy.md、
// CCeraAuctionServerProxy.md、CDispatchServerPacket.md + ORIG 反汇编。
// CAuctionAveragePrice 由 CAuctionAveragePrice.cpp 提供。
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
struct PCK_AUCTION_REGIST_GA : public nsl::PACKET_HEADER
{
    PCK_AUCTION_REGIST_GA() { PACKET_CTOR_BODY(0, 0, 0x16); }
};
struct PCK_AUCTION_REGIST_GP : public nsl::PACKET_HEADER
{
    PCK_AUCTION_REGIST_GP() { PACKET_CTOR_BODY(0x12, 0, 0x16); }
};
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

CAuctionServerProxy::CAuctionServerProxy(char* ip, int port, char* name)
    : BaseServerProxy(ip, port, name)
{
    m_running = 0;
    Init();
}

// [b22判词·AE] CAuctionServerProxy D1Ev（ORIG 0x82aa9xx，orig10/our10）：空体正确，
// 编译器自动发 vptr=&CAuctionServerProxy vtable（ORIG `movl $0x8c1ec10,(%eax)`，未 relink
// .o 显占位 `$0x8`）+ call _ZN15BaseServerProxyD1Ev（P167 实测 1=1）。指令/助记符逐条同
// ORIG，残余唯 vtable 绝对址 + call 目标两处链接期重定位占位（§6.3 假阳性），relink 后转 AE。
CAuctionServerProxy::~CAuctionServerProxy()
{
}

void CAuctionServerProxy::Init()
{
    m_running = 0;
}

void CAuctionServerProxy::OnDisconnect()
{
    Init();
}

bool CAuctionServerProxy::Parsing(int recvLen)
{
    return GetTcpHandler()->Parsing_AuctionServerPacket(recvLen);
}

void CAuctionServerProxy::SendPacket(nsl::PACKET_HEADER* packet)
{
    SendTcpPacket((char*)packet, (int)packet->getSize());
}

void CAuctionServerProxy::SendRegistPacket()
{
    PCK_AUCTION_REGIST_GA packet;
    int envField = G_CEnvironment()->m_serverEnvir.m_gcNo;
    SendPacket((nsl::PACKET_HEADER*)&packet);
}

void CAuctionServerProxy::SendCheckAuctionReadyPacket()
{
    PCK_AUCTION_CHECK_AUCTION_READY_GA packet;
    SendPacket((nsl::PACKET_HEADER*)&packet);
}

bool CAuctionServerProxy::IsRunning()
{
    return m_running;
}

void CAuctionServerProxy::SetRunning(bool flag)
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

CCeraAuctionServerProxy::CCeraAuctionServerProxy(char* ip, int port, char* name)
    : BaseServerProxy(ip, port, name)
{
    m_running = 0;
    Init();
}

// [b22判词·AE] CCeraAuctionServerProxy D1Ev（ORIG 0x82aa9c8，orig10/our10）：空体正确，
// 编译器自动发 vptr=&CCeraAuctionServerProxy vtable（ORIG `movl $0x8c1ebf8,(%eax)`，未
// relink .o 显占位 `$0x8`）+ call _ZN15BaseServerProxyD1Ev（P167 实测 1=1）。逐条同 ORIG，
// 残余唯 vtable 址 + call 目标两处链接期重定位占位（§6.3 假阳性），relink 后转 AE。
CCeraAuctionServerProxy::~CCeraAuctionServerProxy()
{
}

void CCeraAuctionServerProxy::Init()
{
    m_running = 0;
}

void CCeraAuctionServerProxy::OnDisconnect()
{
    Init();
}

bool CCeraAuctionServerProxy::Parsing(int recvLen)
{
    return GetTcpHandler()->Parsing_AuctionServerPacket(recvLen);
}

void CCeraAuctionServerProxy::SendPacket(nsl::PACKET_HEADER* packet)
{
    SendTcpPacket((char*)packet, (int)packet->getSize());
}

void CCeraAuctionServerProxy::SendRegistPacket()
{
    PCK_AUCTION_REGIST_GP packet;
    int envField = G_CEnvironment()->m_serverEnvir.m_gcNo;
    SendPacket((nsl::PACKET_HEADER*)&packet);
}

void CCeraAuctionServerProxy::SendCheckCeraAuctionReadyPacket()
{
    PCK_AUCTION_CHECK_AUCTION_READY_GP packet;
    SendPacket((nsl::PACKET_HEADER*)&packet);
}

bool CCeraAuctionServerProxy::IsRunning()
{
    return m_running;
}

void CCeraAuctionServerProxy::SetRunning(bool flag)
{
    m_running = flag;
}

// ============================================================================
// CDispatchServerPacket（拍卖/点券服务器回包分发，ORIG 0x8473660/0x847ac3c/0x847c1fe）
// 语义：从拍卖/点券代理收包，按 packetID 组 SIG 流转投 DISPATCH_Q。
// ============================================================================
// ORIG：dispatch_sig T 0x863a808、RegistNextTimer T 0x863a858、
//   D1/D2 W 0x863e8ce、D0 W 0x863e8fe；无 C1（类从未被构造）。
// 类声明见 TimerDispatcher.h（TimerSig 派生，对象尺寸 4：仅 vptr）。
// dispatch_sig：两代理 IsRunning()==false 时分别补发就绪检查包，恒返 1。
// ---------------------------------------------------------------------------
void Timer_CheckAuctionReady::RegistNextTimer()
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                static_cast<TIMER_MESSAGE>(0x98), 0x3c, 0, 0);
}

int Timer_CheckAuctionReady::dispatch_sig(int p1, int p2, unsigned int p3)
{
    if (!GlobalData::s_auction_proxy->IsRunning())
    {
        GlobalData::s_auction_proxy->SendCheckAuctionReadyPacket();
    }
    if (!GlobalData::s_cera_auction_proxy->IsRunning())
    {
        GlobalData::s_cera_auction_proxy->SendCheckCeraAuctionReadyPacket();
    }
    return 1;
}

Timer_CheckAuctionReady::~Timer_CheckAuctionReady() {}

// ---------------------------------------------------------------------------
// dispatchPacket 只读布局视图：这些上游包类的权威布局在 Guild/Monitor 域头
// （DNFServer/GameServer/{Guild,Monitor}/*.h）里，与本 TU 已 include 的 Game 域
// 头存在同名类冲突（如 hyperlink_item_info / STBlackUserDBType / CPacketTranslater
// 重定义，实测编译失败），故沿用本文件 GAxxxView 先例：pack(1) 局部视图，字段
// 名/类型/偏移逐字抄自权威头（证据见各注释），ORIG 指令为 `mov 0xN(%reg)` 4B 整字
// 读取（docs/class_func_reports/CDispatchServerPacket/dispatchPacket.md）。
// ---------------------------------------------------------------------------
#pragma pack(push, 1)
struct GuildKeyIntView {              // 0x3fc 等：+0xa 公会键 int 视角（字段 unsigned，ORIG 直落 int 实参）
    char m_pad0[0xa];   int m_guildKey;
};
struct IntView0a { char m_pad0[0xa]; int m_v; };
struct IntView0b { char m_pad0[0xb]; int m_v; };
struct IntView0d { char m_pad0[0xd]; int m_v; };
struct IntView0e { char m_pad0[0xe]; int m_v; };
struct IntView0f { char m_pad0[0xf]; int m_v; };
struct IntView12 { char m_pad0[0x12]; int m_v; };
struct IntView14 { char m_pad0[0x14]; int m_v; };
struct IntView29 { char m_pad0[0x29]; int m_v; };
struct UIntView0a { char m_pad0[0xa]; unsigned int m_v; };
struct UIntView0e { char m_pad0[0xe]; unsigned int m_v; };
struct UIntView0f { char m_pad0[0xf]; unsigned int m_v; };
struct UIntView12 { char m_pad0[0x12]; unsigned int m_v; };
struct UIntView14 { char m_pad0[0x14]; unsigned int m_v; };
struct UCharView0a { char m_pad0[0xa]; unsigned char m_v; };

struct RevengeView {                  // 0x1b6a：+0xa 奖励类型 int，+0xe 关闭时间 uint
    char m_pad0[0xa];   int m_reward;   unsigned int m_closeTime;
};
struct NoticeGuildSecedeView {        // = Packet_Monitor_Notice_Guild_Secede_ToUser（0x3fe，Guild/DNFGuild.h）
    char m_pad0[0xa];    unsigned int m_channel;   // +0xa（GetIdByChannel）
};
struct GuildMemberLoginOutView {      // = Packet_Monitor_Notice_Guild_Member_Login_out（0x407，Guild/DNFGuild.h）
    char m_pad0[0xb];    int m_channel;            // +0xb
};
struct NoticeGuildInfoView {          // = Packet_Monitor_Notice_Guild_Info（0x40a，Guild/DNFGuild.h）
    char m_pad0[0xa];    unsigned int m_channel;   // +0xa（GetIdByChannel）
};
struct GuildCreateToUserView {        // = Packet_Monitor_Notice_Guild_Create_ToUser（0x40c，Guild/DNFPacketTranslater.h）
    char m_pad0[0x29];   unsigned int m_member;    // +0x29
};
struct GuildMemberGradeView {         // = Packet_Monitor_Notify_GuildMemberGrade（0x42c，Guild/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_channel;   // +0xa（GetIdByChannel）
};
struct GuildMemberInfoView {          // = Packet_Monitor_Notice_Guild_Member_Info（0x431，Guild/DNFUser.h）
    char m_pad0[0xa];    int m_channel;            // +0xa
};
struct GuildInviteToInvitedView {     // = Packet_Guild_Call_Guild_Invite_To_Invited（0x435，Guild/DNFPacketTranslater.h）
    char m_pad0[0xe];    unsigned int m_e;         // +0xe
};
struct GuildInviteToCallerView {      // = Packet_Guild_Reply_Guild_Invite_To_Caller（0x437，Guild/DNFPacketTranslater.h）
    char m_pad0[0xe];    unsigned int m_e;         // +0xe
};
struct GuildMasterDelegateView {      // = Packet_Guild_Notice_Guild_Master_Delegate（0x43d，Guild/DNFGuild.h）
    char m_pad0[0xe];    int m_channel;            // +0xe
};
struct HasBeenGuildMasterView {       // = Packet_Notice_Has_Been_Guild_Master（0x43e，Guild/DNFPacketTranslater.h）
    char m_pad0[0xe];    unsigned int m_channel;   // +0xe
};
struct GuildNameChangeView {          // = Packet_Guild_Notice_Guild_Name_Change（0x446，Guild/DNFGuild.h）
    char m_pad0[0x12];   unsigned int m_channel;   // +0x12
};
struct AllUserMinForGuildView {       // = Packet_Send_All_User_Info_Minimum_For_Guild_System（0x447，Guild/DNFUser.h）
    char m_pad0[0x12];   int m_channel;            // +0x12
};
struct MonitorMemberLoginOutView {    // = Packet_Monitor_Notice_Member_Member_Login_out（0x4b6，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xb];    unsigned int m_idByChannel;  // +0xb
};
struct MemberEnterReplyView {         // = Packet_Monitor_Member_Enter_Reply_ToResponser（0x4b9，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xd];    unsigned int m_idByChannel;  // +0xd
};
struct MemberEnterOkView {            // = Packet_Monitor_Notice_Member_Enter_Ok（0x4ba，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xd];    unsigned int m_idByChannel;  // +0xd
};
struct MemberSecedeView {             // = Packet_Monitor_Member_Secede_To_Seceder（0x4bc，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct DeleteMemberIdView {           // = Packet_Monitor_Notice_Delete_Member_Id（0x4bd，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct ReqMemberEnterToRespView {     // = Packet_Monitor_Request_Member_Enter_To_Responser（0x4b8，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct UserRepelView {                // = Packet_Monitor_User_Repel（0x4c1，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct TowerRankingView {             // = Packet_Request_Charac_Tower_Ranking（0x4cb，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xe];    unsigned int m_idByChannel;  // +0xe
};
struct MemberExpLevelUpView {         // = Packet_Monitor_Notice_MemberExp_LevelUp（0x642，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct PowerWarEndView {              // = Packet_Notice_Power_war_End_Info（0x6b0，Guild/PowerManager.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct ChannelGuildAgitView {         // = Packet_Channel_Guild_Agit_Info（0x6e2，Guild/DNFGuild.h）
    char m_pad0[0xa];    unsigned int m_channel;      // +0xa（GetIdByChannel）
};
struct NoticeGuildCargoView {         // = Packet_Notice_Guild_Cargo（0x712，Guild/GuildCargo.h）
    char m_pad0[0xa];    unsigned int m_a;            // +0xa
};
struct CleanPadPointView {            // = Packet_Set_CleanPad_Point（0xb60，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct PunishCancelView {             // = Packet_Punish_Cancel（0xb64，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct SecuConnectWebView {           // = Packet_SecuService_Connect_Web（0xb65，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct FactoryHubUserView {           // = Packet_Notice_Find_Factory_Hub_User（0x100f，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct VillageRewardView {            // = Packet_VillageAttackedReward（0x1775，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct VillageScoreView {             // = Packet_VillageAttackedScore（0x1778，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct GuildFundResultView {          // = Packet_Add_Guild_Fund_Result（0x1bbd，Guild/DNFPacketTranslater.h）
    char m_pad0[0xf];    unsigned int mf;             // +0xf
};
struct TodayGuildMemberView {         // = Packet_Notify_Today_Guild_Member（0x1bc1，Guild/DNFGuild.h）
    char m_pad0[0x12];   unsigned int m_channel;      // +0x12
};
struct ApproveJoinResultView {        // = Packet_Approve_Join_Guild_Result（0x1bc3，Guild/DNFPacketTranslater.h）
    char m_pad0[0x12];   unsigned int m12;            // +0x12
};
struct AttendanceReplyView {          // = Packet_Guild_Attendance_Info_Reply（0x1bc7，Guild/PacketCounter.h）
    char m_pad0[0xe];    unsigned int m_b;            // +0xe
};
struct AchieveAttendanceView {        // = Packet_Achieve_Guild_Attendance（0x1bc8，Guild/DNFGuild.h）
    char m_pad0[0xe];    unsigned int m_channel;      // +0xe（接收者 GetIdByChannel）
};
struct TimeSyncLoginView {            // = Packet_Send_Time_Sync_For_Login（0x1f4c，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xe];    unsigned int m_idByChannel;  // +0xe
};
struct GuildBoardReplyView {          // = Packet_Guild_Reply_Guild_Board（0x2328，Guild/PacketCounter.h）
    char m_pad0[0xf];    unsigned int m_f;            // +0xf
};
struct CollectRewardView {            // = Packet_CollectItemsReward（0x27e8，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xa];    unsigned int m_idByChannel;  // +0xa
};
struct GuildMarkChangeView {          // = Packet_Monitor_Notice_Guild_Mark_Change_ToUser（0x3ff，Guild/DNFGuild.h）
    char m_pad0[0xa];    int m_channel;               // +0xa（GetIdByChannel）
    unsigned int m_charNo;             // +0xe（GetUniqCharNo）
    unsigned int m_charNo2;            // +0x12（变更标记角色 charNo；ORIG 日志误标 guildkey）
};
struct SetGuildKeyView {              // = Packet_Monitor_Set_Guild_Key（0x40b，Guild/DNFUser.h）
    char m_pad0[0xa];    int m_channel;               // +0xa
};
struct GuildMemberMemoView {          // = Packet_Guild_Notify_Guild_Member_Memo（0x4d2，Guild/DNFGuild.h）
    char m_pad0[0xa];    int m_channel;               // +0xa（GetIdByChannel）
};
struct BlackListNoticeView {          // = Packet_Monitor_Notice_Black_List（0x5e2，Monitor/DNFPacketTranslater.h）
    char m_pad0[0xe];    unsigned int m_idByChannel;  // +0xe
};
#pragma pack(pop)

// §6.2 巨件核销 [b28d-solo-CAuction]：verdict DIFF（N82 全帧槽号错位，不可复现）。
// 三件套：①gap ORIG 6348 / OUR 6346（+2）；②P167 调用点差集 1245/1245 [0缺0多]
// （含 0x9de put idx5、0xfa5/0x1b63/0x1b64/0x1b65 DB_Q(idx2)、0xb54 双 DB
// makeRequest、0x3ff/0x2712/0x2713 cMyTrace、0x27f9/0x27fa make_internal_stream_jpn）；
// ③cmp5 按 switch 决策树配对 188 case，实质差异 0（余 6 处伪差：printf@plt 后缀、
// __PRETTY_FUNCTION__ 本地 r 符号 vs .rodata 重定位，链接后同址）。
// 本波修正：
// ① 0xb65 缺 fall-through 真语义 bug：ORIG 0847a316 put 后无 jmp，直落 0x2712 的
//    cMyTrace 块（Ghidra C LAB_0847a31b 双入口实证）→ 去 break 并并入 0x2712 块；
// ② 0x1b6a 具名局部槽 → switch(视图字段) + RevengeView（P251/P0）；
// ③ 67 处字段读取形态：裸偏移 *(int*)(d+N)（4.4 恒 add+load，P30/N102）与冗余
//    (int) cast（4.4 两段 eax→ebx 中转，P265/P211 档③）→ IntView0a..29/UIntView
//    具名视图；14 处 ORIG 两段形用 unsigned 视图 + (int) cast 复现；
// ④ 0xb54 跳转极性反演（jne/je 落点，P59/P222）+ 字节视图 UCharView0a。
// [BigAuction 复核修正] 旧判"N82 全帧槽号错位不可复现"系误诊：case 数据槽 174 个
// 两侧本就 1:1 对齐，真因是**本函数 case 块声明序与 ORIG 4.1 body 布局序不一致**
// （ORIG body 序非数值序：0x3e8,0x3ea,0x3f9,0x3ee,0x27e2,0x3f8,0x5e0,...）。
// 本波按 ORIG 决策树 je→body 入口地址序重排 186 个 case 块后 slot↔case 175/175
// 全中（验证脚本 /tmp/disp_verify.py），call 目标多重集 1245/1245 0缺0多、
// cmp $imm 多重集 253/253 0缺0多（Main 要求的方法证明）。
// 本波再修正（N82 判词作废）：
// ① guard/trace 帧底槽全部对上 ORIG（-0x310/-0x308/-0x2f8/-0x2e8）：翻法 = 把
//    0xdbf/0xdcc 两处 named cMyTrace 局部改单表达式临时链形（P227 同族）——4.4
//    临时槽与 named 槽分组排布，temporize 后 guard 自动落最低槽；
// ② 池选择中段 4.1 双 cmp 形可翻：else-if 条件写全 `payloadLen >= 0x1000 &&
//    payloadLen <= 0x3fff`（ORIG jbe 死分支 = `>=` 假路，语义不变），4.4 产出
//    同款 cmpw $0xfff; jbe 冗余对；
// ③ 残余仅链接伪差：printf@plt 后缀、.o 尾部对齐 nop（relink 即消）。
bool CDispatchServerPacket::dispatchPacket(char* data)
{
    // ORIG 0x8473660：上游服务器包按首 short(sig) 重写头并转投 DISPATCH_Q。
    unsigned short payloadLen = ((PacketHeader*)data)->packetSize;
    payloadLen += 8;
    CStreamGuard guard(true);
    if (payloadLen < 0x1000)
    {
        guard.Set(GlobalData::s_small_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x36));
    }
    else if (payloadLen >= 0x1000 && payloadLen <= 0x3fff)  // 4.1 双 cmp 形（ORIG 重验边界）
    {
        guard.Set(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x3a));
    }
    else
    {
        guard.Set(GlobalData::s_big_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x3e));
    }
    switch (((PacketHeader*)data)->packetId)
    {
        case 0x3e8:
        {
        char* d = data;
        guard.operator*()->operator<<(65);
        guard.operator*()->operator<<((((Packet_Monitor_UDP_Login*)d)->m_channelId));
        guard->put_binary(d, 51);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

    case 0x3ea:
    {
        char* d = data;
        guard.operator*()->operator<<(66);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 11);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3f9:
        {
        char* d = data;
        guard.operator*()->operator<<(301);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3ee:
        {
        char* d = data;
        guard.operator*()->operator<<(67);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 14);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27e2:
        {
        char* d = data;
        guard.operator*()->operator<<(759);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3f8:
        {
        char* d = data;
        guard.operator*()->operator<<(289);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 11);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x5e0:
        {
        char* d = data;
        guard.operator*()->operator<<(171);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 415);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x5dc:
        {
        char* d = data;
        guard.operator*()->operator<<(172);
        guard.operator*()->operator<<((int)(((Packet_Register_To_BlackList*)d)->m_dbid));
        guard->put_binary(d, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x5dd:
        {
        char* d = data;
        guard.operator*()->operator<<(173);
        guard.operator*()->operator<<((int)(((Packet_Delete_To_BlackList*)d)->m_dbid));
        guard->put_binary(d, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3ef:
        {
        // 0x3ef：权威头 Packet_Monitor_Char_Info::m_channel 为 uchar，ORIG 此处 movl 整字读（4B），字段宽度不符，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(68);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 52);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x5e2:
        {
        char* d = data;
        guard.operator*()->operator<<(174);
        guard.operator*()->operator<<((int)(((BlackListNoticeView*)d)->m_idByChannel));
        guard->put_binary(d, 59);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3f2:
        {
        char* d = data;
        guard.operator*()->operator<<(70);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x44c:
        {
        char* d = data;
        guard.operator*()->operator<<(71);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 14);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x44d:
        {
        char* d = data;
        guard.operator*()->operator<<(74);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 34);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x40c:
        {
        char* d = data;
        guard.operator*()->operator<<(98);
        guard.operator*()->operator<<(((IntView29*)d)->m_v);
        guard->put_binary(d, 45);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x40b:
        {
        char* d = data;
        guard.operator*()->operator<<(97);
        guard.operator*()->operator<<((int)(((SetGuildKeyView*)d)->m_channel));
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x40a:
        {
        char* d = data;
        guard.operator*()->operator<<(96);
        guard.operator*()->operator<<((int)(((NoticeGuildInfoView*)d)->m_channel));
        guard->put_binary(d, 313);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3fd:
        {
        // 0x3fd：包类 Packet_Monitor_Notice_Guild_Enter 首成员为 ST_Notice_Guild_Enter 结构（Guild 域头，
        // 本 TU 不可达且无标量首字段证据），布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(86);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 90);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3fe:
        {
        char* d = data;
        guard.operator*()->operator<<(87);
        guard.operator*()->operator<<((((IntView0a*)d)->m_v));
        guard->put_binary(d, 85);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3ff:
        {
        char* d = data;
        guard.operator*()->operator<<(88);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 22);
        // ORIG put_binary 后、MsgQueueMgr::put 前 cMyTrace(0x1a4,0) 打公会徽章变更日志
        // （channel=*(int*)(+10), character=*(int*)(+0xe), guildkey=*(int*)(+0x12)）。
        // ORIG 单表达式链：日志实参先于 cMyTrace 构造求值（edi/esi/ebx 预装载）。
        cMyTrace(__PRETTY_FUNCTION__, 0x1a4, 0)(
            "[GUILD MARK CHANGE] Recv from guild server. (channel:%d, character:%u, "
            "guildkey:%u)",
            (int)(((GuildMarkChangeView*)d)->m_channel), ((IntView0e*)d)->m_v, ((IntView12*)d)->m_v);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3fc:
        {
        char* d = data;
        guard.operator*()->operator<<(89);
        guard.operator*()->operator<<((((GuildKeyIntView*)d)->m_guildKey));
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x400:
        {
        char* d = data;
        guard.operator*()->operator<<(90);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x401:
        {
        char* d = data;
        guard.operator*()->operator<<(91);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x423:
        {
        char* d = data;
        guard.operator*()->operator<<(198);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x424:
        {
        char* d = data;
        guard.operator*()->operator<<(199);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x428:
        {
        char* d = data;
        guard.operator*()->operator<<(200);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x406:
        {
        // 0x406：重建源码无该类，布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(92);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 9023);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x407:
        {
        char* d = data;
        guard.operator*()->operator<<(93);
        guard.operator*()->operator<<((int)(((GuildMemberLoginOutView*)d)->m_channel));
        guard->put_binary(d, 50);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x3f3:
        {
        char* d = data;
        guard.operator*()->operator<<(94);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Other_Channel_Chat_ToUser*)d)->m_idByChannel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x40d:
        {
        char* d = data;
        guard.operator*()->operator<<(95);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Call_Guild_Info*)d)->m_uid));
        guard->put_binary(d, 399);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x42d:
        {
        char* d = data;
        guard.operator*()->operator<<(216);
        guard.operator*()->operator<<((int)(((Packet_Web_Notify_Message_To_Guild*)d)->m_guildKey));
        guard->put_binary(d, 119);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x42f:
        {
        // 0x42f：重建源码无该类，布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(100);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x430:
        {
        char* d = data;
        guard.operator*()->operator<<(101);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Set_Sub_Guild_Master*)d)->m_guildKey));
        guard->put_binary(d, 58);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x431:
        {
        char* d = data;
        guard.operator*()->operator<<(102);
        guard.operator*()->operator<<((int)((UIntView0a*)d)->m_v);
        guard->put_binary(d, 44);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x42c:
        {
        char* d = data;
        guard.operator*()->operator<<(215);
        guard.operator*()->operator<<((int)(((GuildMemberGradeView*)d)->m_channel));
        guard->put_binary(d, 19);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4b6:
        {
        char* d = data;
        guard.operator*()->operator<<(108);
        guard.operator*()->operator<<(((IntView0b*)d)->m_v);
        guard->put_binary(d, 56);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4b7:
        {
        char* d = data;
        guard.operator*()->operator<<(109);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4b8:
        {
        char* d = data;
        guard.operator*()->operator<<(110);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 50);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4ba:
        {
        char* d = data;
        guard.operator*()->operator<<(111);
        guard.operator*()->operator<<(((IntView0d*)d)->m_v);
        guard->put_binary(d, 57);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4b9:
        {
        char* d = data;
        guard.operator*()->operator<<(112);
        guard.operator*()->operator<<(((IntView0d*)d)->m_v);
        guard->put_binary(d, 57);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4bb:
        {
        char* d = data;
        guard.operator*()->operator<<(113);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4bc:
        {
        char* d = data;
        guard.operator*()->operator<<(114);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4bd:
        {
        char* d = data;
        guard.operator*()->operator<<(115);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4bf:
        {
        char* d = data;
        guard.operator*()->operator<<(116);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4be:
        {
        char* d = data;
        guard.operator*()->operator<<(117);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 481);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4c0:
        {
        char* d = data;
        guard.operator*()->operator<<(118);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 56);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4c1:
        {
        char* d = data;
        guard.operator*()->operator<<(122);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x44f:
        {
        char* d = data;
        guard.operator*()->operator<<(133);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x450:
        {
        char* d = data;
        guard.operator*()->operator<<(134);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 14);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x44e:
        {
        char* d = data;
        guard.operator*()->operator<<(85);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x514:
        {
        char* d = data;
        guard.operator*()->operator<<(130);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x414:
        {
        char* d = data;
        guard.operator*()->operator<<(153);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 348);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x546:
        {
        char* d = data;
        guard.operator*()->operator<<(170);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x642:
        {
        char* d = data;
        guard.operator*()->operator<<(186);
        guard.operator*()->operator<<((int)(((MemberExpLevelUpView*)d)->m_idByChannel));
        guard->put_binary(d, 19);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x672:
        {
        char* d = data;
        guard.operator*()->operator<<(188);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Add_Buddy*)d)->m_idByChannel));
        guard->put_binary(d, 59);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x674:
        {
        char* d = data;
        guard.operator*()->operator<<(189);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Del_Buddy*)d)->m_idByChannel));
        guard->put_binary(d, 49);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x9dc:
        {
        char* d = data;
        guard.operator*()->operator<<(146);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 267);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x9dd:
        {
        char* d = data;
        guard.operator*()->operator<<(148);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 48);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x9e0:
        {
        char* d = data;
        guard.operator*()->operator<<(183);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 266);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x9de:
        {
        char* d = data;
        guard.operator*()->operator<<(184);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 20);
        // ORIG put(s_msgq_mgr,5,guard)：QUEUE_IDX 枚举仅命名 0..4，第 6 槽(idx=5)
        // 无队列名（MsgQueueMgr ctor 5→-1 共 6 槽），仍按 ORIG 投 idx5。
        GlobalData::s_msgq_mgr->put(static_cast<MsgQueueMgr::QUEUE_IDX>(5), guard);
    }
        break;

        case 0x3f7:
        {
        char* d = data;
        guard.operator*()->operator<<(206);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x76c:
        {
        char* d = data;
        guard.operator*()->operator<<(211);
        guard.operator*()->operator<<((int)(((Packet_GM_Request_Mid*)d)->m_gmId));
        guard->put_binary(d, 52);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x432:
        {
        char* d = data;
        guard.operator*()->operator<<(218);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Send_Guild_Mail*)d)->m_guildId));
        guard->put_binary(d, 19);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x434:
        {
        char* d = data;
        guard.operator*()->operator<<(219);
        guard.operator*()->operator<<((int)(((Packet_Guild_Call_Guild_Invite*)d)->m_guildKey));
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x435:
        {
        char* d = data;
        guard.operator*()->operator<<(220);
        guard.operator*()->operator<<((int)(((GuildInviteToInvitedView*)d)->m_e));
        guard->put_binary(d, 71);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x436:
        {
        // 0x436：权威头 m_accept 为 uchar，ORIG movl 整字读（4B），字段宽度不符，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(221);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x437:
        {
        char* d = data;
        guard.operator*()->operator<<(222);
        guard.operator*()->operator<<((int)(((GuildInviteToCallerView*)d)->m_e));
        guard->put_binary(d, 52);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4cb:
        {
        char* d = data;
        guard.operator*()->operator<<(229);
        guard.operator*()->operator<<((int)(((TowerRankingView*)d)->m_idByChannel));
        guard->put_binary(d, 98);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4cf:
    {
        guard.operator*()->operator<<(230);
        guard.operator*()->operator<<(-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x439:
        {
        // 0x439：DBMW 统计族多类共用 pid 0x439，+0x14 处字段归属存疑（Party=char / Party_Charac=uint），布局未建模。
        char* d = data;
        guard.operator*()->operator<<(234);
        guard.operator*()->operator<<((int)((UIntView14*)d)->m_v);
        guard->put_binary(d, 82);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x43d:
        {
        char* d = data;
        guard.operator*()->operator<<(231);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 48);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x43b:
        {
        char* d = data;
        guard.operator*()->operator<<(232);
        guard.operator*()->operator<<((int)(((Packet_Guild_Request_Guild_Master_Delegate*)d)->m_requesterCharNo));
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x43e:
        {
        char* d = data;
        guard.operator*()->operator<<(233);
        guard.operator*()->operator<<((int)(((HasBeenGuildMasterView*)d)->m_channel));
        guard->put_binary(d, 18);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x43f:
        {
        // 0x43f：权威头 +0xa 为 char m_guildName[]，ORIG movl 整字读（4B），字段宽度不符，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(237);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 45);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x445:
        {
        char* d = data;
        guard.operator*()->operator<<(298);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 46);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb54:
        {
        char* d = data;
        // ORIG je 8476eeb/8476f0c：d[10] 非 0 → HackType::makeRequest()，否则 BackIP::makeRequest()。
        // 0x3ea/0x3eb（DB_ReloadAutoPunishRule* 请求包）无重建包类（makePacketHeader 手拼），布局未建模，保留字节读。
        if (((UCharView0a*)d)->m_v == 0)
            DB_ReloadAutoPunishRuleBackIP::makeRequest();
        else
            DB_ReloadAutoPunishRuleHackType::makeRequest();
    }
        break;

        case 0xb5f:
        {
        char* d = data;
        guard.operator*()->operator<<(401);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 12);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x444:
        {
        // 0x444：权威头 +0x12 为 char m_name[0x1e]，ORIG 此处 movl 整字读（4B），字段宽度不符，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(291);
        guard.operator*()->operator<<((int)((UIntView12*)d)->m_v);
        guard->put_binary(d, 46);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x446:
        {
        char* d = data;
        guard.operator*()->operator<<(292);
        guard.operator*()->operator<<((int)(((GuildNameChangeView*)d)->m_channel));
        guard->put_binary(d, 45);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x447:
        {
        char* d = data;
        guard.operator*()->operator<<(388);
        guard.operator*()->operator<<((int)((UIntView12*)d)->m_v);
        guard->put_binary(d, 22);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xa8c:
        {
        char* d = data;
        guard.operator*()->operator<<(293);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 61);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb55:
    {
        guard.operator*()->operator<<(294);
        guard.operator*()->operator<<(-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb57:
    {
        guard.operator*()->operator<<(341);
        guard.operator*()->operator<<(-1);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb58:
        {
        char* d = data;
        guard.operator*()->operator<<(343);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb5e:
        {
        char* d = data;
        guard.operator*()->operator<<(381);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 1213);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xc1c:
        {
        // 0xc1c：Packet_Monitor_Notify_Auction_Mail 定义在 ServerLab Auction 域 cpp，无字段头，布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(239);
        guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        guard->put_binary(d, 38);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfa2:
        {
        char* d = data;
        guard.operator*()->operator<<(307);
        guard.operator*()->operator<<((int)(((Packet_PvPChannelInfo*)d)->m_uid));
        guard->put_binary(d, 6136);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfa3:
        {
        char* d = data;
        guard.operator*()->operator<<(308);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 42);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfa5:
        {
        char* d = data;
        guard.operator*()->operator<<(309);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 128);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(...,2,guard)
    }
        break;

        case 0xfab:
        {
        // 0xfab：重建源码无该类（ORIG 无导出符号），布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(352);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfac:
        {
        // 0xfac：重建源码无该类，布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(353);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfad:
        {
        // 0xfad：重建源码无该类，布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(354);
        guard.operator*()->operator<<(((IntView0b*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6d8:
        {
        char* d = data;
        guard.operator*()->operator<<(372);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfae:
        {
        char* d = data;
        guard.operator*()->operator<<(368);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6dc:
        {
        char* d = data;
        guard.operator*()->operator<<(369);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6de:
        {
        char* d = data;
        guard.operator*()->operator<<(370);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6e2:
        {
        char* d = data;
        guard.operator*()->operator<<(371);
        guard.operator*()->operator<<((int)(((ChannelGuildAgitView*)d)->m_channel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x4d2:
        {
        char* d = data;
        guard.operator*()->operator<<(374);
        guard.operator*()->operator<<((int)((UIntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6e0:
        {
        char* d = data;
        guard.operator*()->operator<<(378);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6b0:
        {
        char* d = data;
        guard.operator*()->operator<<(379);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfaf:
        {
        char* d = data;
        guard.operator*()->operator<<(382);
        guard.operator*()->operator<<((int)(((Packet_Pcroom_Response*)d)->m_uid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b6a:
        {
        RevengeView* v = (RevengeView*)data;
        // ORIG 0x8477dab：SetRewardType(d+10) → switch(d+10){1:Open, 2/0:Close} →
        // 0x1b6a：revenge 包为 makePacketHeader(0x1b6a,..) 手拼（Monitor/DNFPacketTranslater.cpp:3343），无类建模。
        // SetCloseTime(d+14) → printf("[Packet_Request_Revenge] ...")。
        GlobalData::s_villageMonsterMgr->SetRewardType(
            (ENUM_VILLAGE_ATTACKED_REWARD)v->m_reward);
        switch (v->m_reward)
        {
        case 1:
            GlobalData::s_revengeDungeonMgr->OpenRevengeDungeon();
            break;
        case 2:
        case 0:
            GlobalData::s_revengeDungeonMgr->CloseRevengeDungeon();
            break;
        }
        GlobalData::s_revengeDungeonMgr->SetCloseTime(v->m_closeTime);
        printf("[Packet_Request_Revenge] isRevengeDungeonOpen : %d, "
               "DungeonClose_Remain_Time : %u\n",
               v->m_reward, v->m_closeTime);
    }
        break;

        case 0x1004:
        {
        char* d = data;
        guard.operator*()->operator<<(389);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1005:
        {
        char* d = data;
        guard.operator*()->operator<<(390);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x100a:
        {
        // 0x100a：Monitor 域 makePacketHeader(0x100a,0x1e1) 手拼包，无类建模，布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(392);
        guard.operator*()->operator<<((int)((UIntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1006:
        {
        char* d = data;
        guard.operator*()->operator<<(391);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6e5:
        {
        char* d = data;
        guard.operator*()->operator<<(398);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6b2:
        {
        char* d = data;
        guard.operator*()->operator<<(408);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 23);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x6ea:
        {
        char* d = data;
        guard.operator*()->operator<<(409);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 10);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x100f:
        {
        char* d = data;
        guard.operator*()->operator<<(416);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb60:
        {
        char* d = data;
        guard.operator*()->operator<<(418);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x70a:
        {
        char* d = data;
        guard.operator*()->operator<<(426);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x712:
        {
        char* d = data;
        guard.operator*()->operator<<(427);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x70b:
        {
        char* d = data;
        guard.operator*()->operator<<(428);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x70c:
        {
        char* d = data;
        guard.operator*()->operator<<(429);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x70d:
        {
        char* d = data;
        guard.operator*()->operator<<(430);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x70e:
        {
        char* d = data;
        guard.operator*()->operator<<(431);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x70f:
        {
        char* d = data;
        guard.operator*()->operator<<(432);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x713:
        {
        char* d = data;
        guard.operator*()->operator<<(434);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x2328:
        {
        char* d = data;
        guard.operator*()->operator<<(435);
        guard.operator*()->operator<<((int)(((GuildBoardReplyView*)d)->m_f));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x2332:
        {
        // 0x2332：权威头 Packet_Guild_Reply_Guild_Board_Delete 的 m_charNo 在 +0x10，+0xf 无字段；ORIG movl 整字读，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(436);
        guard.operator*()->operator<<((int)((UIntView0f*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1039:
        {
        char* d = data;
        guard.operator*()->operator<<(402);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x103a:
        {
        char* d = data;
        guard.operator*()->operator<<(404);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x9d3:
        {
        char* d = data;
        guard.operator*()->operator<<(437);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1773:
        {
        char* d = data;
        guard.operator*()->operator<<(441);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1774:
        {
        char* d = data;
        guard.operator*()->operator<<(442);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1776:
        {
        char* d = data;
        guard.operator*()->operator<<(443);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1777:
        {
        char* d = data;
        guard.operator*()->operator<<(444);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1778:
        {
        char* d = data;
        guard.operator*()->operator<<(445);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1775:
        {
        char* d = data;
        guard.operator*()->operator<<(446);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x177a:
        {
        char* d = data;
        guard.operator*()->operator<<(447);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x177e:
        {
        char* d = data;
        guard.operator*()->operator<<(448);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xb61:
        {
        char* d = data;
        guard.operator*()->operator<<(461);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x17c0:
        {
        char* d = data;
        guard.operator*()->operator<<(463);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x17c2:
        {
        char* d = data;
        guard.operator*()->operator<<(465);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b66:
        {
        char* d = data;
        guard.operator*()->operator<<(202);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, 351);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b5b:
        {
        char* d = data;
        guard.operator*()->operator<<(470);
        guard.operator*()->operator<<((int)(((Packet_Response_PvP_Buddy_Conn_List*)d)->m_uid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b5d:
        {
        char* d = data;
        guard.operator*()->operator<<(471);
        guard.operator*()->operator<<((int)(((Packet_Notice_Request_Add_PvP_Buddy*)d)->m_uid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b5f:
        {
        char* d = data;
        guard.operator*()->operator<<(472);
        guard.operator*()->operator<<((int)(((Packet_Notice_Add_PvP_Buddy_Result*)d)->m_uid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b61:
        {
        char* d = data;
        guard.operator*()->operator<<(473);
        guard.operator*()->operator<<((int)(((Packet_Response_Remove_PvP_Buddy*)d)->m_uid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b62:
        {
        char* d = data;
        guard.operator*()->operator<<(474);
        guard.operator*()->operator<<((int)(((Packet_Notice_PvP_Buddy_In_Out*)d)->m_uid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b63:
        {
        char* d = data;
        guard.operator*()->operator<<(475);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(...,2,guard)
    }
        break;

        case 0x1b64:
        {
        char* d = data;
        guard.operator*()->operator<<(476);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(...,2,guard)
    }
        break;

        case 0x1b65:
        {
        char* d = data;
        guard.operator*()->operator<<(477);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);   // ORIG put(...,2,guard)
    }
        break;

        case 0x27f9:
        {
        char* d = data;
        // ORIG je 84797de：make_internal_stream_jpn(guard, 0x2a, -1) + put_binary(dyn)。
        ARAD::DISPATCHER::make_internal_stream_jpn(
            guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2a, -1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27fa:
        {
        char* d = data;
        // ORIG je 847985a：make_internal_stream_jpn(guard, 0x2b, -1) + put_binary(dyn)。
        ARAD::DISPATCHER::make_internal_stream_jpn(
            guard, (ARAD::ENUM_INTERNALPACKET_JPN)0x2b, -1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1bbd:
        {
        char* d = data;
        guard.operator*()->operator<<(532);
        guard.operator*()->operator<<(((IntView0f*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1bc1:
        {
        char* d = data;
        guard.operator*()->operator<<(533);
        guard.operator*()->operator<<(((IntView12*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1f40:
        {
        char* d = data;
        guard.operator*()->operator<<(503);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1f42:
        {
        char* d = data;
        guard.operator*()->operator<<(504);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b6c:
        {
        char* d = data;
        guard.operator*()->operator<<(521);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1b6d:
        {
        char* d = data;
        guard.operator*()->operator<<(522);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1f45:
        {
        char* d = data;
        guard.operator*()->operator<<(523);
        guard.operator*()->operator<<((int)(((Packet_Find_Charac_Name_UseUID*)d)->m_dbid));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1f4a:
        {
        char* d = data;
        guard.operator*()->operator<<(528);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1f4b:
        {
        char* d = data;
        guard.operator*()->operator<<(529);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1f4c:
        {
        char* d = data;
        guard.operator*()->operator<<(530);
        guard.operator*()->operator<<((int)(((TimeSyncLoginView*)d)->m_idByChannel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1bc3:
        {
        char* d = data;
        guard.operator*()->operator<<(539);
        guard.operator*()->operator<<(((IntView12*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1bc7:
        {
        char* d = data;
        guard.operator*()->operator<<(541);
        guard.operator*()->operator<<(((IntView0e*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x1bc8:
        {
        char* d = data;
        guard.operator*()->operator<<(542);
        guard.operator*()->operator<<(((IntView0e*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfe6:
        {
        // 0xfe6：重建源码无该类（4070），布局未建模，保留裸读。
        char* d = data;
        guard.operator*()->operator<<(694);
        if (((IntView0e*)d)->m_v != 0)
            guard.operator*()->operator<<((int)((UIntView0e*)d)->m_v);
        else
            guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0xfe7:
    {
        // ORIG 0x847a140：GlobalData::s_hades_proxy->ConnTcpHadesSvr() 后直返。
        GlobalData::s_hades_proxy->ConnTcpHadesSvr();
    }
        break;

        case 0xb64:
        {
        char* d = data;
        guard.operator*()->operator<<(603);
        guard.operator*()->operator<<((int)(((PunishCancelView*)d)->m_idByChannel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x2711:
        {
        char* d = data;
        guard.operator*()->operator<<(608);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, 270);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x2717:
        {
        char* d = data;
        // ORIG 0x847a272：CSwitchLog::SetLogWriteFlag(*(int*)(d+10)) 后直返
        // （ORIG mov 0xa(%eax),%eax 整字读取，非单字节）。
        CSwitchLog::SetLogWriteFlag(((IntView0a*)d)->m_v);
    }
        break;

        case 0xb65:
        {
        char* d = data;
        guard.operator*()->operator<<(628);
        guard.operator*()->operator<<((int)(((SecuConnectWebView*)d)->m_idByChannel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        // ORIG 0847a316 put 后无 jmp：直落 0x2712 的 cMyTrace 块（fall through 照抄，P171）
        case 0x2712:
    {
        // ORIG je 847a31b：cMyTrace(0xdbf,5) "TOD: bug random select" 后直返。
        cMyTrace(__PRETTY_FUNCTION__, 0xdbf, 5)("TOD: bug random select");
    }
        break;

        case 0x2713:
    {
        // ORIG je 847a35c：cMyTrace(0xdcc,5) "TOD: bug reload" 后直返。
        cMyTrace(__PRETTY_FUNCTION__, 0xdcc, 5)("TOD: bug reload");
    }
        break;

        case 0x2718:
        {
        char* d = data;
        guard.operator*()->operator<<(677);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x2719:
        {
        char* d = data;
        guard.operator*()->operator<<(678);
        guard.operator*()->operator<<((int)(((Packet_Monitor_Other_Channel_Chat_ToUser_Hyper_Link*)d)->m_idByChannel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x271a:
        {
        char* d = data;
        guard.operator*()->operator<<(679);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x271b:
        {
        char* d = data;
        guard.operator*()->operator<<(680);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27d9:
        {
        char* d = data;
        guard.operator*()->operator<<(709);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27da:
        {
        // 0x27da：LimitNpcBuyItemInfoAll +0xa 仅有占位名 m_padA（非权威字段名），保留裸读。
        char* d = data;
        guard.operator*()->operator<<(710);
        guard.operator*()->operator<<(((IntView0a*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27db:
        {
        char* d = data;
        guard.operator*()->operator<<(711);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27dc:
        {
        char* d = data;
        guard.operator*()->operator<<(712);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27df:
        {
        char* d = data;
        guard.operator*()->operator<<(718);
        guard.operator*()->operator<<(((IntView0e*)d)->m_v);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27e8:
        {
        char* d = data;
        guard.operator*()->operator<<(763);
        guard.operator*()->operator<<((int)(((CollectRewardView*)d)->m_idByChannel));
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27e9:
        {
        char* d = data;
        guard.operator*()->operator<<(764);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27e7:
        {
        char* d = data;
        guard.operator*()->operator<<(762);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27f2:
        {
        char* d = data;
        guard.operator*()->operator<<(770);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27f8:
        {
        char* d = data;
        guard.operator*()->operator<<(815);
        guard.operator*()->operator<<(-1);
        guard->put_binary(d, ((PacketHeader*)d)->packetSize);
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
    }
        break;

        case 0x27fe:
    {
        // ORIG je 847ab96：initGeolocation 失败 → logFormat(1,...,0x1143,
        // "GEO IP TABLE RELOAD FAIL")，恒返 1。
        if (!ARAD::Singleton<RestrictGeolocation>::Get()->initGeolocation())
        {
            LogManager::logFormat(1, "DispatchServerPacket.cpp", __PRETTY_FUNCTION__,
                                  0x1143, "GEO IP TABLE RELOAD FAIL");
        }
    }
        break;

        case 0x27ff:
    {
        // ORIG je 847abd8：ServiceRestrictManager::loadPolicy()，恒返 1。
        ARAD::Singleton<ServiceRestrictManager>::Get()->loadPolicy();
    }
        break;

        case 0x6a5:
    {
        // ORIG je 847abf1：空 case（无操作），返 1。
    }
        break;

        case 0x6af:
    {
        // ORIG je 847abf4：空 case（无操作），返 1。
    }
        break;

        case 0x6ae:
    {
        // ORIG je 847abf7：空 case（无操作），返 1。
    }
        break;

        case 0x6a8:

        case 0x6a9:
    {
        // ORIG 08473c03：sub $0x6a8; cmp $1; jbe → 0x6a8/0x6a9 空 case，返 1。
    }
        break;

        case 0x6ab:

        case 0x6ac:
    {
        // ORIG cmp $0x6ab jge 0847abee：0x6ab/0x6ac 空 case，返 1。
    }
        break;

    default:
    {
        // 未知 sig:ORIG LAB_0847abe7 mov $0x0,%ebx → 返回 0(调用侧
        // GlobalData.cpp `if (!dispatchPacket(data))` 据此走 fallback 处理)。
        return 0;
    }

    }
    // 已知 sig 统一走 ORIG 0847abfb（mov $1,%ebx）→ 返回 1。
    return 1;
}

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
struct GARegistResultView {           // case 0（GA/GP 共用）：+0x0e 结果码
    char m_pad0[0xe];
    int m_result;
};
struct GAAskPriceView {               // case 2：+0x12/+0x16/+0x1a
    char m_pad0[0x12];
    int m_msgSeq;
    int m_characNo;
    int m_itemIdx;
};
struct GAPriceSrcView {               // case 2 价格源：data+0xe+4k
    char m_pad0[0xe];
    int m_src[10];                    // priceCur[i]=m_src[i+4]（+0x1e+4i）
                                      // priceTop[i]=m_src[i+7]（ORIG 读 +0x2a+4i 实证）
};
struct GARegistedNumView {            // case 3：+0x1a ushort/+0x1c byte/+0x1d ushort/+0x1f nums
    char m_pad0[0x12];
    int m_msgSeq;
    int m_characNo;
    unsigned short m_count;
    unsigned char m_flagA;
    unsigned short m_flagB;
    int m_nums[8];                    // AG 读 8 条；PG 仅前 4 条（ORIG 实测）
};
struct GARegistView {                 // case 4 / case 6：+0x1a/+0x1b 单字节
    char m_pad0[0x12];
    int m_msgSeq;
    int m_characNo;
    unsigned char m_okFlag;
    unsigned char m_errCode;
};
struct GABiddingView {                // case 5（PG 使用 m_seller/m_field48）
    char m_pad0[0x12];
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
struct GAItemListHeadView {           // case 7：+0x1a byte/+0x1b uint/+0x1f ushort 计数
    char m_pad0[0x12];
    int m_msgSeq;
    int m_characNo;
    unsigned char m_b1a;
    unsigned int m_u1b;
    unsigned short m_cnt1f;
    char m_items[1];                  // +0x21 条目区基址
};
struct GAMyRegistHeadView {           // case 8：+0x1b byte 计数（PG 拷贝源含该字节）
    char m_pad0[0x12];
    int m_msgSeq;
    int m_characNo;
    unsigned char m_b1a;
    unsigned char m_cnt1b;
    char m_items[1];                  // +0x1c 条目区基址
};
struct GAMyBiddingHeadView {          // case 9：+0x1b ushort 计数
    char m_pad0[0x12];
    int m_msgSeq;
    int m_characNo;
    unsigned char m_b1a;
    unsigned short m_cnt1b;
    char m_items[1];                  // +0x1d 条目区基址
};
struct GALogMsgView {                 // case 0xb：1 字节型 + 8B 主键（两段 4B 写）
    char m_pad0[0x12];
    int m_msgSeq;                     // +0x12
    unsigned char m_type;             // +0x16
    int m_keyLo;                      // +0x17
    int m_keyHi;                      // +0x1b
    int m_a1f;
    int m_a23;
    int m_a27;
    unsigned char m_b2b;              // +0x2b
    int m_i2c;
    unsigned short m_w30;             // +0x30
    int m_i32;                        // +0x32
    unsigned char m_b36;              // +0x36（仅 AG 路径写）
};
struct GAReadyFlagView {              // case 0xd：+0x12 单字节就绪标志
    char m_pad0[0x12];
    unsigned char m_ready;
};
struct GABuyOneView {                 // case 0xe（AG 单件购买回执）
    char m_pad0[0x12];
    int m_s12;
    int m_s16;
    int m_s1a;
    unsigned char m_b1e;
    unsigned char m_b1f;
    int m_s20;
    int m_s24;
};
struct GAVipView {                    // case 0xe（PG OWNER_IS_VIP）
    char m_pad0[0x12];
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
struct GAPassThruView {               // case 0x2718..0x271b 转发
    char m_pad0[0x2];
    unsigned short m_totalLen;        // +0x2（包总长，put_binary 长度）
    char m_pad4[0x6];                 // +0x4..+0x9（ORIG 读点 0xa/0xf 实证）
    int m_s0a;                        // +0xa
    char m_padE[0x1];                 // +0xe
    int m_s0f;                        // +0xf
};
#pragma pack(pop)

// ---------------------------------------------------------------------------
// dispatchAuctionPacket —— 拍卖服务器（GA）回包分发，ORIG T 0x847ac3c
// （报告 docs/class_func_reports/CDispatchServerPacket/dispatchAuctionPacket.md）。
// category==0x13 → 委派 dispatchPointPacket；否则按 packetID 组 SIG 流投
// DISPATCH_Q（QUEUE_IDX 1）。恒返回 true（epilogue mov $0x1,%eax 实测；
// 声明 static bool 由 ORIG __PRETTY_FUNCTION__ 串实证）。
// 形态要点（ORIG 反汇编）：每次 `<<` 前重调 operator*（deEv），每次
// GetInBuffer/put_binary/Get 前重调 operator->（ptEv）；resize 双检为
// `need > size`（setg）/`need <= size`（setle）两独立读取；计数/长度实参
// 每写点重读（P48）；case3 八条明细为逐条非循环写；case0 条件逐臂重读包体。
// ---------------------------------------------------------------------------
// §6.2 巨件核销 [b28d-solo-CAuction]：verdict DIFF（N82 全帧槽号错位，不可复现）。
// 三件套：①gap ORIG 1338 / OUR 1349（−11）；②P167 调用点差集 259/259 [0缺0多]
// （含 case2 价格双循环、case3 8 条逐写、case7/8/9 resize 双检 setg/setle、
// case0xe 单件购买、default logFormat 全配对）；③cmp4 按 call 锚点分段 14 段差，
// 全为 P246 8 字节块拷贝拆散/N86b imul 合并/伪差，语义逐块一致。
// 本波真 bug：case 2 priceTop 源偏移 —— ORIG 0847afe9 `mov 0xa(%eax,%edx,4)`
// （edx=i+8 ⇒ v+0x2a+4i）而源码 m_src[i+8]（视图基址 +0xe）实读 +0x2e+4i，多 4 字节
// ⇒ 改 m_src[i+7]（GAPriceSrcView 扩 m_src[10] 容纳 ORIG 越界读，P171）。
// 残差：N82 槽号 + P246（case5/case0xb/case0xe 块拷贝 dest 缓存 %ecx 不可复现）
// + N86b（imul 合并 ×3）+ __PRETTY_FUNCTION__ 伪差。
bool CDispatchServerPacket::dispatchAuctionPacket(char* data)
{
    unsigned int packetId = (int)((nsl::PACKET_HEADER*)data)->getPacketID();
    int category = (int)((nsl::PACKET_HEADER*)data)->getCategory();
    if (category == 0x13)
    {
        dispatchPointPacket(data);
        return true;
    }

    switch (packetId)
    {
    case 0:  // 拍卖服务注册回执（包+0xe 结果码）
    {
        GARegistResultView* v = (GARegistResultView*)data;
        if (v->m_result == 0)
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x128f), true);
            (*guard)->operator<<(0xf9);
            (*guard)->operator<<(-1);
            SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
            sig->m_result = 0;
            sig->m_flag = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
        else if (v->m_result == 2)
        {
            Timer_CheckAuctionReady::RegistNextTimer();
        }
        else if (v->m_result == 1)
        {
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x12a3, "AUCTION REGIST SERVER ID DUPLICATE");
        }
        else
        {
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x12a7, "AUCTION REGIST UNHANDLED ERROR");
        }
        break;
    }
    case 1:  // 服务关闭通知
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x12b2), true);
        (*guard)->operator<<(0xf9);
        (*guard)->operator<<(-1);
        SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
            guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
        sig->m_result = 0;
        sig->m_flag = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 2:  // SIG_AUCTION_RESULT_ASK_PRICE（AG：两组价格来自包体）
    {
        GAPriceSrcView* v = (GAPriceSrcView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x12cd), true);
        (*guard)->operator<<(0xf0);
        (*guard)->operator<<(v->m_src[1]);
        SIG_AUCTION_RESULT_ASK_PRICE* sig =
            guard->GetInBuffer<SIG_AUCTION_RESULT_ASK_PRICE>();
        sig->m_result = 0;
        sig->m_characNo = v->m_src[2];
        sig->m_itemIdx = v->m_src[3];
        for (int i = 0; i <= 2; ++i)
        {
            sig->m_priceCur[i] = v->m_src[i + 4];
            sig->m_priceTop[i] = v->m_src[i + 7];
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 3:  // SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM（AG：含 8×int 明细）
    {
        GARegistedNumView* v = (GARegistedNumView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x12f0), true);
        (*guard)->operator<<(0xf1);
        (*guard)->operator<<(v->m_msgSeq);
        SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM* sig =
            guard->GetInBuffer<SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM>();
        sig->m_characNo = v->m_characNo;
        sig->m_count = v->m_count;
        sig->m_flagA = v->m_flagA;
        sig->m_flagB = v->m_flagB;
        sig->m_itemNums[0] = v->m_nums[0];
        sig->m_itemNums[1] = v->m_nums[1];
        sig->m_itemNums[2] = v->m_nums[2];
        sig->m_itemNums[3] = v->m_nums[3];
        sig->m_itemNums[4] = v->m_nums[4];
        sig->m_itemNums[5] = v->m_nums[5];
        sig->m_itemNums[6] = v->m_nums[6];
        sig->m_itemNums[7] = v->m_nums[7];
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 4:  // SIG_AUCTION_RESULT_REGIST
    {
        GARegistView* v = (GARegistView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1319), true);
        (*guard)->operator<<(0xf2);
        (*guard)->operator<<(v->m_msgSeq);
        SIG_AUCTION_RESULT_REGIST* sig =
            guard->GetInBuffer<SIG_AUCTION_RESULT_REGIST>();
        sig->m_characNo = v->m_characNo;
        sig->m_okFlag = v->m_okFlag;
        sig->m_errCode = v->m_errCode;
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 5:  // SIG_AUCTION_RESULT_BIDDING（AG：无卖家名段）
    {
        GABiddingView* v = (GABiddingView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1336), true);
        (*guard)->operator<<(0xf3);
        (*guard)->operator<<(v->m_msgSeq);
        SIG_AUCTION_RESULT_BIDDING* sig =
            guard->GetInBuffer<SIG_AUCTION_RESULT_BIDDING>();
        sig->m_characNo = v->m_characNo;
        sig->m_money = v->m_money;
        sig->m_flagA = v->m_flagA;
        sig->m_flagB = v->m_flagB;
        sig->m_field0e = v->m_field20;
        sig->m_bidPrice = v->m_bidPrice;
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 6:  // SIG_AUCTION_RESULT_REGIST_CANCEL
    {
        GARegistView* v = (GARegistView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x135a), true);
        (*guard)->operator<<(0xf4);
        (*guard)->operator<<(v->m_msgSeq);
        SIG_AUCTION_RESULT_REGIST_CANCEL* sig =
            guard->GetInBuffer<SIG_AUCTION_RESULT_REGIST_CANCEL>();
        sig->m_characNo = v->m_characNo;
        sig->m_okFlag = v->m_okFlag;
        sig->m_errCode = v->m_errCode;
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 7:  // 拍卖列表（条目 0x89 字节）
    {
        GAItemListHeadView* v = (GAItemListHeadView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1377), true);
        (*guard)->operator<<(0xf5);
        (*guard)->operator<<(v->m_msgSeq);
        (*guard)->operator<<(v->m_characNo);
        (*guard)->operator<<(v->m_b1a);
        (*guard)->operator<<(v->m_u1b);
        int listBytes = (int)(v->m_cnt1f * 0x89 + 2);
        if (guard->Get()->in_ptr() + listBytes > guard->Get()->size())
        {
            guard->Get()->resize(guard->Get()->in_ptr() + listBytes);
        }
        if (guard->Get()->in_ptr() + listBytes <= guard->Get()->size())
        {
            (*guard)->operator<<(v->m_cnt1f);
            guard->put_binary(v->m_items, (int)(v->m_cnt1f * 0x89));
        }        else
{
            (*guard)->operator<<((unsigned short)0);
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x1390, "onAUCTION_ITEM_LIST_AG, resize failed.");
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 8:  // 我的登记物品（条目 0x75 字节；AG：先写计数再拷贝）
    {
        GAMyRegistHeadView* v = (GAMyRegistHeadView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x13a3), true);
        (*guard)->operator<<(0xf6);
        (*guard)->operator<<(v->m_msgSeq);
        (*guard)->operator<<(v->m_characNo);
        (*guard)->operator<<(v->m_b1a);
        int listBytes = (int)(v->m_cnt1b * 0x75 + 1);
        if (guard->Get()->in_ptr() + listBytes > guard->Get()->size())
        {
            guard->Get()->resize(guard->Get()->in_ptr() + listBytes);
        }
        if (guard->Get()->in_ptr() + listBytes <= guard->Get()->size())
        {
            (*guard)->operator<<(v->m_cnt1b);
            guard->put_binary(v->m_items, (int)(v->m_cnt1b * 0x75));
        }        else
{
            (*guard)->operator<<((unsigned short)0);
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x13bb, "onAUCTION_MY_REGISTED_ITEM_INFO_AG, resize failed.");
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 9:  // 我的竞购信息（条目 0x7d 字节）
    {
        GAMyBiddingHeadView* v = (GAMyBiddingHeadView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x13ce), true);
        (*guard)->operator<<(0xf7);
        (*guard)->operator<<(v->m_msgSeq);
        (*guard)->operator<<(v->m_characNo);
        (*guard)->operator<<(v->m_b1a);
        int listBytes = (int)(v->m_cnt1b * 0x7d + 2);
        if (guard->Get()->in_ptr() + listBytes > guard->Get()->size())
        {
            guard->Get()->resize(guard->Get()->in_ptr() + listBytes);
        }
        if (guard->Get()->in_ptr() + listBytes <= guard->Get()->size())
        {
            (*guard)->operator<<(v->m_cnt1b);
            guard->put_binary(v->m_items, (int)(v->m_cnt1b * 0x7d));
        }        else
{
            (*guard)->operator<<((unsigned short)0);
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x13e6, "onAUCTION_MY_BIDDING_INFO_AG, resize failed.");
        }
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 0xb:  // SIG_AUCTION_LOG_MESSAGE_AG（消费方 cUserHistoryLog::Auction）
    {
        GALogMsgView* v = (GALogMsgView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x142a), true);
        (*guard)->operator<<(0xf8);
        (*guard)->operator<<(v->m_msgSeq);
        SIG_AUCTION_LOG_MESSAGE_AG* sig =
            guard->GetInBuffer<SIG_AUCTION_LOG_MESSAGE_AG>();
        sig->m_type = v->m_type;
        sig->m_keyWords[0] = (unsigned int)v->m_keyLo;
        sig->m_keyWords[1] = (unsigned int)v->m_keyHi;
        sig->m_arg09 = (unsigned int)v->m_a1f;
        sig->m_arg0d = v->m_a23;
        sig->m_arg11 = (unsigned int)v->m_a27;
        sig->m_arg15 = v->m_b2b;
        sig->m_arg16 = v->m_i2c;
        sig->m_arg1a = v->m_w30;
        sig->m_arg1c = v->m_i32;
        sig->m_arg20 = v->m_b36;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 0xc:  // 服务就绪通知（flag=1）
    {
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1447), true);
        (*guard)->operator<<(0xf9);
        (*guard)->operator<<(-1);
        SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
            guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
        sig->m_flag = 1;
        sig->m_result = 0;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    case 0xd:  // 就绪探测回执：成功→通知；否则续约定时器
    {
        GAReadyFlagView* v = (GAReadyFlagView*)data;
        if (v->m_ready != 0)
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1464), true);
            (*guard)->operator<<(0xf9);
            (*guard)->operator<<(-1);
            SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
            sig->m_flag = 1;
            sig->m_result = 0;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        }
        else
        {
            Timer_CheckAuctionReady::RegistNextTimer();
        }
        break;
    }
    case 0xe:  // SIG_AUCTION_RESULT_BUY_ITEM_APIECE（单件购买回执，sig 0x20d）
    {
        GABuyOneView* v = (GABuyOneView*)data;
        CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
            "DispatchServerPacket.cpp", 0x1485), true);
        (*guard)->operator<<(0x20d);
        (*guard)->operator<<(v->m_s12);
        SIG_AUCTION_RESULT_BUY_ITEM_APIECE* sig =
            guard->GetInBuffer<SIG_AUCTION_RESULT_BUY_ITEM_APIECE>();
        sig->m_result = v->m_s16;
        sig->m_characNo = v->m_s1a;
        sig->m_flagA = v->m_b1e;
        sig->m_flagB = v->m_b1f;
        sig->m_arg0a = v->m_s20;
        sig->m_price = v->m_s24;
        GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
        break;
    }
    default:
        LogManager::logFormat(
            1, "DispatchServerPacket.cpp",
            __PRETTY_FUNCTION__,
            0x149c, "[AUCTION PACKET ERROR] not exist packet id (%d)", packetId);
        break;
    }
    return true;
}

// ---------------------------------------------------------------------------
// dispatchPointPacket —— 点券拍卖（GP）回包分发，ORIG T 0x847c1fe
// （报告 docs/class_func_reports/CDispatchServerPacket/dispatchPointPacket.md）。
// 仅 category==0x13 时处理；按 packetID 组 SIG 流投 DISPATCH_Q，恒返回 true
// （声明 static bool 由 ORIG __PRETTY_FUNCTION__ 串实证）。
// 注：packetID 经 nsl::PACKET_HEADER::getPacketID 符号扩展（movsbl），
// 0x2718..0x271b 分支在 ORIG 中不可达，仍忠实保留。
// 形态要点同 dispatchAuctionPacket（de/pt 链、逐点重读、双检 setg/setle、
// case3 四条逐写、case0 逐臂重读）；case 书写序按 ORIG 叶块排布：
// …0xd → 0x2718..0x271b → 0xe（OWNER_IS_VIP 在转发分支之后）→ default；
// 0xa 为 ORIG 静默空 case（决策树 jb→nop 实测）。
// ---------------------------------------------------------------------------
// §6.2 巨件核销 [b28d-solo-CAuction]：verdict DIFF（N82 全帧槽号错位，不可复现）。
// 三件套：①gap ORIG 1614 / OUR 1621（−7）；②P167 调用点差集 309/309 [0缺0多]
// （含 case7/8/9 resize 双检与 put_binary、case5 strncpy、case0xe strcpy、
// 0x2718..0x271b put_binary 转发全配对）；③cmp4 按 call 锚点分段 13 段差，
// 全为 P246 块拷贝/N86b imul 合并/伪差，语义逐块一致。
// 本波真 bug：GAPassThruView 字段偏移 —— 原 m_s0a@+0x4 / m_s0f@+0x8，ORIG 实证
// 读 +0xa/+0xf（0847d781 `mov 0xa(%eax)`、0847d879 `mov 0xf(%eax)`）⇒ 补
// m_pad4[6]/m_padE[1] 校正；put_binary 首实参 data→v（ORIG -0x20(%ebp) 局部实证）。
// 残差：N82 槽号 + P246（case5/case0xb/case0xe 块拷贝）+ N86b（imul 合并 ×3）
// + __PRETTY_FUNCTION__ 伪差。
bool CDispatchServerPacket::dispatchPointPacket(char* data)
{
    unsigned int packetId = (int)((nsl::PACKET_HEADER*)data)->getPacketID();
    int category = (int)((nsl::PACKET_HEADER*)data)->getCategory();
    if (category == 0x13)
    {
        switch (packetId)
        {
        case 0:  // 点券拍卖服务注册回执
        {
            GARegistResultView* v = (GARegistResultView*)data;
            if (v->m_result == 0)
            {
                CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                    "DispatchServerPacket.cpp", 0x14b4), true);
                (*guard)->operator<<(0xf9);
                (*guard)->operator<<(-1);
                SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                    guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
                sig->m_result = 1;
                sig->m_flag = 1;
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
                break;
            }
            if (v->m_result == 2)
            {
                Timer_CheckAuctionReady::RegistNextTimer();
                break;
            }
            if (v->m_result == 1)
            {
                LogManager::logFormat(
                    1, "DispatchServerPacket.cpp",
                    __PRETTY_FUNCTION__,
                    0x14c8, "AUCTION REGIST SERVER ID DUPLICATE");
                break;
            }
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x14cc, "AUCTION REGIST UNHANDLED ERROR");
            break;
        }
        case 1:  // 服务关闭通知
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x14d5), true);
            (*guard)->operator<<(0xf9);
            (*guard)->operator<<(-1);
            SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
            sig->m_flag = 0;
            sig->m_result = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 2:  // SIG_AUCTION_RESULT_ASK_PRICE（PG：价格组清零）
        {
            GAPriceSrcView* v = (GAPriceSrcView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x14e9), true);
            (*guard)->operator<<(0xf0);
            (*guard)->operator<<(v->m_src[1]);
            SIG_AUCTION_RESULT_ASK_PRICE* sig =
                guard->GetInBuffer<SIG_AUCTION_RESULT_ASK_PRICE>();
            sig->m_result = 1;
            sig->m_characNo = v->m_src[2];
            sig->m_itemIdx = v->m_src[3];
            for (int i = 0; i <= 2; ++i)
            {
                sig->m_priceCur[i] = 0;
                sig->m_priceTop[i] = 0;
            }
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 3:  // SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM（PG：明细仅前 4 条）
        {
            GARegistedNumView* v = (GARegistedNumView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1502), true);
            (*guard)->operator<<(0xf1);
            (*guard)->operator<<(v->m_msgSeq);
            SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM* sig =
                guard->GetInBuffer<SIG_AUCTION_RESULT_ASK_REGISTED_ITEM_NUM>();
            sig->m_characNo = v->m_characNo;
            sig->m_count = v->m_count;
            sig->m_flagA = v->m_flagA;
            sig->m_flagB = v->m_flagB;
            sig->m_itemNums[0] = v->m_nums[0];
            sig->m_itemNums[1] = v->m_nums[1];
            sig->m_itemNums[2] = v->m_nums[2];
            sig->m_itemNums[3] = v->m_nums[3];
            sig->m_result = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 4:  // SIG_AUCTION_RESULT_REGIST
        {
            GARegistView* v = (GARegistView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1523), true);
            (*guard)->operator<<(0xf2);
            (*guard)->operator<<(v->m_msgSeq);
            SIG_AUCTION_RESULT_REGIST* sig =
                guard->GetInBuffer<SIG_AUCTION_RESULT_REGIST>();
            sig->m_characNo = v->m_characNo;
            sig->m_okFlag = v->m_okFlag;
            sig->m_errCode = v->m_errCode;
            sig->m_result = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 5:  // SIG_AUCTION_RESULT_BIDDING（PG：附卖家名与尾字段）
        {
            GABiddingView* v = (GABiddingView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1536), true);
            (*guard)->operator<<(0xf3);
            (*guard)->operator<<(v->m_msgSeq);
            SIG_AUCTION_RESULT_BIDDING* sig =
                guard->GetInBuffer<SIG_AUCTION_RESULT_BIDDING>();
            sig->m_characNo = v->m_characNo;
            sig->m_money = v->m_money;
            sig->m_flagA = v->m_flagA;
            sig->m_flagB = v->m_flagB;
            sig->m_field0e = v->m_field20;
            sig->m_bidPrice = v->m_bidPrice;
            sig->m_result = 1;
            strncpy(sig->m_sellerName, v->m_seller, strlen(v->m_seller));
            sig->m_field36 = v->m_field48;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 6:  // SIG_AUCTION_RESULT_REGIST_CANCEL
        {
            GARegistView* v = (GARegistView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x154e), true);
            (*guard)->operator<<(0xf4);
            (*guard)->operator<<(v->m_msgSeq);
            SIG_AUCTION_RESULT_REGIST_CANCEL* sig =
                guard->GetInBuffer<SIG_AUCTION_RESULT_REGIST_CANCEL>();
            sig->m_characNo = v->m_characNo;
            sig->m_okFlag = v->m_okFlag;
            sig->m_errCode = v->m_errCode;
            sig->m_result = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 7:  // 拍卖列表 PG（条目 0x89 字节）
        {
            GAItemListHeadView* v = (GAItemListHeadView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1561), true);
            (*guard)->operator<<(0xf5);
            (*guard)->operator<<(v->m_msgSeq);
            (*guard)->operator<<(v->m_characNo);
            (*guard)->operator<<(v->m_b1a);
            (*guard)->operator<<(v->m_u1b);
            int listBytes = (int)(v->m_cnt1f * 0x89 + 2);
            if (guard->Get()->in_ptr() + listBytes > guard->Get()->size())
            {
                guard->Get()->resize(guard->Get()->in_ptr() + listBytes);
            }
            if (guard->Get()->in_ptr() + listBytes <= guard->Get()->size())
            {
                (*guard)->operator<<(v->m_cnt1f);
                guard->put_binary(v->m_items, (int)(v->m_cnt1f * 0x89));
            }        else
{
                (*guard)->operator<<((unsigned short)0);
                LogManager::logFormat(
                    1, "DispatchServerPacket.cpp",
                    __PRETTY_FUNCTION__,
                    0x1579, "onAUCTION_ITEM_LIST_PG, resize failed.");
            }
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 8:  // 我的登记物品 PG（条目 0x75 字节；PG：双 put_binary 无计数写）
        {
            GAMyRegistHeadView* v = (GAMyRegistHeadView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1584), true);
            (*guard)->operator<<(0xf6);
            (*guard)->operator<<(v->m_msgSeq);
            (*guard)->operator<<(v->m_characNo);
            (*guard)->operator<<(v->m_b1a);
            int listBytes = (int)(v->m_cnt1b * 0x75 + 1);
            if (guard->Get()->in_ptr() + listBytes > guard->Get()->size())
            {
                guard->Get()->resize(guard->Get()->in_ptr() + listBytes);
            }
            if (guard->Get()->in_ptr() + listBytes <= guard->Get()->size())
            {
                guard->put_binary(&v->m_cnt1b, 1);
                guard->put_binary(v->m_items, (int)(v->m_cnt1b * 0x75));
            }        else
{
                (*guard)->operator<<((unsigned short)0);
                LogManager::logFormat(
                    1, "DispatchServerPacket.cpp",
                    __PRETTY_FUNCTION__,
                    0x159b, "onAUCTION_MY_REGISTED_ITEM_INFO_PG, resize failed.");
            }
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 9:  // 我的竞购信息 PG（条目 0x7d 字节）
        {
            GAMyBiddingHeadView* v = (GAMyBiddingHeadView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x15a6), true);
            (*guard)->operator<<(0xf7);
            (*guard)->operator<<(v->m_msgSeq);
            (*guard)->operator<<(v->m_characNo);
            (*guard)->operator<<(v->m_b1a);
            int listBytes = (int)(v->m_cnt1b * 0x7d + 2);
            if (guard->Get()->in_ptr() + listBytes > guard->Get()->size())
            {
                guard->Get()->resize(guard->Get()->in_ptr() + listBytes);
            }
            if (guard->Get()->in_ptr() + listBytes <= guard->Get()->size())
            {
                (*guard)->operator<<(v->m_cnt1b);
                guard->put_binary(v->m_items, (int)(v->m_cnt1b * 0x7d));
            }        else
{
                (*guard)->operator<<((unsigned short)0);
                LogManager::logFormat(
                    1, "DispatchServerPacket.cpp",
                    __PRETTY_FUNCTION__,
                    0x15bd, "onAUCTION_MY_BIDDING_INFO_PG, resize failed.");
            }
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0xa:  // ORIG 静默忽略（决策树 jb→空块实测）
        {
            break;
        }
        case 0xb:  // SIG_AUCTION_LOG_MESSAGE_PG（消费方 cUserHistoryLog::AuctionPoint）
        {
            GALogMsgView* v = (GALogMsgView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x15ce), true);
            (*guard)->operator<<(0xfa);
            (*guard)->operator<<(v->m_msgSeq);
            SIG_AUCTION_LOG_MESSAGE_PG* sig =
                guard->GetInBuffer<SIG_AUCTION_LOG_MESSAGE_PG>();
            sig->m_type = v->m_type;
            sig->m_keyWords[0] = (unsigned int)v->m_keyLo;
            sig->m_keyWords[1] = (unsigned int)v->m_keyHi;
            sig->m_arg09 = (unsigned int)v->m_a1f;
            sig->m_arg0d = v->m_a23;
            sig->m_arg11 = (unsigned int)v->m_a27;
            sig->m_arg15 = v->m_b2b;
            sig->m_arg16 = v->m_i2c;
            sig->m_arg1a = v->m_w30;
            sig->m_arg1c = v->m_i32;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0xc:  // 服务就绪通知（flag=1）
        {
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x15e5), true);
            (*guard)->operator<<(0xf9);
            (*guard)->operator<<(-1);
            SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
            sig->m_flag = 1;
            sig->m_result = 1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0xd:  // 就绪探测回执：非零→通知；零→续约定时器
        {
            GAReadyFlagView* v = (GAReadyFlagView*)data;
            if (v->m_ready != 0)
            {
                CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                    "DispatchServerPacket.cpp", 0x15fa), true);
                (*guard)->operator<<(0xf9);
                (*guard)->operator<<(-1);
                SIG_AUCTION_NOTIFY_AUCTION_SERVICE* sig =
                    guard->GetInBuffer<SIG_AUCTION_NOTIFY_AUCTION_SERVICE>();
                sig->m_result = 1;
                sig->m_flag = 1;
                GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            }
            else
            {
                Timer_CheckAuctionReady::RegistNextTimer();
            }
            break;
        }
        case 0x2718:  // 转发类分支（ORIG 不可达但保留）：sig 0x2a5
        {
            GAPassThruView* v = (GAPassThruView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1613), true);
            (*guard)->operator<<(0x2a5);
            (*guard)->operator<<(v->m_s0a);
            guard->put_binary(v, (int)v->m_totalLen);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0x2719:  // sig 0x2a6
        {
            GAPassThruView* v = (GAPassThruView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1620), true);
            (*guard)->operator<<(0x2a6);
            (*guard)->operator<<(v->m_s0f);
            guard->put_binary(v, (int)v->m_totalLen);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0x271a:  // sig 0x2a7
        {
            GAPassThruView* v = (GAPassThruView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x162c), true);
            (*guard)->operator<<(0x2a7);
            (*guard)->operator<<(v->m_s0a);
            guard->put_binary(v, (int)v->m_totalLen);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0x271b:  // sig 0x2a8
        {
            GAPassThruView* v = (GAPassThruView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1638), true);
            (*guard)->operator<<(0x2a8);
            (*guard)->operator<<(-1);
            guard->put_binary(v, (int)v->m_totalLen);
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        case 0xe:  // SIG_AUCTION_OWNER_IS_VIP（sig 0x2da；ORIG 叶块排在转发分支后）
        {
            GAVipView* v = (GAVipView*)data;
            CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
                "DispatchServerPacket.cpp", 0x1646), true);
            (*guard)->operator<<(0x2da);
            (*guard)->operator<<(v->m_s12);
            SIG_AUCTION_OWNER_IS_VIP* sig =
                guard->GetInBuffer<SIG_AUCTION_OWNER_IS_VIP>();
            sig->m_arg04 = v->m_s16;
            sig->m_arg08 = v->m_s1a;
            sig->m_arg0d = v->m_s1f;
            sig->m_arg11 = v->m_s23;
            strcpy(sig->m_ownerName, v->m_owner);
            sig->m_characNo = v->m_s12;
            sig->m_flagC = v->m_b1e;
            sig->m_arg26 = v->m_s38;
            sig->m_arg22 = v->m_s34;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            break;
        }
        default:
            LogManager::logFormat(
                1, "DispatchServerPacket.cpp",
                __PRETTY_FUNCTION__,
                0x166f, "[AUCTION PACKET ERROR] not exist packet id (%d)", packetId);
            break;
        }
    }
    return true;
}
