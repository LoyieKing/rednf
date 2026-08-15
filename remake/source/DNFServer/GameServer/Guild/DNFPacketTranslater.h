#ifndef DNFPACKETTRANSLATER_H_
#define DNFPACKETTRANSLATER_H_

#include <queue>
#include <string>
#include "PacketHeader.h"
#include "Thread.h"

#include "PacketHeader.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFUser.h"
#include "GuildCargo.h"
#include "BlackUser.h"

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
    unsigned char ma;                 // +0xa
    unsigned int mb;                 // +0xb
    unsigned int mf;                 // +0xf
    unsigned int m13;                 // +0x13

};

// from GuildPackets.h
class Packet_Answer_Join_Power : public PacketHeader {
public:
    Packet_Answer_Join_Power();
    unsigned int m_a;                 // +0xa
    unsigned int m_e;                 // +0xe
    unsigned int m_12;                // +0x12
    unsigned char m_16;               // +0x16
};

// from GuildPackets.h
class Packet_Answer_Secede_Power : public PacketHeader {
public:
    Packet_Answer_Secede_Power();
    unsigned int m_a;                 // +0xa
    unsigned int m_e;                 // +0xe
    unsigned int m_12;                // +0x12
    unsigned char m_16;               // +0x16
};

// from GuildPackets.h
class Packet_Approve_Join_Guild_Result : public PacketHeader {
public:
    Packet_Approve_Join_Guild_Result();
    unsigned int ma;                 // +0xa
    unsigned int me;                 // +0xe
    unsigned int m12;                 // +0x12
    unsigned int m16;                 // +0x16

};

// from GuildPackets.h
class Packet_Channel_Check_Guild_Cargo_Push_Item : public PacketHeader {
public:
    Packet_Channel_Check_Guild_Cargo_Push_Item();
    unsigned int ma;                 // +0xa
    unsigned int me;                 // +0xe
    unsigned char m12;                 // +0x12
    unsigned int m13;                 // +0x13
    unsigned int m17;                 // +0x17
    unsigned int m1b;                 // +0x1b
    unsigned int m1f;                 // +0x1f
    unsigned char m23;                 // +0x23
    unsigned char m24;                 // +0x24
    unsigned short m25;                 // +0x25

};

// from GuildPackets.h
class Packet_Channel_Delete_Guild_Agit : public PacketHeader {
public:
    Packet_Channel_Delete_Guild_Agit();
    unsigned int ma;                 // +0xa
    unsigned int me;                 // +0xe
    unsigned int m12;                 // +0x12
    unsigned char m16;                 // +0x16

};

// from GuildPackets.h
class Packet_Channel_Guild_Cargo_Move_Item : public PacketHeader {
public:
    Packet_Channel_Guild_Cargo_Move_Item();
    unsigned int ma;                 // +0xa
    unsigned int me;                 // +0xe
    unsigned char m12;                 // +0x12
    unsigned int m13;                 // +0x13
    unsigned int m17;                 // +0x17

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
    unsigned int m_channel;          // +0xa（GetIdByChannel）
    unsigned int m_charNo;           // +0xe
    unsigned char m_result;          // +0x12（成功 0xc1）

};

// from GuildPackets.h
class Packet_DBMW_Query_Msg : public PacketHeader {
public:
    Packet_DBMW_Query_Msg();
    unsigned int m_queryId;    // +0xa（0x4f00；DBMW QueryMsg 传给 set_query/exec）
    unsigned int m_handleIdx;  // +0xe（DBMW m_handles[idx]）
    char m_data[0x1001];       // +0x12
};

// from GuildPackets.h
class Packet_DBMW_Register_To_BlackList : public PacketHeader {
public:
    Packet_DBMW_Register_To_BlackList();
    unsigned int m_dbid;              // +0xa
    char m_name[0x1e];                // +0xe
    unsigned int m2c;                 // +0x2c
    char m_pad30[0x4];                // +0x30
};

// from GuildPackets.h
class Packet_DBMW_Request_BlackList : public PacketHeader {
public:
    Packet_DBMW_Request_BlackList();
    unsigned int m_charNo;      // +0xa
    unsigned char m_type;       // +0xe
};

