#ifndef DNFGUILD_H_
#define DNFGUILD_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"
#include "PacketHeader.h"

#include "PacketHeader.h"
#include "DNFGuild.h"
#include "GuildBoard.h"
#include "GuildCargo.h"

class CGuild;
class CServerHandler;
class CUser;
class DnfItemInfo;
struct Packet_Achieve_Guild_Attendance;
struct Packet_Channel_Create_Guild_Agit;
struct Packet_Channel_Guild_Agit_Info;
struct Packet_DBMW_Save_Power_Secede_Time;
struct Packet_DB_Call_Guild_All_Members;
struct Packet_DB_Call_Unconn_Guild_Member;
struct Packet_DB_Create_Guild_Agit;
struct Packet_DB_Delete_Guild_Agit;
struct Packet_DB_Load_Guild_Agit;
struct Packet_DB_Upgrade_Guild_Agit;
struct Packet_Guild_Notice_Guild_Master_Delegate;
struct Packet_Guild_Notice_Guild_Name_Change;
struct Packet_Guild_Notify_Guild_Member_Memo;
struct Packet_Guild_Notify_Message_To_Guild_Mem;
struct Packet_Monitor_Guild_Chat_ToUser;
struct Packet_Monitor_Guild_Chat_ToUser_Hyper_Link;
struct Packet_Monitor_Notice_Guild_Dismiss_ToUser;
struct Packet_Monitor_Notice_Guild_Enter_ToUser;
struct Packet_Monitor_Notice_Guild_Info;
struct Packet_Monitor_Notice_Guild_Mark_Change_ToUser;
struct Packet_Monitor_Notice_Guild_Member_Login_out;
struct Packet_Monitor_Notice_Guild_Secede_ToUser;
struct Packet_Monitor_SAVE_Guild;
struct Packet_Notify_Today_Guild_Member;
struct Packet_Query_Today_Guild_Member;
class RandomOption;
class RandomOptionField;
class RandomOptionSeed;
class ReservedCapacity;
struct STGuildAgitDBInfo;
struct STGuildCallInfo;
struct STGuildDBInfo;
struct STGuildDBInfoOnly;
struct STGuildMemberChangableInfo;
struct STGuildMemberCharacData;
struct STGuildMemberProxy;
struct STGuildMemberWebConnInfo;
struct STGuildSkill;
struct STTodayGuildMember;
struct ST_GuildCreateFromWeb;
struct ST_Notice_Guild_Enter;
struct ST_Notice_Guild_Secede;
class UpgradeSeparateInfo;
class hyperlink_item_info;

// from GuildDomain.h
struct STGuildSkill
{
    STGuildSkill();
    char m_data[5];
};

// from GuildDomain.h
struct STGuildMemberProxy
{
    STGuildMemberProxy();
    char m_data[0x41];
};

// from GuildDomain.h
struct __attribute__((packed)) STGuildDBInfoOnly
{
    STGuildDBInfoOnly();
    char m_pad0[0x17];             // +0
    unsigned int m_masterId;       // +0x17
    unsigned char m_guildLevel;    // +0x1b
    unsigned char m_flag0 : 1;     // +0x1c bit0
    unsigned char m_flag1 : 1;     // +0x1c bit1
    unsigned char m_flagPad : 6;   // +0x1c bits2-7
    char m_pad1d;                  // +0x1d
    unsigned int m_field1e;        // +0x1e
    unsigned short m_totalCnt;     // +0x22
    unsigned int m_guildPoint;     // +0x24
    unsigned char m_guildRank;     // +0x28
    unsigned int m_guildExp;       // +0x29
    unsigned char m_subMasterCnt;  // +0x2d
    unsigned int m_subGuildMaster[5]; // +0x2e（5×4B，跨度 0x2e..0x42，与原 m_pad2e 同跨度）
    unsigned short m_field42;      // +0x42
    unsigned char m_field44;       // +0x44
    STGuildSkill m_skills[16];     // +0x45（自动构造）
    unsigned char m_powerSide;     // +0x95
    unsigned int m_powerSecedeTime;// +0x96
    unsigned int m_powerWarPoint;  // +0x9a
    unsigned char m_agitFlag;      // +0x9e
    unsigned char m_field9f;       // +0x9f
    unsigned int m_guildFund;      // +0xa0
    char m_pada4[0xb9 - 0xa4];     // +0xa4
    unsigned int m_fieldB9;        // +0xb9
};

