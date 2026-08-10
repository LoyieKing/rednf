#ifndef POWERWARCHARACINFO_H_
#define POWERWARCHARACINFO_H_

#include <map>
#include <queue>
#include <deque>
#include <list>
#include <algorithm>
#include <string>
#include <vector>
#include "Thread.h"

#include "PacketHeader.h"

class CPowerWarCharacInfo;
struct Packet_DB_Save_Power_War_Bonus_Point;
struct STPowerWarCharacInfo;
struct STUserRank;

// from GuildDomain.h
struct STPowerWarCharacInfo
{
    STPowerWarCharacInfo();
    static bool Compare(const STPowerWarCharacInfo* a, const STPowerWarCharacInfo* b);
    static void* operator new(unsigned int size);
    static void operator delete(void* p);
    static void operator delete(void* p, unsigned int size);
    char m_data[0x10];
};

// from GuildDomain.h
class CPowerWarCharacInfo
{
public:
    CPowerWarCharacInfo();
    virtual ~CPowerWarCharacInfo();
    void Initialize();
    void Clean();
    int IsExistCharac(unsigned int charNo);
    unsigned int GetUserRanking(unsigned int charNo);
    void PrintDebugInfo();
    void CalcAllUserRanking();
    STPowerWarCharacInfo* FindPowerwarCharac(unsigned int charNo);
    std::vector<STPowerWarCharacInfo*>* GetCharacInfoVector();
    STPowerWarCharacInfo* CreatePowerwarCharac();
    unsigned int GetUserPowerWarPoint(unsigned int charNo);
    int InsertPowerwarCharac(unsigned int charNo, STPowerWarCharacInfo* info);
    void GetAllUserRankingInfo(unsigned int& count, STUserRank* rank);
    void GetStatueRankingUsers(std::vector<STPowerWarCharacInfo*>& vec);
    void UpdatePowerwarCharacInfo(unsigned int charNo, unsigned short point);
    int GetBonus(Packet_DB_Save_Power_War_Bonus_Point& pkt);
    int GetBonus(int idx);
    void CalcBonus();
    char m_data[0x2c];   // vptr@0 + map@+4(0x14) + vector@+0x1c(0xc) + list@+0x28(8) = 0x30
};

#endif