// from GuildPackets.h
class Packet_DBMW_Request_Guild_Create : public PacketHeader {
public:
    Packet_DBMW_Request_Guild_Create();
    unsigned char m_group;            // +0xa
    unsigned int m_dbid;              // +0xb
    unsigned int m_charNo;            // +0xf
    char m_name[0x1e];                // +0x13
    unsigned char m_job;              // +0x31
    unsigned char m_growth;           // +0x32
    unsigned char m_level;            // +0x33
    unsigned char m_sex;              // +0x34
    char m_ssn[2];                    // +0x35
    char m_pad37[1];                  // +0x37
    char m_guildName[0x17];           // +0x38
    char m_extra[0xd];                // +0x4f
};

// from GuildPackets.h
class Packet_DBMW_Save_Guild_Join : public PacketHeader {
public:
    Packet_DBMW_Save_Guild_Join();
    unsigned int m_groupGuildId;      // +0xa（(group<<24)|(guildId&0xffffff)）
    unsigned char m_guildIdHi;        // +0xe（guildId>>24）
    unsigned char m_dbidLo;           // +0xf（dbid&0xff）
    unsigned short m_dbidMid;         // +0x10（(dbid>>8)&0xffff）
    unsigned char m_dbidHi;           // +0x12（dbid>>24）
    unsigned char m_callerIdLo;       // +0x13（callerId&0xff）
    unsigned short m_callerIdMid;     // +0x14（(callerId>>8)&0xffff）
    unsigned char m_callerIdHi;       // +0x16（callerId>>24）
    unsigned int m_uniqCharNo;        // +0x17
    char m_name[0x1e];                // +0x1b
    unsigned char m_job;              // +0x39
    unsigned char m_growth;           // +0x3a
    unsigned char m_level;            // +0x3b
    unsigned char m_sex;              // +0x3c
    char m_ssn[2];                    // +0x3d
    char m_pad3f[1];                  // +0x3f
};

// from GuildPackets.h
class Packet_DBMW_Send_Guild_Mail : public PacketHeader {
public:
    Packet_DBMW_Send_Guild_Mail();
    unsigned int m_charNo;    // +0xa
    unsigned char m_group;    // +0xe
    unsigned int m_guildKey;  // +0xf
    char m_content[0x100];    // +0x13
    char m_title[0x11];       // +0x113
    unsigned int m_param;     // +0x124
};

// from GuildPackets.h
class Packet_DB_Request_Guild_Master_Delegate : public PacketHeader {
public:
    Packet_DB_Request_Guild_Master_Delegate();
    unsigned int m_guildKey;          // +0xa
    unsigned int m_charNo;            // +0xe
    unsigned char m_group;            // +0x12
    char m_name[0x1e];                // +0x13
};

// from GuildPackets.h
class Packet_DB_Request_Guild_Secede : public PacketHeader {
public:
    Packet_DB_Request_Guild_Secede();
    unsigned int m_guildKey;          // +0xa
    unsigned int m_charNo;            // +0xe
    unsigned char m_flag;             // +0x12
    unsigned int m_nameLen;           // +0x13
    char m_name[0x1e];                // +0x17
    char m_pad35[0x4];                // +0x35
};

// from GuildPackets.h
class Packet_DB_Write_Guild_Member_Memo : public PacketHeader {
public:
    Packet_DB_Write_Guild_Member_Memo();
    unsigned int m_charNo;            // +0xa
    unsigned int m_guildKey;          // +0xe
    char m_memo[0x15];                // +0x12
};

// from GuildPackets.h
class Packet_DMBW_Delete_To_BlackList : public PacketHeader {
public:
    Packet_DMBW_Delete_To_BlackList();
    unsigned int m_dbid;              // +0xa
    char m_name[0x1e];                // +0xe
    unsigned int m2c;                 // +0x2c

};

// from GuildPackets.h
class Packet_Delete_To_BlackList_Result : public PacketHeader {
public:
    Packet_Delete_To_BlackList_Result();
    unsigned int m_channel;           // +0xa
    char m_name[0x1e];                // +0xe
    unsigned int m2c;                 // +0x2c
    unsigned char m_result;           // +0x30
};

// from GuildPackets.h
class Packet_Guild_Apply_Origial_Power_Side_Reply : public PacketHeader {
public:
    Packet_Guild_Apply_Origial_Power_Side_Reply();
    unsigned int ma;                 // +0xa
    unsigned int me;                 // +0xe
    unsigned int m12;                 // +0x12
    unsigned char m16;                 // +0x16

};