// from GuildDomain.h
struct STGuildDBInfo
{
    STGuildDBInfo();
    STGuildDBInfoOnly m_info;         // +0
    STGuildMemberProxy m_members[300];  // +0xbd
};

// from GuildDomain.h
struct STGuildAgitDBInfo
{
    STGuildAgitDBInfo();
    union
    {
    char m_data[1];
    unsigned char m_agitLevel;         // +0（this+0x4d09，ORIG 仅 1 字节）
    };
};

// from GuildDomain.h
struct STGuildMemberChangableInfo
{
    char m_data[0x20];
};

// from GuildDomain.h
struct hyperlink_item_info
{
    char m_data[0x40];
};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_DB_Insert_Guild_Cargo_History : public PacketHeader {
public:
    Packet_DB_Insert_Guild_Cargo_History();
    unsigned char m_a;             // +0xa
    unsigned int m_b;              // +0xb
    unsigned int m_c;              // +0xf
    char m_pad[0x15];              // +0x13
    unsigned char m_d;             // +0x28
    unsigned int m_e;              // +0x29
    unsigned int m_f;              // +0x2d
    DnfItemInfo m_item;            // +0x31
};
#pragma pack(pop)

// from GuildDomain.h
struct STGuildMemberWebConnInfo
{
    char m_data[0x5dc];
};

// from GuildDomain.h
#pragma pack(push,1)
struct STGuildCallInfo
{
    STGuildCallInfo();
    unsigned int m_field0;      // +0
    unsigned char m_field4;     // +4
    unsigned char m_field5;     // +5
    char m_data[0x1d - 6];
};
#pragma pack(pop)

// from GuildDomain.h
struct ST_GuildCreateFromWeb
{
    ST_GuildCreateFromWeb();
    char m_data[0x1f];
};

// from GuildDomain.h
struct ST_Notice_Guild_Enter
{
    ST_Notice_Guild_Enter();
    char m_data[0x41];
};

// from GuildDomain.h
struct ST_Notice_Guild_Secede
{
    ST_Notice_Guild_Secede();
    char m_data[0x43];
};

