#ifndef POWERWARGUILDINFO_H_
#define POWERWARGUILDINFO_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"

class CGuildManager;
class CPowerWarGuildInfo;
struct Packet_DB_Save_Power_War_Point_Reward;
struct STDBSavePowerWarPoint;
struct STGuildRank;
struct STPowerWarGuildInfo;

// from GuildDomain.h
struct STPowerWarGuildInfo
{
    STPowerWarGuildInfo();
    static bool Compare(const STPowerWarGuildInfo* a, const STPowerWarGuildInfo* b);
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    char m_data[0x14];
};

// from GuildDomain.h
struct STDBSavePowerWarPoint
{
    STDBSavePowerWarPoint();
    char m_data[8];   // {guildKey@0, point@4}
};

// from GuildDomain.h
class CPowerWarGuildInfo
{
public:
    CPowerWarGuildInfo();
    virtual ~CPowerWarGuildInfo();
    void Initialize();
    void Clean();
    STPowerWarGuildInfo* CreatePowerwarGuild();
    void DeletePowerWarGuild(STPowerWarGuildInfo* info);
    STPowerWarGuildInfo* FindPowerwarGuild(unsigned int guildKey);
    int InsertPowerwarGuild(unsigned int guildKey, STPowerWarGuildInfo* info);
    STPowerWarGuildInfo* GetSpecificGuildInfo(unsigned int guildKey);
    unsigned int GetGuildRanking(unsigned int guildKey);
    void CalcAllGuildRanking();
    void PrintDebugInfo();
    void UpdateGuildPowerwarInfo(unsigned int guildKey, unsigned short point);
    void RewardGuildPowerWarPoint(CGuildManager& gm, bool a, int b, int c, int d, int e);
    STDBSavePowerWarPoint* CreateDBSavePowerWarPoint();
    void DeleteDBSavePowerWarPoint(STDBSavePowerWarPoint* p);
    void MakePacketDBPowerWarPoint(Packet_DB_Save_Power_War_Point_Reward* pkt);
    int GetPowerWarPointDBSaveCount();
    void GetAllGuildRankingInfo(int& count, STGuildRank* rank);
    std::map<unsigned int, STPowerWarGuildInfo*> m_map;   // +4（vptr@0）
    std::vector<STPowerWarGuildInfo*> m_vec;              // +0x1c
    std::vector<STDBSavePowerWarPoint*> m_vec2;           // +0x28
};

#endif