// from GuildPackets.h
class Packet_Guild_Call_Guild_Invite_To_Caller : public PacketHeader {
public:
    Packet_Guild_Call_Guild_Invite_To_Caller();
    unsigned int m_a;                 // +0xa
    unsigned int m_e;                 // +0xe
    unsigned int m_12;                // +0x12
};

// from GuildPackets.h
class Packet_Guild_Call_Guild_Invite_To_Invited : public PacketHeader {
public:
    Packet_Guild_Call_Guild_Invite_To_Invited();
    unsigned int m_a;                 // +0xa
    unsigned int m_e;                 // +0xe
    char m_name[0x1e];                // +0x12
    char m_guildName[0x17];           // +0x30
};

// from GuildPackets.h
class Packet_Guild_Cargo_History_Response : public PacketHeader {
public:
    Packet_Guild_Cargo_History_Response();
    unsigned int ma;                // +0xa
    unsigned int me;                // +0xe
    STGuildCargoLog m_log[0x32];    // +0x12
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
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_Guild_Exp_Book_Delete : public PacketHeader {
public:
    Packet_Guild_Exp_Book_Delete();
    unsigned int m_channel;          // +0xa（GetIdByChannel）
    unsigned int m_charNo;           // +0xe（GetUniqCharNo）
    int m_group;                     // +0x12（用法依上下文：OnNoticeGuildSecede 传 guildKey，
                                     //      另一 handler 传 Get_ServerGroup()&0xff；不要固定语义）
    unsigned int m16;                // +0x16（用法依上下文：OnNoticeGuildSecede 传
                                     //      Get_ServerGroup()&0xff，另一 handler 传 DBID）

};

// from GuildPackets.h
class Packet_Guild_Reply_Guild_Invite_To_Caller : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Invite_To_Caller();
    unsigned int m_a;   // +0xa
    unsigned int m_e;   // +0xe
    unsigned int m_12;  // +0x12
    char m_rest[0x1e];  // +0x16
} __attribute__((packed));

// from GuildPackets.h
class Packet_Guild_Reply_Guild_Invite_To_Invited : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Invite_To_Invited();
    unsigned int m_a;   // +0xa
    unsigned int m_e;   // +0xe
    unsigned int m_12;  // +0x12
} __attribute__((packed));

// from GuildPackets.h
class Packet_Guild_Reply_Guild_Master_Delegate : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Master_Delegate();
    unsigned int m_a;                 // +0xa
    unsigned int m_e;                 // +0xe
    unsigned int m_12;                // +0x12
};

// from GuildPackets.h
class Packet_Guild_Reply_Guild_Secede : public PacketHeader {
public:
    Packet_Guild_Reply_Guild_Secede();
    unsigned int m_guildKey;          // +0xa
    unsigned short m_totalCnt;        // +0xe
    unsigned int m_requester;         // +0x10
    unsigned int m_channel;           // +0x14
    unsigned int m_secedeType;        // +0x18
    unsigned char m_secedeFlag;       // +0x1c
    char m_name[0x1e];                // +0x1d
    char m_guildName[0x17];           // +0x3b
};

// from GuildPackets.h
class Packet_Monitor_Call_Guild_Info_Reply : public PacketHeader {
public:
    Packet_Monitor_Call_Guild_Info_Reply();
    unsigned int ma;                    // +0xa
    unsigned int me;                    // +0xe
    unsigned char m12;                  // +0x12
    STGuildCallInfo m_infos[0x5f];      // +0x13
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
    unsigned int m_channel;           // +0xa（GetIdByChannel）
    unsigned int m_charNo;            // +0xe（GetUniqCharNo）
    unsigned char m_grade;            // +0x12

};

// from GuildPackets.h
class Packet_Monitor_Notify_New_Mail : public PacketHeader {
public:
    Packet_Monitor_Notify_New_Mail();
    unsigned int ma;    // +0xa
    unsigned int me;    // +0xe
};

// from GuildPackets.h
class Packet_Monitor_Reply_Guild_Mail : public PacketHeader {
public:
    Packet_Monitor_Reply_Guild_Mail();
    unsigned int m_charNo;            // +0xa
    unsigned int m_channel;           // +0xe
    unsigned char m_result;           // +0x12
};

