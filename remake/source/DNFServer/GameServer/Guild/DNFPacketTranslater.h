#ifndef DNFPACKETTRANSLATER_H_
#define DNFPACKETTRANSLATER_H_

#include <queue>
#include <string>
#include "PacketHeader.h"
#include "Thread.h"

#include "PacketHeader.h"
#include "DNFGuild.h"
#include "GuildCargo.h"

class CApplication;
class CGuild;
class CPacketTranslater;
class CUser;
struct Packet_Add_Guild_Fund_Result;
struct Packet_Answer_Guild_Member_Connection_From_Web;
struct Packet_Answer_Join_Power;
struct Packet_Answer_Secede_Power;
struct Packet_Approve_Join_Guild_Result;
struct Packet_Channel_Check_Guild_Cargo_Push_Item;
struct Packet_Channel_Delete_Guild_Agit;
struct Packet_Channel_Guild_Cargo_Move_Item;
struct Packet_Channel_Guild_Cargo_Pop_Item;
struct Packet_Channel_Guild_Cargo_Push_Item;
struct Packet_Channel_Guild_Cargo_Upgrade;
struct Packet_DBMW_Query_Msg;
struct Packet_DBMW_Register_To_BlackList;
struct Packet_DBMW_Request_BlackList;
struct Packet_DBMW_Request_Guild_Create;
struct Packet_DBMW_Save_Guild_Join;
struct Packet_DBMW_Send_Guild_Mail;
struct Packet_DB_Request_Guild_Master_Delegate;
struct Packet_DB_Request_Guild_Secede;
struct Packet_DB_Write_Guild_Member_Memo;
struct Packet_DMBW_Delete_To_BlackList;
struct Packet_Delete_To_BlackList_Result;
struct Packet_Guild_Apply_Origial_Power_Side_Reply;
struct Packet_Guild_Call_Guild_Invite_To_Caller;
struct Packet_Guild_Call_Guild_Invite_To_Invited;
struct Packet_Guild_Cargo_History_Response;
struct Packet_Guild_Cargo_Response;
struct Packet_Guild_Exp_Book_Delete;
struct Packet_Guild_Reply_Guild_Invite_To_Caller;
struct Packet_Guild_Reply_Guild_Invite_To_Invited;
struct Packet_Guild_Reply_Guild_Master_Delegate;
struct Packet_Guild_Reply_Guild_Secede;
struct Packet_Monitor_Call_Guild_Info_Reply;
struct Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade;
struct Packet_Monitor_Notice_Guild_Create_ToUser;
struct Packet_Monitor_Notify_GuildMemberGrade;
struct Packet_Monitor_Notify_New_Mail;
struct Packet_Monitor_Reply_Guild_Mail;
struct Packet_Monitor_Set_Sub_Guild_Master_Reply;
struct Packet_Monitor_UDP_User_Getout;
struct Packet_No_Cache;
struct Packet_Notice_DB_Guild_War_End;
struct Packet_Notice_GuildName_On_Guild_Create;
struct Packet_Notice_Has_Been_Guild_Master;
struct Packet_Register_To_BlackList_RESULT;
struct Packet_Reply_Change_Guild_Name;
struct Packet_Reply_Guild_Create;
struct Packet_Request_Result_BlackList;
struct Packet_Tcp_Server_Connect;
struct Packet_UnChangable_GuildInfo_Save;
struct Packet_Web_Notify_Message_To_Guild;
struct STGuildJoinInfo;

