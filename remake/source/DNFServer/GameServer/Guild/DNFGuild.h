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
struct STGuildDBInfoOnly
{
    STGuildDBInfoOnly();
    union
    {
        char m_data[0xbd];
        struct __attribute__((packed))
        {
            char m_pad0[0x17];
            unsigned int m_masterId;       // +0x17（this+0x37）
            unsigned char m_guildLevel;    // +0x1b（this+0x3b）
            char m_pad2[0x22 - 0x1c];
            unsigned short m_totalCnt;     // +0x22（this+0x42）
            unsigned int m_guildPoint;     // +0x24（this+0x44）
            unsigned char m_guildRank;     // +0x28（this+0x48）
            unsigned int m_guildExp;       // +0x29（this+0x49）
            char m_pad5[0x2d - 0x2d];
            unsigned char m_subMasterCnt;  // +0x2d（this+0x4d）
            char m_pad6[0x95 - 0x2e];
            unsigned char m_powerSide;     // +0x95（this+0xb5）
            unsigned int m_powerSecedeTime;// +0x96（this+0xb6）
            unsigned int m_powerWarPoint;  // +0x9a（this+0xba）
            unsigned char m_agitFlag;      // +0x9e（this+0xbe）
            char m_pad7[0xa0 - 0x9f];
            unsigned int m_guildFund;      // +0xa0（this+0xc0）
            char m_pad8[0xbd - 0xa4];
        };
    };
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
struct RandomOptionField
{
    void reset();
    char m_data[3];
};

// from GuildDomain.h
struct RandomOptionSeed
{
    void reset();
    char m_data[1];
};

// from GuildDomain.h
struct RandomOption
{
    void reset();
    char m_data[0xe];
};

// from GuildDomain.h
struct UpgradeSeparateInfo
{
    UpgradeSeparateInfo();
    void reset();
    unsigned char GetUpgradeSeparate() const;
    char m_data[1];
};

// from GuildDomain.h
struct ReservedCapacity
{
    ReservedCapacity();
    void reset();
    char m_data[0xc];
};

// from GuildDomain.h
struct STGuildMemberCharacData
{
    STGuildMemberCharacData();
    char m_data[0x21];
};

// from GuildDomain.h
struct hyperlink_item_info
{
    char m_data[0x40];
};

// from GuildDomain.h
class DnfItemInfo
{
public:
    DnfItemInfo();
    void reset();
    char m_data[0x35];
};

// from GuildDomain.h
struct STGuildMemberWebConnInfo
{
    char m_data[0x5dc];
};

// from GuildDomain.h
struct STGuildCallInfo
{
    STGuildCallInfo();
    char m_data[0x1d];
};

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
    bool GetDBSaveFlag();
    unsigned int GetGuildExp();
    void AddGuildExp(unsigned int exp);
    unsigned char GetGuildLevel();
    unsigned char GetGuildRank();
    unsigned int GetMasterId();
    unsigned int GetGuildFund();
    void AddGuildFund(unsigned int fund);
    void SubGuildFund(unsigned int fund);
    bool IsAddableGuildFund(unsigned int fund);
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
    void InsertGuildMemberChanglableInfo(unsigned int charNo);
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
    int CheckPowerSecedeTime();
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
    char m_data[0xc];
};

// from GuildPackets.h
class Packet_Channel_Guild_Agit_Info : public PacketHeader {
public:
    Packet_Channel_Guild_Agit_Info();
    char m_data[0xd];
};

// from GuildPackets.h
class Packet_DBMW_Save_Power_Secede_Time : public PacketHeader {
public:
    Packet_DBMW_Save_Power_Secede_Time();
    char m_data[0x5];
};

// from GuildPackets.h
class Packet_DB_Call_Guild_All_Members : public PacketHeader {
public:
    Packet_DB_Call_Guild_All_Members();
    char m_data[0xa];
};

// from GuildPackets.h
class Packet_DB_Call_Unconn_Guild_Member : public PacketHeader {
public:
    Packet_DB_Call_Unconn_Guild_Member();
    char m_data[0xa];
};

// from GuildPackets.h
class Packet_DB_Create_Guild_Agit : public PacketHeader {
public:
    Packet_DB_Create_Guild_Agit();
    char m_data[0xa];
};

// from GuildPackets.h
class Packet_DB_Delete_Guild_Agit : public PacketHeader {
public:
    Packet_DB_Delete_Guild_Agit();
    char m_data[0xa];
};

// from GuildPackets.h
class Packet_DB_Load_Guild_Agit : public PacketHeader {
public:
    Packet_DB_Load_Guild_Agit();
    char m_data[0xa];
};

// from GuildPackets.h
class Packet_DB_Upgrade_Guild_Agit : public PacketHeader {
public:
    Packet_DB_Upgrade_Guild_Agit();
    char m_data[0xa];
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
    char m_data[0xa];
};

// from GuildPackets.h
class Packet_Monitor_Notice_Guild_Enter_ToUser : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Enter_ToUser();
    char m_data[0x50];
};

// from GuildPackets.h
class Packet_Monitor_Notice_Guild_Info : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Info();
    char m_data[0x12f];
};

// from GuildPackets.h
class Packet_Monitor_Notice_Guild_Member_Login_out : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Member_Login_out();
    char m_data[0x28];
};

// from GuildPackets.h
class Packet_Monitor_Notice_Guild_Secede_ToUser : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Secede_ToUser();
    char m_data[0x4b];
};

// from GuildPackets.h
class Packet_Monitor_SAVE_Guild : public PacketHeader {
public:
    Packet_Monitor_SAVE_Guild();
    char m_data[0xc6];
};

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
    char m_data[0xa];
};

// from GuildPackets.h
class Packet_Monitor_Notice_Guild_Mark_Change_ToUser : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Mark_Change_ToUser();
    char m_data[0xc];
};

extern int g_guildDBProcessDay;

#endif