// from GuildPackets.h
class Packet_Monitor_Set_Sub_Guild_Master_Reply : public PacketHeader {
public:
    Packet_Monitor_Set_Sub_Guild_Master_Reply();
    unsigned int m_a;                 // +0xa
    unsigned int m_e;                 // +0xe
    unsigned int m_12;                // +0x12
    unsigned char m_16;               // +0x16
    char m_name[0x1d];                // +0x17
    char m_pad34[0x6];                // +0x34
};

// from GuildPackets.h
class Packet_Monitor_UDP_User_Getout : public PacketHeader {
public:
    Packet_Monitor_UDP_User_Getout();
    unsigned int ma;                 // +0xa

};

// from GuildPackets.h
class Packet_No_Cache : public PacketHeader {
public:
    Packet_No_Cache();
    unsigned int ma;                 // +0xa
    unsigned int me;                 // +0xe
    unsigned int m12;                 // +0x12

};

// from GuildPackets.h
class Packet_Notice_DB_Guild_War_End : public PacketHeader {
public:
    Packet_Notice_DB_Guild_War_End();
    unsigned char m_group;            // +0xa
    char m_fieldB[0x28];              // +0xb
    char m_field33[0x28];             // +0x33
    char m_field5b[0x14];             // +0x5b
};

// from GuildPackets.h
class Packet_Notice_GuildName_On_Guild_Create : public PacketHeader {
public:
    Packet_Notice_GuildName_On_Guild_Create();
    unsigned int m_charNo;           // +0xa（创建者 charNo）
    unsigned int m_channel;          // +0xe
    unsigned int m_guildKey;         // +0x12
    char pad0x16[0x17];                 // +0x16
    unsigned char m_group;             // +0x2d（Get_ServerGroup）

};

// from GuildPackets.h
class Packet_Notice_Has_Been_Guild_Master : public PacketHeader {
public:
    Packet_Notice_Has_Been_Guild_Master();
    unsigned int m_charNo;            // +0xa
    unsigned int m_channel;           // +0xe
};

// from GuildPackets.h
class Packet_Register_To_BlackList_RESULT : public PacketHeader {
public:
    Packet_Register_To_BlackList_RESULT();
    unsigned int m_channel;           // +0xa
    char m_name[0x1e];                // +0xe
    unsigned int m2c;                 // +0x2c
    unsigned char m_result;           // +0x30
};

// from GuildPackets.h
class Packet_Reply_Change_Guild_Name : public PacketHeader {
public:
    Packet_Reply_Change_Guild_Name();
    unsigned int m_charNo;            // +0xa
    unsigned int m_guildKey;          // +0xe（成功分支写 guildKey；失败分支残留 channel，ORIG 同）
    unsigned int m_12;                // +0x12
    unsigned char m_result;           // +0x16（错误码 100/0x56/0）
    char m_name[0x17];                // +0x17
};

// from GuildPackets.h
class Packet_Reply_Guild_Create : public PacketHeader {
public:
    Packet_Reply_Guild_Create();
    unsigned int m_charNo;            // +0xa
    unsigned int m_channel;           // +0xe（GetIdByChannel）
    unsigned int m_result;            // +0x12（错误码 1/0x20）
    char m_name[0x17];                // +0x16
};

// from GuildPackets.h
class Packet_Request_Result_BlackList : public PacketHeader {
public:
    Packet_Request_Result_BlackList();
    unsigned int m_charNo;              // +0xa（pb->m_charNo）
    unsigned char m_count;              // +0xe
    STBlackUserDBType m_items[10];      // +0xf
} __attribute__((packed));

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
    unsigned int m_guildKey;       // +0xa
    unsigned int m_charNo;         // +0xe
    char m_name[0x1d];             // +0x12
    char m_pad;                    // +0x2f
};

// from GuildPackets.h
class Packet_Web_Notify_Message_To_Guild : public PacketHeader {
public:
    Packet_Web_Notify_Message_To_Guild();
    unsigned int m_guildKey;          // +0xa
    unsigned char m_mode;             // +0xe
    char m_payload[0x65];             // +0xf
};

// from GuildPackets.h
class Packet_Answer_Guild_Member_Connection_From_Web : public PacketHeader {
public:
    Packet_Answer_Guild_Member_Connection_From_Web();
    unsigned int m_guildKey;          // +0xa
    unsigned short m_fieldE;          // +0xe
    STGuildMemberWebConnInfo m_members;  // +0x10
};