// from GuildPacket.h
class CPacketTranslater
{
public:
    static void attach(CApplication* app);
    static void OnHeartBeat(PacketHeader* pkt);
    static void OnLogin(PacketHeader* pkt);
    static void OnLogout(PacketHeader* pkt);
    static void OnReplyUserInfo(PacketHeader* pkt);
    static void OnCharLogin(PacketHeader* pkt);
    static void OnReplyQueryGuild(PacketHeader* pkt);
    static void OnDBReplyQueryGuildMember(PacketHeader* pkt);
    static void OnNoticeGuildEnter(PacketHeader* pkt);
    static void OnNoticeGuildSecede(PacketHeader* pkt);
    static void OnNoticeGuildMarkChange(PacketHeader* pkt);
    static void OnNoticeGuildDismiss(PacketHeader* pkt);
    static void OnNoticeGuildChatMsg(PacketHeader* pkt);
    static void OnSetGuildMemberGrade(PacketHeader* pkt);
    static void OnCallGuildMembers(PacketHeader* pkt);
    static void OnCallGuildAllMembers(PacketHeader* pkt);
    static void OnDBReplyGuildAllMembers(PacketHeader* pkt);
    static void OnDBReplyUnconnGuildMember(PacketHeader* pkt);
    static void OnCallGuildLevelUp(PacketHeader* pkt);
    static void OnCallGuildInfo(PacketHeader* pkt);
    static void OnNoticeGuildCreate(PacketHeader* pkt);
    static void OnCharacterDelete(PacketHeader* pkt);
    static void OnUpdateChangableCharInfo(PacketHeader* pkt);
    static void OnEventStart(PacketHeader* pkt);
    static void OnEventEnd(PacketHeader* pkt);
    static void OnLoadGuildWarEnterableGuilds(PacketHeader* pkt);
    static void OnNoticeGuildWarEnd(PacketHeader* pkt);
    static void OnNoticeGuildMailArrive(PacketHeader* pkt);
    static void OnNoticeGuildWarStart(PacketHeader* pkt);
    static void OnNoticeGuildWarPointChange(PacketHeader* pkt);
    static void OnRequestGuildWarInfo(PacketHeader* pkt);
    static void OnRegisterToBlackList(PacketHeader* pkt);
    static void OnDeleteToBlackList(PacketHeader* pkt);
    static void OnRequestBlackList(PacketHeader* pkt);
    static void OnDBMWResisterToBlackList(PacketHeader* pkt);
    static void OnDBMWDeleteToBlackList(PacketHeader* pkt);
    static void OnDBMWResponseBlackListOnLogin(PacketHeader* pkt);
    static void OnIncreaseGuildExp(PacketHeader* pkt);
    static void OnBuyGuildSkill(PacketHeader* pkt);
    static void OnDBMWChangeUnconnectedGuildMemberGrade(PacketHeader* pkt);
    static void OnNotifyMessageToGuild(PacketHeader* pkt);
    static void OnMonitorManagerConnectOK(PacketHeader* pkt);
    static void OnMonitorSendGuildLetter(PacketHeader* pkt);
    static void OnDBMWReplySendGuildLetter(PacketHeader* pkt);
    static void OnCallGuildInvite(PacketHeader* pkt);
    static void OnReplyGuildInvite(PacketHeader* pkt);
    static void OnDBMWGuildJoin(PacketHeader* pkt);
    static void OnRequestGuildSecede(PacketHeader* pkt);
    static void OnDBReplyGuildSecede(PacketHeader* pkt);
    static void OnRequestGuildMasterDelegate(PacketHeader* pkt);
    static void OnDBReplyGuildMasterDelegate(PacketHeader* pkt);
    static void OnRequestGuildCreate(PacketHeader* pkt);
    static void OnDBReplyGuildCreate(PacketHeader* pkt);
    static void OnSetGuildMemberGradeFromWeb(PacketHeader* pkt);
    static void OnGuildMasterDelegateFromWeb(PacketHeader* pkt);
    static void OnCheckGuildMemberConnectionFromWeb(PacketHeader* pkt);
    static void OnInnerPacketLogin(PacketHeader* pkt);
    static void OnInnerPacketLogout(PacketHeader* pkt);
    static void OnChangeGuildName(PacketHeader* pkt);
    static void OnPowerWarStartInfo(PacketHeader* pkt);
    static void OnChangePowerWarPoint(PacketHeader* pkt);
    static void OnPacketJoinPower(PacketHeader* pkt);
    static void OnPacketSecedePower(PacketHeader* pkt);
    static void OnSetPowerWarCfg(PacketHeader* pkt);
    static void OnLoadFromDBOnGuildBooting(PacketHeader* pkt);
    static void OnGMPowerWarStart(PacketHeader* pkt);
    static void OnGMPowerWarEnd(PacketHeader* pkt);
    static void OnChangeCharName(PacketHeader* pkt);
    static void OnChangeGuildNameFromWeb(PacketHeader* pkt);
    static void OnNotifyNewGroupMail(PacketHeader* pkt);
    static void OnInCreaseDecreasePowerWarPoint(PacketHeader* pkt);
    static void OnCreateGuildAgit(PacketHeader* pkt);
    static void OnDeleteGuildAgit(PacketHeader* pkt);
    static void OnDBCreateGuildAgit(PacketHeader* pkt);
    static void OnDBDeleteGuildAgit(PacketHeader* pkt);
    static void OnPowerWarProcessInfo(PacketHeader* pkt);
    static void OnDBLoadGuildAgit(PacketHeader* pkt);
    static void OnUpgradeGuildAgit(PacketHeader* pkt);
    static void OnDBUpgradeGuildAgit(PacketHeader* pkt);
    static void OnGuildExpLimit(PacketHeader* pkt);
    static void OnWriteGuildMemberMemo(PacketHeader* pkt);
    static void OnLoadGuildCargo(PacketHeader* pkt);
    static void OnLoadGuildCargoHistory(PacketHeader* pkt);
    static void OnGuildCargo(PacketHeader* pkt);
    static void OnGuildCargoHistory(PacketHeader* pkt);
    static void OnGuildCargoCheckPushItem(PacketHeader* pkt);
    static void OnGuildCargoPushItem(PacketHeader* pkt);
    static void OnGuildCargoPopItem(PacketHeader* pkt);
    static void OnGuildCargoMoveItem(PacketHeader* pkt);
    static void OnGuildCargoUpgrade(PacketHeader* pkt);
    static void OnGuildRequestGuildBoardOpen(PacketHeader* pkt);
    static void OnDBLoadReplyGuildBoardOpen(PacketHeader* pkt);
    static void OnGuildRequestGuildBoardWrite(PacketHeader* pkt);
    static void OnDBLoadReplyGuildBoardWrite(PacketHeader* pkt);
    static void OnGuildRequestGuildBoardDelete(PacketHeader* pkt);
    static void OnDBLoadReplyGuildBoardDelete(PacketHeader* pkt);
    static void OnWebGuildBoardWrite(PacketHeader* pkt);
    static void OnWebGuildBoardDelete(PacketHeader* pkt);
    static void OnDBLoadReplyWebGuildBoardWrite(PacketHeader* pkt);
    static void OnGuildApplyOriginalPowerSide(PacketHeader* pkt);
    static void OnGameServerRegist(PacketHeader* pkt);
    static void OnAddGuildFund(PacketHeader* pkt);
    static void OnRefreshGuildInfo(PacketHeader* pkt);
    static void OnReplyTodayGuildMember(PacketHeader* pkt);
    static void OnApproveJoinGuild(PacketHeader* pkt);
    static void OnDBResponseApproveJoinGuild(PacketHeader* pkt);
    static void OnGuildAttendanceInfo(PacketHeader* pkt);
    static void OnGuildDebug(PacketHeader* pkt);
    static void OnRenew_GM_List(PacketHeader* pkt);
    static void OnNoticeGuildChatMsgHyperLink(PacketHeader* pkt);
    static void RequestBlackListToDBMW(unsigned int charNo);
    static void SendPacketGuildMail(unsigned char group, unsigned int charNo,
                                    unsigned int guildKey, const char* title, const char* content,
                                    unsigned int param);
    static void GuildJoin(CGuild* guild, STGuildJoinInfo* info, unsigned int dbid);
    static void GuildJoin(CGuild* guild, CUser* user, unsigned int dbid);