// from GuildDomain.h
class CGuild
{
public:
    CGuild(unsigned int guildKey);
    ~CGuild();
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    unsigned int GetGuildKey();
    bool IsSubGuildMaster(unsigned int dbid);
    bool IsGuildMaster(unsigned int dbid);
    char* GetGuildName();
    unsigned short GetGuildDBFlag();
    void SetGuildDBFlag(unsigned short flag);
    bool IsSetGuildDBFlag(unsigned short flag);
    void EnableDBSaveFlag();
    unsigned char GetDBSaveFlag();
    unsigned int GetGuildExp();
    void AddGuildExp(unsigned int exp);
    unsigned char GetGuildLevel();
    unsigned char GetGuildRank();
    unsigned int GetMasterId();
    unsigned int GetGuildFund();
    void AddGuildFund(unsigned int fund);
    void SubGuildFund(unsigned int fund);
    unsigned int IsAddableGuildFund(unsigned int fund);
    bool IsCompleteGuildFund();
    int InsertGuildMember(unsigned int charNo, CUser* user);
    int DeleteGuildMember(unsigned int charNo, CUser* user);
    CUser* FindGuildMember(unsigned int charNo);
    void AddGuildMemberPoint(unsigned int charNo, unsigned int point);
    void AddGuildExpUntilLimit(unsigned int exp, unsigned int limit);
    void GuildSkillPointUp(unsigned short point);
    void CheckGuildSkill();
    void ResetGuildPointRank();
    void IncPowerJoinCount();
    void LoadGuild(STGuildDBInfoOnly& info, char* name);
    void SaveGuild(unsigned char flag, CServerHandler* handler, unsigned int param);
    void DBGuildSave(unsigned char flag, CServerHandler* handler, unsigned int param);
    void SetGuildMessage(char* msg);
    void QueryGuild(CServerHandler* handler, unsigned int charNo);
    void SendGuildInfoToMemberOnly(CUser* user);
    void QueryTodayGuildMember(CServerHandler* handler);
    void SetTodayGuildMember(STTodayGuildMember& member);
    void NotifyTodayGuildMember(CUser* user);
    void LoadGuildAgit(CServerHandler* handler, unsigned int charNo);
    void NoticeGuildMemberLogin_Out(CUser* user, char flag);
    void QueryUnconnGuildMemberProxy(CServerHandler* handler, unsigned int charNo);
    int LoadGuildOneMemberProxy(STGuildMemberProxy& proxy);
    int LoadGuildOneMemberProxy(CUser* user);
    void IncTotalCnt_Of_GuildDBInfo();
    void SendGuildInfoToMembers(bool flag);
    void MakeGameServerSendUserInfoPacket(CUser* user, unsigned int guildKey);
    void DBGuildMemberSave(CUser* user, unsigned char flag, CServerHandler* handler,
                       unsigned char param);
    bool InsertGuildMemberChanglableInfo(unsigned int charNo);
    void SendToGuild(PacketHeader* pkt);
    void SendToGuildForMail();
    void NotifyMessageToGuildMember();
    void SendGuildInfoToManagers();
    void SendGuildNameChangeToMembers();
    void SendGuildAgitInfoToMembers();
    void NoticeChatMsgToGuildMembers(unsigned int charNo, char* msg, int len,
                                     const char* name);
    void NoticeChatMsgToGuildMembersHyperLink(unsigned int charNo, char* msg, int len,
                                          unsigned char type,
                                              const hyperlink_item_info* link,
                                              const char* name);
    void UpdateChangableInfoProcess();
    void NoticeEnterToGuildMember(char* info);
    void NoticeSecedeToGuildMember(char* info);
    bool IsEmpty();
    STGuildDBInfo* GetGuildDBInfo();
    bool IsExistGuildAgit();
    void SetGuildAgitFlag(bool flag);
    unsigned char GetCurSubGuildMasterCnt();
    unsigned short GetTotalCnt_Of_GuildDBInfo();
    void DecTotalCnt_Of_GuildDBInfo();
    bool CheckPowerSecedeTime();
    int ChangeGuildMemberCharName(unsigned int charNo, char* name);
    void AddGuildPoint(unsigned short point);
    void SetGuildAgitInfo(STGuildAgitDBInfo& info);
    void SetGuildAgitLevelUp();
    void CreateGuildAgit(CServerHandler* handler, unsigned int a, unsigned int b,
                     unsigned int c, unsigned int d);
    void DeleteGuildAgit(CServerHandler* handler, unsigned int a, unsigned int b);
    void UpgradeGuildAgit(CServerHandler* handler, unsigned int a, unsigned int b,
                      unsigned int c, unsigned int d);
    void AddGuildMember(ST_Notice_Guild_Enter& info, CUser* user);
    void SetSubGuildMaster(unsigned int charNo, bool flag);
    void SecedeProxyMember(ST_Notice_Guild_Secede& info);
    int ChangeGuildMaster(CServerHandler* handler, CUser* user, unsigned int charNo);
    bool ChangeGuildName(char* name, int flag);
    void ChangeUnconnectedGuildMemberGrade(unsigned int charNo, int grade);
    char* getUnconnectedGuildMemberName(unsigned int charNo);
    void WriteGuildMemberMemo(CUser* user, const char* memo);
    void ReplyGuildMembers(CUser* user);
    int GuildLevelUp(CServerHandler* handler, CUser* user);
    void NotifyMemoToGuildMember(CUser* user, const char* memo);
    void NotifyAllTodayGuildMember();
    void NotifyAllAchieveAttendance(unsigned int charNo, unsigned int phase);
    void NoticeMarkChangeToGuildMember(unsigned int charNo);
    void NoticeGuildMasterDelegateToMembers(char* name);
    void NotifyCreateGuildAgitToGuildMember(unsigned int charNo);
    void NotifyDeleteGuildAgitToGuildMember(unsigned int charNo);
    void DBSaveGuildMembers(unsigned char flag, CServerHandler* handler, unsigned char param);
    void DBGuildSaveProcess(CServerHandler* handler);
    void DBSavePowerSecedeTime(unsigned char flag, CServerHandler* handler);
    void CallGuildAllMembersProxy(CUser* user, CServerHandler* handler);
    void QueryGuildAllMembersProxy(CServerHandler* handler, unsigned int charNo);
    void LoadGuildAllMembersProxy(STGuildMemberProxy* proxy, unsigned char flag,
                              unsigned char param);
    int PopGuildMemberChanglableInfo(unsigned int charNo,
                                     STGuildMemberChangableInfo& info) const;
    int ReplyGuildMembersToWeb(STGuildMemberWebConnInfo* info);
    void DBSaveGuildMemberUnChangableInfo(CServerHandler* handler, unsigned int a,
                                      unsigned int b, char* name);
    void DismissGuildMemberAndNotice(int group);
    void ReplyGuildAllMembers(CUser* user);
    int BuyGuildSkill(int skillId, int slot, short param, unsigned int charNo);
    unsigned char GetPowerSide();
    void SetPowerSide(unsigned char side);
    unsigned int GetPowerWarPoint();
    void AddPowerWarPoint(unsigned int point);
    void SubPowerWarPoint(unsigned int point);
    void SetPowerSecedeTime(unsigned int time);
    CGuildCargo* GetGuildCargo();
    CGuildBoard* GetGuildBoard();
    std::map<unsigned int, CUser*> m_members;  // +0
    unsigned int m_guildKey;                    // +0x18
    unsigned short m_field1c;                   // +0x1c
    unsigned short m_field1e;                   // +0x1e
    STGuildDBInfo m_dbInfo;                     // +0x20
    STGuildAgitDBInfo m_agitInfo;               // +0x4d09
    char m_field4d0a[100];                      // +0x4d0a
    char m_pad4d6e[2];                          // +0x4d6e
    unsigned short m_field4d70;                 // +0x4d70
    unsigned short m_field4d72;                 // +0x4d72
    unsigned short m_field4d74;                 // +0x4d74
    char m_field4d76[0x1c];                     // +0x4d76（未知区域）
    unsigned short m_field4d92;                 // +0x4d92
    unsigned short m_field4d94;                 // +0x4d94
    unsigned char m_field4d96;                  // +0x4d96
    std::map<unsigned int, STGuildMemberChangableInfo> m_changable;  // +0x4d98
    unsigned char m_field4db0;                  // +0x4db0
    char m_pad4db1[3];                          // 对齐：cargo 从 +0x4db4 开始
    CGuildCargo m_cargo;                        // +0x4db4（内嵌对象）
    CGuildBoard m_board;                        // +0x66c4（内嵌对象）
};

