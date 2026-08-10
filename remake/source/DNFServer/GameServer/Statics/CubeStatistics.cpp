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
    key.m_field0 = (unsigned int)(short)((const CubePkt*)pkt)->m_a;
    key.m_field4 = ((const CubePkt*)pkt)->m_c;
    key.m_field8 = (unsigned int)(short)((const CubePkt*)pkt)->m_b;
    key.m_fieldc = ((const CubePkt*)pkt)->m_e;
    int value = ((const CubePkt*)pkt)->m_d;
    std::map<STCubeStatisticKey, int>::iterator it = m_data.find(key);
    if (it != m_data.end())
    {
        it->second += value;
    }
    else
    {
        m_data.insert(std::make_pair(key, value));
    }
}
void CCubeStatistic::sendStatisticData(CServerHandler* handler)
{
    Packet_DBMW_Cube_Statistic pkt;
    unsigned int count = 0;
    if (!m_data.empty())
    {
        for (std::map<STCubeStatisticKey, int>::iterator it = m_data.begin();
             it != m_data.end(); ++it)
        {
            *(short*)((char*)&pkt + count * 0xd + 0xe) = (short)it->first.m_field0;
            *(int*)((char*)&pkt + count * 0xd + 0x12) = (int)it->first.m_field4;
            *(short*)((char*)&pkt + count * 0xd + 0x10) = (short)it->first.m_field8;
            *(char*)((char*)&pkt + count * 0xd + 0x1a) = it->first.m_fieldc;
            *(int*)((char*)&pkt + count * 0xd + 0x16) = it->second;
            count++;
            if (0x1d5 < count)
            {
                *(unsigned int*)((char*)&pkt + 10) = 0x1d6;
                handler->SendToDB((PacketHeader*)&pkt);
                count = 0;
                DNF_LOG_SCOPE_LINE(0x40, "./log/statistic", "cube statistic DB Sent %d", 0x1d6);
            }
        }
        if (0 < (int)count)
        {
            *(unsigned int*)((char*)&pkt + 10) = count;
            *(unsigned short*)((char*)&pkt + 2) = (unsigned short)(count * 0xd + 0xe);
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x49, "./log/statistic", "cube statistic DB Sent %d", count);
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
               it->first.m_field0, it->first.m_field4, it->first.m_field8,
               (unsigned int)it->first.m_fieldc, it->second);
    }
}
Packet_DBMW_Cube_Statistic::Packet_DBMW_Cube_Statistic()
    : PacketHeader(0xc34, 0x17ec)
{
    m_count = 0;
    memset(m_rest, 0, sizeof(m_rest));
}
CCubeStatistic::CCubeStatistic()
{
}
CCubeStatistic::~CCubeStatistic()
{
}
