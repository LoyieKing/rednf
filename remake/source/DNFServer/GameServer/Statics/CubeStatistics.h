#ifndef CUBE_STATISTICS_H_
#define CUBE_STATISTICS_H_

#include <map>

#include "GMAccounts.h"
#include "PacketHeader.h"

class CServerHandler;
class Packet_Cube_Statistic;

// ---- CCubeStatistic：0x18 ----
class CCubeStatistic
{
public:
    CCubeStatistic();
    ~CCubeStatistic();
    void addStatisticData(Packet_Cube_Statistic* pkt);
    void sendStatisticData(CServerHandler* handler);
    void printStatisticData();
    void resetStatisticData();
    std::map<STCubeStatisticKey, int> m_data;
};

// ---- Cube 统计上报包 ----
#pragma pack(push, 1)
class Packet_DBMW_Cube_Statistic : public PacketHeader
{
public:
    Packet_DBMW_Cube_Statistic();
    int m_count;             // +0xa
    char m_rest[0x17de];     // +0xe
} __attribute__((packed));
#pragma pack(pop)

#endif // CUBE_STATISTICS_H_