#pragma pack(push, 1)
// from GuildPackets.h
class Packet_Achieve_Guild_Attendance : public PacketHeader {
public:
    Packet_Achieve_Guild_Attendance();
    char m_data[0x10];
};

// from GuildPackets.h
class Packet_Channel_Create_Guild_Agit : public PacketHeader {
public:
    Packet_Channel_Create_Guild_Agit();
    unsigned int ma;                 // +0xa
    unsigned int me;                 // +0xe
    unsigned int m12;                 // +0x12

};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Channel_Guild_Agit_Info : public PacketHeader {
public:
    Packet_Channel_Guild_Agit_Info();
    unsigned int m_a;              // +0xa
    unsigned int m_b;              // +0xe
    unsigned int m_c;              // +0x12
    STGuildAgitDBInfo m_info;      // +0x16
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_DBMW_Save_Power_Secede_Time : public PacketHeader {
public:
    Packet_DBMW_Save_Power_Secede_Time();
    unsigned char m_flag;        // +0xa
    unsigned int m_key;          // +0xb
};

// from GuildPackets.h
class Packet_DB_Call_Guild_All_Members : public PacketHeader {
public:
    Packet_DB_Call_Guild_All_Members();
    unsigned int ma;                 // +0xa
    unsigned int me;                 // +0xe

};

// from GuildPackets.h
class Packet_DB_Call_Unconn_Guild_Member : public PacketHeader {
public:
    Packet_DB_Call_Unconn_Guild_Member();
    unsigned int ma;                 // +0xa
    unsigned int me;                 // +0xe

};

// from GuildPackets.h
class Packet_DB_Create_Guild_Agit : public PacketHeader {
public:
    Packet_DB_Create_Guild_Agit();
    unsigned int m0xa;                 // +0xa
    unsigned int m0xe;                 // +0xe

};

// from GuildPackets.h
class Packet_DB_Delete_Guild_Agit : public PacketHeader {
public:
    Packet_DB_Delete_Guild_Agit();
    unsigned int ma;                 // +0xa
    unsigned int me;                 // +0xe

};

// from GuildPackets.h
class Packet_DB_Load_Guild_Agit : public PacketHeader {
public:
    Packet_DB_Load_Guild_Agit();
    unsigned int m_charNo;   // +0xa
};

// from GuildPackets.h
class Packet_DB_Upgrade_Guild_Agit : public PacketHeader {
public:
    Packet_DB_Upgrade_Guild_Agit();
    unsigned int ma;                 // +0xa
    unsigned int me;                 // +0xe

};

// from GuildPackets.h
class Packet_Guild_Notice_Guild_Master_Delegate : public PacketHeader {
public:
    Packet_Guild_Notice_Guild_Master_Delegate();
    char m_data[0x26];
};

// from GuildPackets.h
class Packet_Guild_Notice_Guild_Name_Change : public PacketHeader {
public:
    Packet_Guild_Notice_Guild_Name_Change();
    char m_data[0x23];
};

// from GuildPackets.h
class Packet_Guild_Notify_Guild_Member_Memo : public PacketHeader {
public:
    Packet_Guild_Notify_Guild_Member_Memo();
    char m_data[0x3b];
};

// from GuildPackets.h
class Packet_Guild_Notify_Message_To_Guild_Mem : public PacketHeader {
public:
    Packet_Guild_Notify_Message_To_Guild_Mem();
    char m_data[0x6d];
};

// from GuildPackets.h
class Packet_Monitor_Guild_Chat_ToUser : public PacketHeader {
public:
    Packet_Monitor_Guild_Chat_ToUser();
    char m_data[0x127];
};

// from GuildPackets.h
class Packet_Monitor_Guild_Chat_ToUser_Hyper_Link : public PacketHeader {
public:
    Packet_Monitor_Guild_Chat_ToUser_Hyper_Link();
    char m_data[0x260];
};

// from GuildPackets.h
class Packet_Monitor_Notice_Guild_Dismiss_ToUser : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Dismiss_ToUser();
    unsigned int ma;                 // +0xa
    unsigned int me;                 // +0xe

};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Monitor_Notice_Guild_Enter_ToUser : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Enter_ToUser();
    unsigned int m_a;              // +0xa
    unsigned int m_b;              // +0xe
    char m_pad12[2];               // +0x12
    ST_Notice_Guild_Enter m_info;  // +0x14
    char m_pad55[5];               // +0x55
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_Monitor_Notice_Guild_Info : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Info();
    char m_pad[8];
    unsigned int m_field12;          // +0x12
    STGuildDBInfoOnly m_info;        // +0x16
    char m_padD3;                    // +0xd3（ORIG 布局：m_info 后 1 字节间隙）
    char m_rest[0x65];               // +0xd4
};

