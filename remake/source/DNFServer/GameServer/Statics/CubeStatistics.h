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
struct CubeStatisticItem
{
    unsigned short m_channel;   // +0x0（CubeStatisticView.channel）
    unsigned short m_level;     // +0x2（CubeStatisticView.level）
    unsigned int m_itemIndex;   // +0x4（CubeStatisticView.item_index）
    unsigned int m_itemCount;   // +0x8（CubeStatisticView.item_count）
    unsigned char m_type;       // +0xc（CubeStatisticView.type）
} __attribute__((packed));

class Packet_DBMW_Cube_Statistic : public PacketHeader
{
public:
    Packet_DBMW_Cube_Statistic();
    int m_count;             // +0xa
    CubeStatisticItem m_items[470];  // +0xe, 470 * 0xd = 0x17de
} __attribute__((packed));
#pragma pack(pop)

#endif // CUBE_STATISTICS_H_