// from GuildPackets.h
class Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade : public PacketHeader {
public:
    Packet_Monitor_DB_Change_Unconnected_GuildMember_Grade();
    unsigned char ma;                 // +0xa group
    unsigned int mb;                  // +0xb guildKey
    unsigned int m_f;                 // +0xf charNo
    unsigned char m_13;               // +0x13 caller grade
    char m_name[0x1e];                // +0x14 name
    unsigned char m32;                // +0x32 new grade
};

// from GuildPackets.h
class Packet_Guild_Mark_Change_Notice : public PacketHeader {
public:
    Packet_Guild_Mark_Change_Notice() : PacketHeader(0x403, 0x12) {};
    unsigned int m_guildKey;  // +0xa
    unsigned int m_fieldE;    // +0xe
};

// inbound DB / monitor replies
struct STGuildJoinInfo
{
    unsigned char m_serverId;  // +0
    char m_pad1[3];
    int m_guildId;             // +0x4
    unsigned int m_id;         // +0x8
    unsigned int m_mid;        // +0xc
    int m_characNo;            // +0x10
    char m_characName[0x1e];   // +0x14
    unsigned char m_lev;       // +0x32
    unsigned char m_growType;  // +0x33
    unsigned char m_job;       // +0x34
    unsigned char m_sex;       // +0x35
    char m_bornYear[3];        // +0x36
    char m_pad39[3];           // +0x39
};

class Packet_DB_Reply_Query_Guild : public PacketHeader {
public:
    unsigned char m_result;          // +0xa
    unsigned int m_guildId;          // +0xb
    unsigned int m_characNo;         // +0xf
    STGuildDBInfoOnly m_guildInfo;   // +0x13
    char m_rest[0x65];               // +0xd0
};

class Packet_DB_Reply_Query_Guild_Member : public PacketHeader {
public:
    unsigned char m_flag;            // +0xa
    unsigned int m_guildId;          // +0xb
    unsigned int m_characNo;         // +0xf
    STGuildMemerDBInfo m_info;       // +0x13
};

class Packet_Monitor_Notice_Guild_Secede : public PacketHeader {
public:
    ST_Notice_Guild_Secede m_info;  // +0xa
};

class Packet_Monitor_Notice_Guild_Create : public PacketHeader {
public:
    ST_GuildCreateFromWeb m_info;  // +0xa
};

class Packet_Notice_Guild_Mail_Arrive : public PacketHeader {
public:
    unsigned char m_count;       // +0xa
    unsigned int m_guildKeys[1]; // +0xb
};

class Packet_DBMW_Guild_Join : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_caller;      // +0xe
    unsigned int m_joiner;      // +0x12
    int m_result;               // +0x16
};

class Packet_DBMW_Reply_Send_Guild_Letter : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa
    unsigned int m_guildKey;    // +0xe
    unsigned char m_result;     // +0x12
};

class Packet_DB_Reply_Guild_Secede : public PacketHeader {
public:
    unsigned int m_guildKey;         // +0xa
    unsigned int m_requesterCharNo;  // +0xe
    unsigned int m_targetCharNo;     // +0x12
    int m_secedeType;                // +0x16
    unsigned char m_secedeFlag;      // +0x1a
    unsigned int m_nameLen;          // +0x1b
    char m_name[0x1e];               // +0x1f
    unsigned int m_dbid;             // +0x3d
};

class Packet_DB_Reply_Guild_Master_Delegate : public PacketHeader {
public:
    unsigned int m_guildKey;         // +0xa
    unsigned int m_requesterCharNo;  // +0xe
    unsigned int m_delegateeCharNo;  // +0x12
    char m_name[0x1e];               // +0x16
    int m_result;                    // +0x34
};

class Packet_Check_Guild_Member_Connection_From_Web : public PacketHeader {
public:
    unsigned int m_guildKey;  // +0xa
};

class Packet_Change_Guild_Name_From_Web : public PacketHeader {
public:
    unsigned int m_charNo;      // +0xa
    unsigned int m_guildKey;    // +0xe
    char m_name[0x1e];          // +0x12
};

class Packet_DB_Reply_On_Guild_Booting : public PacketHeader {
public:
    int m_a;      // +0xa
    int m_e;      // +0xe
    char m_12;    // +0x12
};

class Packet_DB_Reply_Guild_All_Members : public PacketHeader {
public:
    unsigned int m_guildId;          // +0xa
    unsigned int m_characNo;         // +0xe
    unsigned char m_count;           // +0x12
    unsigned char m_field13;         // +0x13
    STGuildMemberProxy m_members;    // +0x14
};