// from GuildPackets.h
class Packet_Monitor_Notice_Guild_Member_Login_out : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Member_Login_out();
    char m_data[0x28];
};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Monitor_Notice_Guild_Secede_ToUser : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Secede_ToUser();
    unsigned int m_a;              // +0xa
    unsigned int m_b;              // +0xe
    ST_Notice_Guild_Secede m_info; // +0x12
};
#pragma pack(pop)

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Monitor_SAVE_Guild : public PacketHeader {
public:
    Packet_Monitor_SAVE_Guild();
    unsigned char m_flag;          // +0xa
    unsigned int m_b;              // +0xb
    STGuildDBInfoOnly m_info;      // +0xf
    unsigned int m_param;          // +0xcc
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_Notify_Today_Guild_Member : public PacketHeader {
public:
    Packet_Notify_Today_Guild_Member();
    char m_data[0x33];
};

// from GuildPackets.h
class Packet_Query_Today_Guild_Member : public PacketHeader {
public:
    Packet_Query_Today_Guild_Member();
    unsigned int ma;                 // +0xa

};

// from GuildPackets.h
class Packet_Monitor_Notice_Guild_Mark_Change_ToUser : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Mark_Change_ToUser();
    char m_data[0xc];
};

#pragma pack(pop)
extern int g_guildDBProcessDay;

#endif
