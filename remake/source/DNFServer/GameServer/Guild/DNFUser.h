#ifndef DNFUSER_H_
#define DNFUSER_H_

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
#include "DNFUser.h"
#include "MemPool.h"

class CBlackUser;
class CGameServer;
class CGuild;
class CServerHandler;
class CTcpGameServer;
class CUser;
struct Packet_Monitor_Notice_Guild_Member_Info;
struct Packet_Monitor_SAVE_Guild_Member;
struct Packet_Monitor_Set_Guild_Key;
struct Packet_Send_All_User_Info_Minimum_For_Guild_System;
struct STBlackUserDBType;
struct STGuildMemerDBInfo;

// from GuildDomain.h
struct STGuildMemerDBInfo
{
    STGuildMemerDBInfo();
    unsigned char m_data[0x1a];
};

// from GuildDomain.h
class CUser
{
public:
    CUser();
    ~CUser();
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    unsigned int GetDBID();
    void SetDBID(unsigned int dbid);
    unsigned int GetUniqCharNo();
    void SetUniqCharNo(unsigned int charNo);
    int GetIdByChannel();
    void SetIdByChannel(int channel);
    CGameServer* GetGameServer();
    void SetGameServer(CGameServer* server);
    CTcpGameServer* GetTcpGameServer();
    void SetTcpGameServer(CTcpGameServer* server);
    void SetUserPosState(unsigned char state);
    char* GetCharName();
    void ChangeCharName(char* name);
    void ResetCharInfo();
    void SetUserInfo_CharNo(char sex, char job, short flag, unsigned int charNo, char* name);
    void SetUserInfo(unsigned int dbid, unsigned int charNo, char* name, int channel,
                     CGameServer* server);
    unsigned char GetJob();
    unsigned char GetGrowthType();
    unsigned char GetSex();
    void SetSex(unsigned char sex);
    void SetSsn(char* ssn);
    char* GetSsn();
    unsigned char GetLevel();
    unsigned int GetGuildKey();
    void ResetGuild();
    void AttachGuild(CGuild* guild);
    void DetachGuild();
    unsigned short GetGuildMemFlag();
    void SetGuildMemFlag(unsigned short flag);
    void ResetGuildMemFlag(unsigned short flag);
    bool IsSetGuildMemFlag(unsigned short flag);
    bool IsSubGuildMaster();
    void SetGuildInviteFact(unsigned int guildId, unsigned int callerId, unsigned char fact);
    unsigned int GetGuildInviteGuildId();
    unsigned int GetGuildInviteCallerId();
    void GuildInviteProcess();
    void SetGuildMemberMemo(const char* memo);
    void SetUserChangableInfo(short type, char value);
    void AddGuildMemberPoint(unsigned int point);
    void ResetGuildPoint();
    void SetGuildMessage(char* msg);
    void SendTcpGameserver(PacketHeader* pkt);
    void SendToGameserver(char* buf, int len);
    void QueryGuildMember(CServerHandler* handler);
    void SendGuildMemberDBInfo(STGuildMemerDBInfo& info);
    void LoadGuildMember(unsigned int guildKey, STGuildMemerDBInfo& info);
    STGuildMemerDBInfo* GetGuildMemDBInfo();
    void SaveGuildMember(unsigned char type, unsigned int value, CServerHandler* handler,
                         unsigned char flag);
    void ChangeGuildMemberGrade(unsigned char grade);
    void SendSetGuildKeyToUser(unsigned int guildKey, unsigned int grade);
    void MakeGameServerSendUserInfoPacket(unsigned int charNo);
    bool RegisterToBlackList(unsigned int charNo, char* name);
    bool RegisterToBlackList(unsigned int charNo, char* name, unsigned int param);
    int DeleteToBlackList(unsigned int charNo);
    void ResetBlackList();
    int IsBlackUser(unsigned int charNo);
    void GetBlackList(unsigned char& count, STBlackUserDBType* list);
    void GetBlackList(unsigned char& count, unsigned int* list);
    unsigned short GetBlackListSize();
    unsigned short GetBlackListDBFlag();
    void SetBlackListDBFlag(unsigned short flag);
    void* GetMapBlackList();
    void RegisterToCashBlackList(std::map<unsigned int, CBlackUser*>& map);
    unsigned int m_dbid;              // +0
    unsigned int m_charNo;            // +4
    CGameServer* m_gameServer;        // +8
    CTcpGameServer* m_tcpGameServer;  // +0xc
    unsigned char m_posState;         // +0x10
    int m_channel;                    // +0x14
    char m_charInfo[0x1e];            // +0x18（name@0x18, level/job/sex 等）
    char m_job;                       // +0x36
    char m_growthType;                // +0x37
    short m_guildMemFlag;             // +0x38
    unsigned char m_field3a;          // +0x3a
    char m_field3b[7];                // +0x3b
    CGuild* m_guild;                  // +0x44
    unsigned short m_field48;         // +0x48
    STGuildMemerDBInfo m_guildDBInfo; // +0x4a
    std::map<unsigned int, CBlackUser*> m_blackList;  // +0x64
    unsigned short m_field7c;         // +0x7c
    char m_field7e;                   // +0x7e
    unsigned int m_field80;           // +0x80
    unsigned int m_field84;           // +0x84
    void* m_field88;                  // +0x88（MemPool 空闲链指针，尺寸 0x8c）
};

// from GuildPackets.h
class Packet_Monitor_Notice_Guild_Member_Info : public PacketHeader {
public:
    Packet_Monitor_Notice_Guild_Member_Info();
    char m_pad[8];
    STGuildMemerDBInfo m_info;   // +0x12
};

// from GuildPackets.h
#pragma pack(push,1)
class Packet_Monitor_SAVE_Guild_Member : public PacketHeader {
public:
    Packet_Monitor_SAVE_Guild_Member();
    unsigned char m_flag;          // +0xa
    unsigned int m_b;              // +0xb
    unsigned int m_charNo;         // +0xf
    STGuildMemerDBInfo m_info;     // +0x13
    char m_pad2d;                  // +0x2d
};
#pragma pack(pop)

// from GuildPackets.h
class Packet_Monitor_Set_Guild_Key : public PacketHeader {
public:
    Packet_Monitor_Set_Guild_Key();
    char m_data[0xc];
};

// from GuildPackets.h
class Packet_Send_All_User_Info_Minimum_For_Guild_System : public PacketHeader {
public:
    Packet_Send_All_User_Info_Minimum_For_Guild_System();
    char m_data[0xc];
};

#endif
