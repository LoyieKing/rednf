// df_statics_r — CubeStatistics（ORIG CubeStatistics.o 拆分）
#include <stdio.h>
#include <string.h>

#include "CubeStatistics.h"
#include "DNFServerHandler.h"
#include "DNFFileLog.h"
#include "Packet_Cube_Statistic.h"

namespace
{
// Packet_Cube_Statistic 载荷镜像（与 ORIG 读取偏移一致）
struct CubePkt
{
    char pad[0xa];
    unsigned short m_a;   // +0xa
    unsigned short m_b;   // +0xc
    unsigned int m_c;     // +0xe
    int m_d;              // +0x12
    unsigned char m_e;    // +0x16
} __attribute__((packed));
}
void CCubeStatistic::addStatisticData(Packet_Cube_Statistic* pkt)
{
    STCubeStatisticKey key;
    key.m_channel = (unsigned int)(short)((const CubePkt*)pkt)->m_a;
    key.m_index = ((const CubePkt*)pkt)->m_c;
    key.m_level = (unsigned int)(short)((const CubePkt*)pkt)->m_b;
    key.m_type = ((const CubePkt*)pkt)->m_e;
    int value = ((const CubePkt*)pkt)->m_d;
    std::map<STCubeStatisticKey, int>::iterator it = m_data.find(key);
    if (it != m_data.end())
        it->second += value;
    else
        m_data.insert(std::make_pair(key, value));
}
void CCubeStatistic::sendStatisticData(CServerHandler* handler)
{
    Packet_DBMW_Cube_Statistic pkt;
    int count = 0;
    if (!m_data.empty())
    {
        for (std::map<STCubeStatisticKey, int>::iterator it = m_data.begin();
             it != m_data.end(); ++it)
        {
            pkt.m_items[count].m_channel = it->first.m_channel;
            pkt.m_items[count].m_itemIndex = (int)it->first.m_index;
            pkt.m_items[count].m_level = it->first.m_level;
            pkt.m_items[count].m_type = it->first.m_type;
            pkt.m_items[count].m_itemCount = (int)it->second;
            count = count + 1;
            if (0x1d5U < count)
            {
                pkt.m_count = 0x1d6;
                handler->SendToDB((PacketHeader*)&pkt);
                count = 0;
                DNF_LOG_SCOPE_LINE(0x40, "./log/statistic", "CCubeStatistic::sendStatisticData : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", 0x1d6);
            }
        }
        if (count > 0)
        {
            pkt.m_count = count;
            pkt.packetSize = (unsigned short)(count * 0xd + 0xe);
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x49, "./log/statistic", "CCubeStatistic::sendStatisticData : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", count);
        }
    }
}
void CCubeStatistic::resetStatisticData()
{
    m_data.clear();
}
void CCubeStatistic::printStatisticData()
{
    for (std::map<STCubeStatisticKey, int>::iterator it = m_data.begin();
         it != m_data.end(); ++it)
    {
        printf("\nchannel %d, index %d, level %d, type %d, count %d\n",
               it->first.m_channel, it->first.m_index, it->first.m_level,
               (unsigned int)it->first.m_type, it->second);
    }
}
Packet_DBMW_Cube_Statistic::Packet_DBMW_Cube_Statistic()
    : PacketHeader(0xc34, 0x17ec)
{
    m_count = 0;
    memset(m_items, 0, sizeof(m_items));
}
CCubeStatistic::CCubeStatistic()
{
}
CCubeStatistic::~CCubeStatistic()
{
}
