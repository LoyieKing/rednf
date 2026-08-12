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
#include "PowerWarTypes.h"

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
    union {
        char m_data[0xc];
        unsigned int m_field[3];
    };
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
    void GetBonus(Packet_DB_Save_Power_War_Bonus_Point& pkt);
    int GetBonus(int idx);
    void CalcBonus();
    std::map<unsigned int, STPowerWarCharacInfo*> m_map;   // +4（vptr@0）
    std::vector<STPowerWarCharacInfo*> m_vec;              // +0x1c
    std::list<STUserPoint> m_list;                         // +0x28
};

#endif