class Packet_DB_Reply_Unconn_Guild_Member : public PacketHeader {
public:
    unsigned int m_guildId;          // +0xa
    unsigned int m_characNo;         // +0xe
    STGuildMemberProxy m_proxy;      // +0x12
};

class Packet_DBMW_Reply_Guild_Create : public PacketHeader {
public:
    unsigned int m_characNo; // +0xa
    unsigned int m_guildId;  // +0xe
    unsigned int m_result;   // +0x12
    char m_name[0x17];       // +0x16
};

class Packet_DB_Response_Approve_Join_Guild : public PacketHeader {
public:
    int m_result;            // +0xa
    int m_guildId;           // +0xe
    int m_id;                // +0x12
    int m_characNo;          // +0x16
    STGuildJoinInfo m_joinInfo;  // +0x1a
};

class Packet_Reply_Today_Guild_Member : public PacketHeader {
public:
    unsigned int m_guildId;         // +0xa
    STTodayGuildMember m_member;    // +0xe
};

class Packet_Notify_New_Group_Mail : public PacketHeader {
public:
    unsigned int m_count;        // +0xa
    unsigned int m_characNos[1]; // +0xe
};

class Packet_Guild_Load_Guild_Agit : public PacketHeader {
public:
    unsigned int m_guildId;      // +0xa
    STGuildAgitDBInfo m_info;    // +0xe
};

class Packet_Guild_Load_Guild_Cargo : public PacketHeader {
public:
    unsigned int m_guildId;         // +0xa
    STGuildCargoDBInfo m_cargo;     // +0xe
};

class Packet_Guild_Load_Guild_Cargo_History : public PacketHeader {
public:
    unsigned int m_guildId;  // +0xa
    unsigned int m_count;    // +0xe
    STGuildCargoLog m_logs;  // +0x12
};

class Packet_DB_Load_Reply_Guild_Board_Open : public PacketHeader {
public:
    unsigned short m_result;     // +0xa
    unsigned char m_count;       // +0xc
    unsigned int m_guildId;      // +0xd
    unsigned int m_charNo;       // +0x11
    unsigned char m_boardCount;  // +0x15
    STGuildBoardDBInfo m_boards; // +0x16
};

class Packet_DB_Load_Reply_Guild_Board_Write : public PacketHeader {
public:
    unsigned short m_result;     // +0xa
    unsigned int m_guildId;      // +0xc
    unsigned int m_charNo;       // +0x10
    STGuildBoardDBInfo m_board;  // +0x14
};

class Packet_DB_Load_Reply_Guild_Board_Delete : public PacketHeader {
public:
    unsigned short m_result;  // +0xa
    unsigned int m_guildId;   // +0xc
    unsigned int m_charNo;    // +0x10
    unsigned int m_boardNo;   // +0x14
};

class Packet_DB_Load_Reply_Web_Guild_Board_Write : public PacketHeader {
public:
    unsigned short m_result;     // +0xa
    unsigned int m_guildId;      // +0xc
    unsigned int m_charNo;       // +0x10
    STGuildBoardDBInfo m_board;  // +0x14
};

class Packet_DB_Monitor_Change_Unconnected_GuildMember_Grade : public PacketHeader {
public:
    unsigned int m_guildId;      // +0xa
    unsigned int m_characNo;     // +0xe
    char m_name[0x1e];           // +0x12
    unsigned char m_result;      // +0x30
    unsigned char m_grade;       // +0x31
    unsigned int m_newGrade;     // +0x32
};

class Packet_DBMW_Reponse_BlackList : public PacketHeader {
public:
    unsigned int m_mid;                   // +0xa
    STBlackUserDBType m_blackList[10];    // +0xe
};

class Packet_DB_Delete_Guild_Agit_Reply : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_charNo;      // +0xe
    unsigned int m_field12;     // +0x12
};

class Packet_DB_Create_Guild_Agit_Reply : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_charNo;      // +0xe
    unsigned int m_field12;     // +0x12
};

class Packet_DB_Upgrade_Guild_Agit_Reply : public PacketHeader {
public:
    unsigned int m_guildKey;    // +0xa
    unsigned int m_charNo;      // +0xe
    unsigned int m_field12;     // +0x12
};

#pragma pack(pop)
#endif