    static CApplication* m_pclApp;
};

#pragma pack(push, 1)
// from GuildPackets.h
class Packet_Add_Guild_Fund_Result : public PacketHeader {
public:
    Packet_Add_Guild_Fund_Result();
    char m_data[0xd];
};

// from GuildPackets.h
class Packet_Answer_Join_Power : public PacketHeader {
public:
    Packet_Answer_Join_Power();
    char m_data[0xd];
};

// from GuildPackets.h
class Packet_Answer_Secede_Power : public PacketHeader {
public:
    Packet_Answer_Secede_Power();
    char m_data[0xd];
};

// from GuildPackets.h
class Packet_Approve_Join_Guild_Result : public PacketHeader {
public:
    Packet_Approve_Join_Guild_Result();
    char m_data[0x10];
};

// from GuildPackets.h
class Packet_Channel_Check_Guild_Cargo_Push_Item : public PacketHeader {
public:
    Packet_Channel_Check_Guild_Cargo_Push_Item();
    char m_data[0x1d];
};

// from GuildPackets.h
class Packet_Channel_Delete_Guild_Agit : public PacketHeader {
public:
    Packet_Channel_Delete_Guild_Agit();
    char m_data[0xd];
};

// from GuildPackets.h
class Packet_Channel_Guild_Cargo_Move_Item : public PacketHeader {
public:
    Packet_Channel_Guild_Cargo_Move_Item();
    char m_data[0x11];
};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Channel_Guild_Cargo_Pop_Item : public PacketHeader {
public:
    Packet_Channel_Guild_Cargo_Pop_Item();
    unsigned int m_a;            // +0xa
    unsigned int m_b;            // +0xe
    unsigned int m_c;            // +0x12
    DnfItemInfo m_item;          // +0x16
    unsigned char m_flag;        // +0x4b
    unsigned int m_d;            // +0x4c
};
#pragma pack(pop)

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Channel_Guild_Cargo_Push_Item : public PacketHeader {
public:
    Packet_Channel_Guild_Cargo_Push_Item();
    unsigned int m_a;            // +0xa
    unsigned int m_b;            // +0xe
    unsigned char m_c;           // +0x12
    unsigned int m_d;            // +0x13
    DnfItemInfo m_item;          // +0x17
    unsigned char m_flag;        // +0x4c
    unsigned short m_e;          // +0x4d
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_Channel_Guild_Cargo_Upgrade : public PacketHeader {
public:
    Packet_Channel_Guild_Cargo_Upgrade();
    char m_data[0xa];
};

// from GuildPackets.h
class Packet_DBMW_Query_Msg : public PacketHeader {
public:
    Packet_DBMW_Query_Msg();
    unsigned int m_fieldA;    // +0xa
    unsigned int m_fieldE;    // +0xe
    char m_data[0x1001];      // +0x12
};

// from GuildPackets.h
class Packet_DBMW_Register_To_BlackList : public PacketHeader {
public:
    Packet_DBMW_Register_To_BlackList();
    char m_data[0x2a];
};

// from GuildPackets.h
class Packet_DBMW_Request_BlackList : public PacketHeader {
public:
    Packet_DBMW_Request_BlackList();
    char m_data[0x5];
};

// from GuildPackets.h
class Packet_DBMW_Request_Guild_Create : public PacketHeader {
public:
    Packet_DBMW_Request_Guild_Create();
    char m_data[0x52];
};

// from GuildPackets.h
class Packet_DBMW_Save_Guild_Join : public PacketHeader {
public:
    Packet_DBMW_Save_Guild_Join();
    char m_data[0x36];
};

// from GuildPackets.h
class Packet_DBMW_Send_Guild_Mail : public PacketHeader {
public:
    Packet_DBMW_Send_Guild_Mail();
    char m_data[0x11f];
};

// from GuildPackets.h
class Packet_DB_Request_Guild_Master_Delegate : public PacketHeader {
public:
    Packet_DB_Request_Guild_Master_Delegate();
    char m_data[0x27];
};

// from GuildPackets.h
class Packet_DB_Request_Guild_Secede : public PacketHeader {
public:
    Packet_DB_Request_Guild_Secede();
    char m_data[0x2f];
};

// from GuildPackets.h
class Packet_DB_Write_Guild_Member_Memo : public PacketHeader {
public:
    Packet_DB_Write_Guild_Member_Memo();
    char m_data[0x1d];
};

// from GuildPackets.h
class Packet_DMBW_Delete_To_BlackList : public PacketHeader {
public:
    Packet_DMBW_Delete_To_BlackList();
    char m_data[0x26];
};

// from GuildPackets.h
class Packet_Delete_To_BlackList_Result : public PacketHeader {
public:
    Packet_Delete_To_BlackList_Result();
    char m_data[0x27];
};

// from GuildPackets.h
class Packet_Guild_Apply_Origial_Power_Side_Reply : public PacketHeader {
public:
    Packet_Guild_Apply_Origial_Power_Side_Reply();
    char m_data[0xd];
};

// from GuildPackets.h
class Packet_Guild_Call_Guild_Invite_To_Caller : public PacketHeader {
public:
    Packet_Guild_Call_Guild_Invite_To_Caller();
    char m_data[0xc];
};

// from GuildPackets.h
class Packet_Guild_Call_Guild_Invite_To_Invited : public PacketHeader {
public:
    Packet_Guild_Call_Guild_Invite_To_Invited();
    char m_data[0x3d];
};

// from GuildPackets.h
class Packet_Guild_Cargo_History_Response : public PacketHeader {
public:
    Packet_Guild_Cargo_History_Response();
    char m_data[0x968];
};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Guild_Cargo_Response : public PacketHeader {
public:
    Packet_Guild_Cargo_Response();
    unsigned int m_a;              // +0xa
    unsigned int m_b;              // +0xe
    STGuildCargoDBInfo m_cargo;    // +0x12
    unsigned char m_flag;          // +0x18ee
    char m_pad;                    // +0x18ef
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_Guild_Exp_Book_Delete : public PacketHeader {
public:
    Packet_Guild_Exp_Book_Delete();
    char m_data[0x10];
};

// from GuildPackets.h
class Packet_Guild_Reply_Guild_Invite_To_Caller : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Invite_To_Caller();
    char m_data[0x2a];
};

// from GuildPackets.h
class Packet_Guild_Reply_Guild_Invite_To_Invited : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Invite_To_Invited();
    char m_data[0xc];
};

