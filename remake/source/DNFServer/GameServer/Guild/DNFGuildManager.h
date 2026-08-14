#ifndef DNFGUILDMANAGER_H_
#define DNFGUILDMANAGER_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"
#include "DNFGuildManager.h"
#include "DNFGuildWar.h"
#include "Scheduler.h"

class CApplication;
class CGuild;
class CGuildManager;
class CServerHandler;
class CUser;
struct STAttendanceInfo;
struct STGuildDBInfoOnly;
struct STTodayGuildMember;
struct ST_Notice_Guild_Enter;
struct ST_Notice_Guild_Secede;

// from GuildDomain.h
// ORIG 二进制实测（CGuild::SetTodayGuildMember 拷贝 0x27 字节：9×dword+word+byte）
// 结构体总大小 0x28；与 DBMW DNFPacket.h 字段布局一致（charac_no + name[0x1e] + 5 uchar）。
struct STTodayGuildMember
{
    char m_data[0x28];
};

// from GuildDomain.h
struct STAttendanceInfo
{
    STAttendanceInfo();
    char m_data[0x1c];
};

// from GuildDomain.h
class CGuildManager
{
public:
    CGuildManager();
    ~CGuildManager();
    void Init(CApplication* app);
    void Process();
    void ProcessByMinute();
    void ProcessBySecond();
    void DBSaveProcess(CApplication* app, bool force);
    void AttendGuild(unsigned int guildKey, unsigned int charNo);
    CGuild* GuildMemLogin(unsigned int guildKey, CUser* user);
    void GuildMemLogout(unsigned int guildKey, CUser* user);
    CGuild* GuildEnter(unsigned int guildKey, ST_Notice_Guild_Enter& info);
    CGuild* GuildSecede(unsigned int guildKey, ST_Notice_Guild_Secede& info);
    CGuild* CreateGuild(unsigned int guildKey, CServerHandler* handler,
                        unsigned int masterId);
    int DeleteGuild(unsigned int guildKey);
    int DeleteGuild(CGuild* guild);
    void GuildDismiss(CGuild* guild);
    bool InsertGuild(unsigned int guildKey, CGuild* guild);
    CGuild* FindGuild(unsigned int guildKey);
    bool IsEmptyGuild(unsigned int guildKey);
    bool LoadGuild(unsigned int guildKey, STGuildDBInfoOnly& info, char* name);
    void DBGuildProcess(CServerHandler* handler, bool flag);
    void DBGuildAndGuildMemberSave(CServerHandler* handler);
    void DBLoadAllLoginGuild(CServerHandler* handler);
    void SendGuildInfoToMembers(unsigned int guildKey, bool flag);
    void LoadGuildAgit(unsigned int guildKey, CServerHandler* handler);
    STTodayGuildMember* GetTodayMember(unsigned int guildKey);
    void InsertTodayMember(unsigned int guildKey, STTodayGuildMember& member);
    void RefreshTodayMember(bool flag);
    void RefreshAttendanceInfo(bool flag);
    int CheckAchieveAttendance(unsigned int guildKey);
    void RewardAttendance(unsigned int guildKey, unsigned int charNo, int flag);
    int GetAttendancePhase(unsigned int guildKey);
    void GetAttendanceInfo(unsigned int guildKey, STAttendanceInfo& info);
    int GetAttendanceExp(unsigned int guildKey, int phase);
    unsigned int GetGuildExpWithLevel(unsigned char level);
    int GetGuildLevelWithExp(unsigned int exp);
    unsigned int GetMaxGuildExp1();
    unsigned int GetMaxGuildExp2();
    void SetGuildExpTable(unsigned int* table);
    bool IsGuildWarEventOn(unsigned char group);
    bool IsGuildWarEnterableChar(unsigned char group, unsigned int charNo);
    CGuildWar* GetGuildWar();
    void CargoLock();
    void CargoUnlock();
    unsigned char IsCargoLock();
    CApplication* m_app;          // +0
    std::map<unsigned int, CGuild*> m_guilds;  // +4
    CScheduler m_scheduler;       // +0x1c
    unsigned char m_cargoLock;    // +0x40（CargoLock/CargoUnlock/IsCargoLock）
    char m_pad41[3];              // +0x41（对齐）
    CGuildWar m_guildWar;         // +0x44
    std::map<unsigned int, STTodayGuildMember> m_todayMembers;  // +0x58
    char m_time1[0x2c];           // +0x70
    std::map<unsigned int, std::vector<unsigned int> > m_attendance;  // +0x9c
    char m_time2[0x2c];           // +0xb4
    static unsigned int m_ExpTable[17];
    static unsigned int m_uGuildExpMax1;
    static unsigned int m_uGuildExpMax2;
};

#endif