// from GuildPackets.h
class Packet_Guild_Reply_Guild_Master_Delegate : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Master_Delegate();
    char m_data[0xc];
};

// from GuildPackets.h
class Packet_Guild_Reply_Guild_Secede : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Secede();
    char m_data[0x48];
};

// from GuildPackets.h
class Packet_Monitor_Call_Guild_Info_Reply : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_Info_Reply();
    char m_data[0xacc];
};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Monitor_Notice_Guild_Create_ToUser : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Create_ToUser();
    ST_GuildCreateFromWeb m_info;   // +0xa
    unsigned int m_member;          // +0x29
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_Monitor_Notify_GuildMemberGrade : public PacketHeader {
public:
    Packet_Monitor_Notify_GuildMemberGrade();
    char m_data[0x9];
};

// from GuildPackets.h
class Packet_Monitor_Notify_New_Mail : public PacketHeader {
public:
    Packet_Monitor_Notify_New_Mail();
    char m_data[0x8];
};

// from GuildPackets.h
class Packet_Monitor_Reply_Guild_Mail : public PacketHeader {
public:
    Packet_Monitor_Reply_Guild_Mail();
    char m_data[0x9];
};

// from GuildPackets.h
class Packet_Monitor_Set_Sub_Guild_Master_Reply : public PacketHeader {
public:
    Packet_Monitor_Set_Sub_Guild_Master_Reply();
    char m_data[0x30];
};

// from GuildPackets.h
class Packet_Monitor_UDP_User_Getout : public PacketHeader {
public:
    Packet_Monitor_UDP_User_Getout();
    char m_data[0xa];
};

// from GuildPackets.h
class Packet_No_Cache : public PacketHeader {
public:
    Packet_No_Cache();
    char m_data[0xc];
};

// from GuildPackets.h
class Packet_Notice_DB_Guild_War_End : public PacketHeader {
public:
    Packet_Notice_DB_Guild_War_End();
    char m_data[0x65];
};

// from GuildPackets.h
class Packet_Notice_GuildName_On_Guild_Create : public PacketHeader {
public:
    Packet_Notice_GuildName_On_Guild_Create();
    char m_data[0x24];
};

// from GuildPackets.h
class Packet_Notice_Has_Been_Guild_Master : public PacketHeader {
public:
    Packet_Notice_Has_Been_Guild_Master();
    char m_data[0x8];
};

// from GuildPackets.h
class Packet_Register_To_BlackList_RESULT : public PacketHeader {
public:
    Packet_Register_To_BlackList_RESULT();
    char m_data[0x27];
};

// from GuildPackets.h
class Packet_Reply_Change_Guild_Name : public PacketHeader {
public:
    Packet_Reply_Change_Guild_Name();
    char m_data[0x24];
};

// from GuildPackets.h
class Packet_Reply_Guild_Create : public PacketHeader {
public:
    Packet_Reply_Guild_Create();
    char m_data[0x23];
};

// from GuildPackets.h
class Packet_Request_Result_BlackList : public PacketHeader {
public:
    Packet_Request_Result_BlackList();
    char m_data[0x195];
};

// from GuildPackets.h
class Packet_Tcp_Server_Connect : public PacketHeader {
public:
    Packet_Tcp_Server_Connect();
    unsigned char m_field_a;   // +0xa（ORIG 直写 movb $0xcb,-0xNN(%ebp)）
};

// from GuildPackets.h
class Packet_UnChangable_GuildInfo_Save : public PacketHeader {
public:
    Packet_UnChangable_GuildInfo_Save();
    char m_data[0x26];
};

// from GuildPackets.h
class Packet_Web_Notify_Message_To_Guild : public PacketHeader {
public:
    Packet_Web_Notify_Message_To_Guild();
    char m_data[0x6a];
};

// from GuildPackets.h
class Packet_Answer_Guild_Member_Connection_From_Web : public PacketHeader {
public:
    Packet_Answer_Guild_Member_Connection_From_Web();
    char m_data[0x5e2];
};

// from GuildPackets.h
class Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade : public PacketHeader {
public:
    Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade();
    char m_data[0x29];
};

#pragma pack(pop)
#endif
