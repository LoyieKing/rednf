// df_statics_r — Statistics（ORIG Statistics.o 拆分）
#include <stdio.h>
#include <string.h>

#include "Statistics.h"
#include "DNFServerHandler.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "StatisticProxy.h"
#include "Packet_P2P_Statistics.h"
#include "Packet_Server_Match_data.h"
#include "Packet_Value_Statistic.h"
#include "Packet_Circulation_Statistic.h"
#include "Packet_Emblem_Create_Statistic.h"

void StatisticManager::ResetDisjointAvatarInfoTotal()
{
    m_disjoint.clear();
}
void* StatisticManager::getCubeStatisticObject()
{
    return &m_cube;
}
STCubeStatisticKey::STCubeStatisticKey()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldc = 0;
}
STCubeStatisticKey::STCubeStatisticKey(const STCubeStatisticKey& other)
{
    m_field0 = other.m_field0;
    m_field4 = other.m_field4;
    m_field8 = other.m_field8;
    m_fieldc = other.m_fieldc;
}
STCubeStatisticKey::~STCubeStatisticKey() throw()
{
}
StatisticManager::StatisticManager()
    : m_flag(0), m_field34(0)
{
    for (int i = 0; i < 8; i++)
    {
        m_modules[i].Reset();
    }
    m_dungeonLag.clear();
}
StatisticManager::~StatisticManager()
{
    ResetPartyMap();
    ResetPartyJobMap();
    ResetPartyCharacMap();
    ResetDeathTowerValueMap();
    ResetDeathTowerPlayDataJobMap();
    ResetDeathTowerPlayDataPartyMap();
    ResetPacketOverflowMap();
    ResetHellPartyStatisticItemMap();
    ResetReasonCrashDownInfoMap();
    for (int i = 0; i < 8; i++)
    {
        m_modules[i].Reset();
    }
    m_dungeonLag.clear();
}
void StatisticManager::ResetPartyMap()
{
    m_party.clear();
}
void StatisticManager::ResetPartyJobMap()
{
    m_partyJob.clear();
}
void StatisticManager::ResetPartyCharacMap()
{
    m_partyCharac.clear();
}
void StatisticManager::ResetDeathTowerValueMap()
{
    m_deathTowerValue.clear();
}
void StatisticManager::ResetDeathTowerPlayDataJobMap()
{
    m_deathTowerJob.clear();
}
void StatisticManager::ResetDeathTowerPlayDataPartyMap()
{
    m_deathTowerParty.clear();
}
void StatisticManager::ResetPacketOverflowMap()
{
    m_packetOverflow.clear();
}
void StatisticManager::ResetHellPartyStatisticItemMap()
{
    m_hellParty.clear();
}
void StatisticManager::WriteDeathTowerValueStatistic(Packet_DeathTower_Statistic_Value* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        char m_f0a;
        unsigned short m_f0b;
        unsigned int m_f0d;
        int m_f11;
    };
    STDeathTowerValueStatisticKey key;
    key.m_field0 = ((Wire*)pkt)->m_f0a;
    key.m_field2 = ((Wire*)pkt)->m_f0b;
    key.m_field4 = ((Wire*)pkt)->m_f0d;
    ValueStatistic value;
    value.m_data[0] = ((Wire*)pkt)->m_f11;
    std::map<STDeathTowerValueStatisticKey, ValueStatistic>::iterator it =
        m_deathTowerValue.find(key);
    if (m_deathTowerValue.empty() || it == m_deathTowerValue.end())
    {
        m_deathTowerValue.insert(std::make_pair(key, value));
    }
    else
    {
        ValueStatistic* v = &it->second;
        *v += value;
    }
}
void StatisticManager::WriteDeathTowerPlayDataJobStatistic(
    Packet_DeathTower_Statistic_Playdata_Job* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        char m_f0a;
        unsigned short m_f0b;
        unsigned int m_f0d;
        char m_f11;
        int m_f12;
    };
    STDeathTowerPlayDataJobStatisticKey key;
    key.m_field0 = ((Wire*)pkt)->m_f0a;
    key.m_field2 = ((Wire*)pkt)->m_f0b;
    key.m_field8 = ((Wire*)pkt)->m_f11;
    key.m_field4 = ((Wire*)pkt)->m_f0d;
    PlayDataJobStatistic value;
    value.m_data[0] = ((Wire*)pkt)->m_f12;
    std::map<STDeathTowerPlayDataJobStatisticKey, PlayDataJobStatistic>::iterator it =
        m_deathTowerJob.find(key);
    if (m_deathTowerJob.empty() || it == m_deathTowerJob.end())
    {
        m_deathTowerJob.insert(std::make_pair(key, value));
    }
    else
    {
        it->second += value;
    }
}
void StatisticManager::WriteDeathTowerPlayDataPartyStatistic(
    Packet_DeathTower_Statistic_Playdata_Party* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        char m_f0a;
        char m_f0b;
        int m_f0c;
    };
    STDeathTowerPlayDataPartyStatisticKey key;
    key.m_field0 = ((Wire*)pkt)->m_f0a;
    key.m_field1 = ((Wire*)pkt)->m_f0b;
    PlayDataPartyStatistic value;
    value.m_data[0] = ((Wire*)pkt)->m_f0c;
    std::map<STDeathTowerPlayDataPartyStatisticKey, PlayDataPartyStatistic>::iterator it =
        m_deathTowerParty.find(key);
    if (m_deathTowerParty.empty() || it == m_deathTowerParty.end())
    {
        m_deathTowerParty.insert(std::make_pair(key, value));
    }
    else
    {
        it->second += value;
    }
}
void StatisticManager::WriteDungeonPartyStatistic(Packet_Dungeon_Statistic_Party* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xc];
        int m_field4;             // +0xc
        char m_field8;            // +0x10
        unsigned char m_field9;   // +0x11
        unsigned char m_fielda;   // +0x12
        unsigned char m_fieldb;   // +0x13
        unsigned char m_fieldc;   // +0x14
        char m_fieldd;            // +0x15
        int m_data[10];           // +0x16
        short m_last;             // +0x3e
    };
    STPartyStatisticKey key;
    key.m_field0 = 0;
    key.m_field4 = ((Wire*)pkt)->m_field4;
    key.m_field8 = ((Wire*)pkt)->m_field8;
    key.m_field9 = ((Wire*)pkt)->m_field9;
    key.m_fielda = ((Wire*)pkt)->m_fielda;
    key.m_fieldb = ((Wire*)pkt)->m_fieldb;
    key.m_fieldc = ((Wire*)pkt)->m_fieldc;
    key.m_fieldd = ((Wire*)pkt)->m_fieldd;
    PartyStatistic value;
    value.m_data[0] = ((Wire*)pkt)->m_data[0];
    value.m_data[1] = ((Wire*)pkt)->m_data[1];
    value.m_data[2] = ((Wire*)pkt)->m_data[2];
    value.m_data[3] = ((Wire*)pkt)->m_data[3];
    value.m_data[4] = ((Wire*)pkt)->m_data[4];
    value.m_data[5] = ((Wire*)pkt)->m_data[5];
    value.m_data[6] = ((Wire*)pkt)->m_data[6];
    value.m_data[7] = ((Wire*)pkt)->m_data[7];
    value.m_data[8] = ((Wire*)pkt)->m_data[8];
    value.m_data[9] = ((Wire*)pkt)->m_data[9];
    value.m_data[11] = ((Wire*)pkt)->m_last;
    std::map<STPartyStatisticKey, PartyStatistic>::iterator it = m_party.find(key);
    if (m_party.empty() || it == m_party.end())
    {
        m_party.insert(std::make_pair(key, value));
    }
    else
    {
        it->second += value;
    }
}
void StatisticManager::WriteDungeonPartyJobStatistic(Packet_Dungeon_Statistic_Party_Job* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xc];
        unsigned int m_f0c;
        char m_f10;
        char m_f11;
        char m_f12;
        char m_f13;
        char m_f14;
        char m_f15;
        unsigned int m_f16;
        char m_f1a;
        int m_f1b;
    };
    STPartyJobStatisticKey key;
    key.m_field0 = 0;
    key.m_field4 = ((Wire*)pkt)->m_f0c;
    key.m_field8 = ((Wire*)pkt)->m_f10;
    key.m_field9 = ((Wire*)pkt)->m_f11;
    key.m_fielda = ((Wire*)pkt)->m_f12;
    key.m_fieldb = ((Wire*)pkt)->m_f13;
    key.m_fieldc = ((Wire*)pkt)->m_f14;
    key.m_fieldd = ((Wire*)pkt)->m_f15;
    key.m_field10 = (STPartyJobStatisticKey::STPartyJobKeyField10)((Wire*)pkt)->m_f16;
    key.m_field14 = ((Wire*)pkt)->m_f1a;
    PartyJobStatistic value;
    value.m_data[1] = ((Wire*)pkt)->m_f1b;
    std::map<STPartyJobStatisticKey, PartyJobStatistic>::iterator it = m_partyJob.find(key);
    if (m_partyJob.empty() || it == m_partyJob.end())
    {
        m_partyJob.insert(std::make_pair(key, value));
    }
    else
    {
        it->second += value;
    }
}
void StatisticManager::WriteDungeonPartyCharacStatistic(Packet_Dungeon_Statistic_Party_Charac* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xc];
        unsigned int m_f0c;
        char m_f10;
        char m_f11;
        char m_f12;
        unsigned int m_f13;
        char m_f17;
        char m_f18;
        int m_f19;
        int m_f1d;
        int m_f21;
        int m_f25;
        int m_f29;
        int m_f2d;
        int m_f31;
        int m_f35;
        int m_f39;
        int m_f3d;
        short m_f41;
        int m_f43;
    };
    STPartyCharacKey key;
    key.m_field0 = 0;
    key.m_field4 = ((Wire*)pkt)->m_f0c;
    key.m_field8 = ((Wire*)pkt)->m_f10;
    key.m_field9 = ((Wire*)pkt)->m_f11;
    key.m_fielda = ((Wire*)pkt)->m_f12;
    key.m_fieldc = (STPartyCharacKey::STPartyCharacKeyFieldC)((Wire*)pkt)->m_f13;
    key.m_field10 = ((Wire*)pkt)->m_f17;
    key.m_field11 = ((Wire*)pkt)->m_f18;
    PartyCharacStatistic value;
    value.m_data[0] = ((Wire*)pkt)->m_f19;
    value.m_data[1] = ((Wire*)pkt)->m_f1d;
    value.m_data[2] = ((Wire*)pkt)->m_f21;
    value.m_data[3] = ((Wire*)pkt)->m_f25;
    value.m_data[4] = ((Wire*)pkt)->m_f29;
    value.m_data[5] = ((Wire*)pkt)->m_f2d;
    value.m_data[6] = ((Wire*)pkt)->m_f31;
    value.m_data[7] = ((Wire*)pkt)->m_f35;
    value.m_data[8] = ((Wire*)pkt)->m_f39;
    value.m_data[9] = ((Wire*)pkt)->m_f3d;
    value.m_data[10] = (int)((Wire*)pkt)->m_f41;
    value.m_data[12] = ((Wire*)pkt)->m_f43;
    std::map<STPartyCharacKey, PartyCharacStatistic>::iterator it = m_partyCharac.find(key);
    if (m_partyCharac.empty() || it == m_partyCharac.end())
    {
        m_partyCharac.insert(std::make_pair(key, value));
    }
    else
    {
        it->second += value;
    }
}
void StatisticManager::WritePacketOverflowStatistic(Packet_Overflow_Statistic_Add* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        char m_f0a;
        unsigned short m_f0b;
    };
    STPacketOverflowKey key;
    key.m_field0 = ((Wire*)pkt)->m_f0a;
    key.m_field2 = ((Wire*)pkt)->m_f0b;
    std::map<STPacketOverflowKey, int>::iterator it = m_packetOverflow.find(key);
    if (m_packetOverflow.empty() || it == m_packetOverflow.end())
    {
        m_packetOverflow.insert(std::make_pair(key, 1));
    }
    else
    {
        int* v = &it->second;
        *v += 1;
    }
}
void StatisticManager::SendDBPartyStatistic(CServerHandler* handler)
{
    Packet_DBMW_Dungeon_Statistic_Party pkt;
    int idx = 0;
    if (!m_party.empty())
    {
        for (std::map<STPartyStatisticKey, PartyStatistic>::iterator it = m_party.begin();
             it != m_party.end(); ++it)
        {
            pkt.m_elem[idx].m_field0 = it->first.m_field0;
            pkt.m_elem[idx].m_field4 = it->first.m_field4;
            pkt.m_elem[idx].m_field8 = it->first.m_field8;
            pkt.m_elem[idx].m_field9 = it->first.m_field9;
            pkt.m_elem[idx].m_fielda = it->first.m_fielda;
            pkt.m_elem[idx].m_fieldb = it->first.m_fieldb;
            pkt.m_elem[idx].m_fieldc = it->first.m_fieldc;
            pkt.m_elem[idx].m_fieldd = it->first.m_fieldd;
            pkt.m_elem[idx].m_data[0] = it->second.m_data[0];
            pkt.m_elem[idx].m_data[1] = it->second.m_data[1];
            pkt.m_elem[idx].m_data[2] = it->second.m_data[2];
            pkt.m_elem[idx].m_data[3] = it->second.m_data[3];
            pkt.m_elem[idx].m_data[4] = it->second.m_data[4];
            pkt.m_elem[idx].m_data[5] = it->second.m_data[5];
            pkt.m_elem[idx].m_data[6] = it->second.m_data[6];
            pkt.m_elem[idx].m_data[7] = it->second.m_data[7];
            pkt.m_elem[idx].m_data[8] = it->second.m_data[8];
            pkt.m_elem[idx].m_data[9] = it->second.m_data[9];
            pkt.m_elem[idx].m_data[10] = it->second.m_data[10];
            pkt.m_elem[idx].m_data[11] = it->second.m_data[11];
            idx++;
            if (99 < idx)
            {
                pkt.m_count = 100;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x183, "./log/statistic", "Packet_DBMW_Dungeon_Statistic_Party : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            pkt.m_count = idx;
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x18d, "./log/statistic", "Packet_DBMW_Dungeon_Statistic_Party : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
        }
    }
}
void StatisticManager::SendDBPartyJobStatistic(CServerHandler* handler)
{
    Packet_DBMW_Dungeon_Statistic_Party_Job pkt;
    int idx = 0;
    if (!m_partyJob.empty())
    {
        for (std::map<STPartyJobStatisticKey, PartyJobStatistic>::iterator it = m_partyJob.begin();
             it != m_partyJob.end(); ++it)
        {
            pkt.m_items[idx].m_field0 = it->first.m_field0;
            pkt.m_items[idx].m_field4 = it->first.m_field4;
            pkt.m_items[idx].m_field8 = it->first.m_field8;
            pkt.m_items[idx].m_field9 = it->first.m_field9;
            pkt.m_items[idx].m_fielda = it->first.m_fielda;
            pkt.m_items[idx].m_fieldb = it->first.m_fieldb;
            pkt.m_items[idx].m_fieldc = it->first.m_fieldc;
            pkt.m_items[idx].m_fieldd = it->first.m_fieldd;
            pkt.m_items[idx].m_field10 = it->first.m_field10;
            pkt.m_items[idx].m_field14 = it->first.m_field14;
            pkt.m_items[idx].m_data[0] = it->second.m_data[0];
            pkt.m_items[idx].m_data[1] = it->second.m_data[1];
            idx++;
            if (0xf2 < idx)
            {
                pkt.m_count = 0xf3;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x1b3, "./log/statistic", "Packet_DBMW_Dungeon_Statistic_Party_Job : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            pkt.m_count = idx;
            DNF_LOG_SCOPE_LINE(0x1bd, "./log/statistic", "Packet_DBMW_Dungeon_Statistic_Party_Job : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
}
void StatisticManager::SendDBPartyCharacStatistic(CServerHandler* handler)
{
    Packet_DBMW_Dungeon_Statistic_Party_Charac pkt;
    int idx = 0;
    if (!m_partyCharac.empty())
    {
        for (std::map<STPartyCharacKey, PartyCharacStatistic>::iterator it = m_partyCharac.begin();
             it != m_partyCharac.end(); ++it)
        {
            pkt.m_items[idx].m_field0 = it->first.m_field0;
            pkt.m_items[idx].m_field4 = it->first.m_field4;
            pkt.m_items[idx].m_field8 = it->first.m_field8;
            pkt.m_items[idx].m_field9 = it->first.m_field9;
            pkt.m_items[idx].m_fielda = it->first.m_fielda;
            pkt.m_items[idx].m_fieldc = it->first.m_fieldc;
            pkt.m_items[idx].m_field10 = it->first.m_field10;
            pkt.m_items[idx].m_field11 = it->first.m_field11;
            pkt.m_items[idx].m_data[0] = it->second.m_data[0];
            pkt.m_items[idx].m_data[1] = it->second.m_data[1];
            pkt.m_items[idx].m_data[2] = it->second.m_data[2];
            pkt.m_items[idx].m_data[3] = it->second.m_data[3];
            pkt.m_items[idx].m_data[4] = it->second.m_data[4];
            pkt.m_items[idx].m_data[5] = it->second.m_data[5];
            pkt.m_items[idx].m_data[6] = it->second.m_data[6];
            pkt.m_items[idx].m_data[7] = it->second.m_data[7];
            pkt.m_items[idx].m_data[8] = it->second.m_data[8];
            pkt.m_items[idx].m_data[9] = it->second.m_data[9];
            pkt.m_items[idx].m_data[10] = it->second.m_data[10];
            pkt.m_items[idx].m_data[11] = it->second.m_data[11];
            pkt.m_items[idx].m_data[12] = it->second.m_data[12];
            idx++;
            if (0x58 < idx)
            {
                pkt.m_count = 0x59;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x1f0, "./log/statistic", "Packet_DBMW_Dungeon_Statistic_Party_Charac : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            pkt.m_count = idx;
            DNF_LOG_SCOPE_LINE(0x1fa, "./log/statistic", "Packet_DBMW_Dungeon_Statistic_Party_Charac : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
}
void StatisticManager::SendDBDeathTowerValueStatistic(CServerHandler* handler)
{
    Packet_DBMW_DeathTower_Statistic_Value pkt;
    int idx = 0;
    if (!m_deathTowerValue.empty())
    {
        for (std::map<STDeathTowerValueStatisticKey, ValueStatistic>::iterator it =
                 m_deathTowerValue.begin(); it != m_deathTowerValue.end(); ++it)
        {
            pkt.m_items[idx].m_field0 = it->first.m_field0;
            pkt.m_items[idx].m_field2 = it->first.m_field2;
            pkt.m_items[idx].m_field4 = it->first.m_field4;
            pkt.m_items[idx].m_value = it->second.m_data[0];
            idx++;
            if (0x196 < idx)
            {
                pkt.m_count = 0x197;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x217, "./log/statistic", "Packet_DBMW_DeathTower_Statistic_Value : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            pkt.m_count = idx;
            DNF_LOG_SCOPE_LINE(0x220, "./log/statistic", "Packet_DBMW_DeathTower_Statistic_Value : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
}
void StatisticManager::SendDBDeathTowerPlayDataJobStatistic(CServerHandler* handler)
{
    Packet_DBMW_DeathTower_Statistic_Playdata_Job pkt;
    int idx = 0;
    if (!m_deathTowerJob.empty())
    {
        for (std::map<STDeathTowerPlayDataJobStatisticKey, PlayDataJobStatistic>::iterator it =
                 m_deathTowerJob.begin(); it != m_deathTowerJob.end(); ++it)
        {
            pkt.m_items[idx].m_field0 = it->first.m_field0;
            pkt.m_items[idx].m_field2 = it->first.m_field2;
            pkt.m_items[idx].m_field4 = it->first.m_field4;
            pkt.m_items[idx].m_field8 = it->first.m_field8;
            if (it->second.m_data[1] == 0)
            {
                it->second.m_data[1] = 1;
            }
            int count = it->second.m_data[1];
            int avg = it->second.m_data[0] / count;
            pkt.m_items[idx].m_avg = avg;
            pkt.m_items[idx].m_count = count;
            if (it->first.m_field2 == 0)
            {
                DNF_LOG_SCOPE_LINE(0x23d, "./log/statistic",
                    "SendDBDeathTowerPlayDataJobStatistic : 0 level error!! deathTower_type (%d) level (%d) job_ (%d) grow_type_ (%d) / updateCount (%d) clearStage (%d)\n",
                    (int)it->first.m_field0, (int)it->first.m_field2,
                    (int)it->first.m_field4, (int)it->first.m_field8,
                    count, avg);
            }
            else
            {
                idx++;
                if (0x17d < idx)
                {
                    pkt.m_count = 0x17e;
                    handler->SendToDB((PacketHeader*)&pkt);
                    DNF_LOG_SCOPE_LINE(0x24e, "./log/statistic", "DeathTowerPlayDataJob DB Sent %d", idx);
                    idx = 0;
                }
            }
        }
        if (idx != 0)
        {
            pkt.m_count = idx;
            DNF_LOG_SCOPE_LINE(0x257, "./log/statistic", "DeathTowerPlayDataJob DB Sent %d", idx);
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
}
void StatisticManager::SendDBDeathTowerPlayDataPartyStatistic(CServerHandler* handler)
{
    Packet_DBMW_DeathTower_Statistic_Playdata_Party pkt;
    int idx = 0;
    if (!m_deathTowerParty.empty())
    {
        for (std::map<STDeathTowerPlayDataPartyStatisticKey, PlayDataPartyStatistic>::iterator it =
                 m_deathTowerParty.begin(); it != m_deathTowerParty.end(); ++it)
        {
            pkt.m_items[idx].m_field0 = it->first.m_field0;
            pkt.m_items[idx].m_field1 = it->first.m_field1;
            if (it->second.m_data[1] == 0)
            {
                it->second.m_data[1] = 1;
            }
            pkt.m_items[idx].m_value = it->second.m_data[0] / it->second.m_data[1];
            idx++;
            if (0x263 < idx)
            {
                pkt.m_count = 0x264;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x276, "./log/statistic", "Packet_DBMW_DeathTower_Statistic_Playdata_Party : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            pkt.m_count = idx;
            DNF_LOG_SCOPE_LINE(0x27f, "./log/statistic", "Packet_DBMW_DeathTower_Statistic_Playdata_Party : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
}
void StatisticManager::SendDBPacketOverflowStatistic(CServerHandler* handler)
{
    Packet_DBMW_Packet_Overflow_Statistic pkt;
    int count = 0;
    if (!m_packetOverflow.empty())
    {
        for (std::map<STPacketOverflowKey, int>::iterator it = m_packetOverflow.begin();
             it != m_packetOverflow.end(); ++it)
        {
            pkt.m_typed.m_field0 = it->first.m_field0;
            pkt.m_typed.m_field2 = it->first.m_field2;
            pkt.m_typed.m_field4 = it->second;
            handler->SendToDB((PacketHeader*)&pkt);
            count++;
        }
        DNF_LOG_SCOPE_LINE(0x297, "./log/Statistic", "Packet_DBMW_Packet_Overflow_Statistic : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb", count);
    }
}
void StatisticManager::AMDecrypt(void* data, unsigned int len)
{
    for (unsigned int i = 0; i < len; i++)
    {
        ((unsigned char*)data)[i] ^= 0x9d;
        // ORIG 0x8074f24 实测：先求值 (x >> 6) 再 (x << 2)（sar 在前、shl 在后）。
        ((unsigned char*)data)[i] = ((unsigned char*)data)[i] >> 6 |
                                    ((unsigned char*)data)[i] << 2;
    }
}
void StatisticManager::WriteAssertManagerStatistic(Packet_Assert_Manager_Info* pkt)
{
    if (pkt == 0)
    {
        return;
    }
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        unsigned int m_f0a;
        char m_pad[0x100];
        unsigned short m_f10e;
        int m_f110;
    };
    STAssertManagerKey key;
    memcpy(key.m_str0, (char*)pkt + 0xe, ((Wire*)pkt)->m_f0a);
    AMDecrypt(key.m_str0, ((Wire*)pkt)->m_f0a);
    key.m_field100 = ((Wire*)pkt)->m_f10e;
    if (-1 < ((Wire*)pkt)->m_f110 && ((Wire*)pkt)->m_f110 < 0x101)
    {
        memcpy(key.m_str2, (char*)pkt + 0x114, ((Wire*)pkt)->m_f110);
        AMDecrypt(key.m_str2, ((Wire*)pkt)->m_f110);
        DNF_LOG_SCOPE_LINE(0x2b5, "./log/Statistic", "[AssertManager] Reason(%s)", key.m_str2);
    }
    if (99 < m_assertManager.size())
    {
        DNF_LOG_SCOPE_LINE(0x2ba, "./log/Statistic", "[AssertManager] Map 100 Over!!!");
        return;
    }
    std::map<STAssertManagerKey, int>::iterator it = m_assertManager.find(key);
    if (m_assertManager.empty() || it == m_assertManager.end())
    {
        m_assertManager.insert(std::make_pair(key, 1));
    }
    else
    {
        it->second += 1;
    }
}
void StatisticManager::SendDBAssertManagerStatistic(CServerHandler* handler)
{
    Packet_DBMW_Assert_Manager_Info_Write_Query pkt;
    int idx = 0;
    if (!m_assertManager.empty())
    {
        for (std::map<STAssertManagerKey, int>::iterator it = m_assertManager.begin();
             it != m_assertManager.end(); ++it)
        {
            STAssertManagerWriteItem* slot = &pkt.m_items[idx];
            memcpy(slot->m_str0, it->first.m_str0, 0x100);
            slot->m_field100 = it->first.m_field100;
            slot->m_count = it->second;
            memcpy(slot->m_str2, it->first.m_str2, 0x100);
            idx++;
            if (8 < idx)
            {
                pkt.m_count = 9;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x2e1, "./log/Statistic", "Packet_DBMW_Assert_Manager_Info_Write_Query : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            pkt.m_count = idx;
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x2eb, "./log/Statistic", "Packet_DBMW_Assert_Manager_Info_Write_Query : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb", idx);
        }
    }
}
void StatisticManager::ResetAssertManagerMap()
{
    m_assertManager.clear();
}
void StatisticManager::WriteUserTingTImeCheckStatistic(
    Packet_User_Ting_TimeCheck_Statistic_Add* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        int m_f0a;
    };
    STUserTingTimeCheckKey key;
    key.m_field0 = ((Wire*)pkt)->m_f0a / 0x3c;
    if (0x59f < (int)key.m_field0)
    {
        DNF_LOG_SCOPE_LINE(0x2fd, "./log/Statistic", "[User Ting Wrong] %d Sec", ((Wire*)pkt)->m_f0a);
        return;
    }
    std::map<STUserTingTimeCheckKey, int>::iterator it = m_userTing.find(key);
    if (m_userTing.empty() || it == m_userTing.end())
    {
        m_userTing.insert(std::make_pair(key, 1));
    }
    else
    {
        it->second += 1;
    }
    if ((int)key.m_field0 < 0xb && 0 < (int)key.m_field0)
    {
        if (m_field110.size() <= 1000)
        {
            unsigned int& uKey = *(unsigned int*)((char*)pkt + 0xe);
            std::map<unsigned int, int>::iterator it2 = m_field110.find(uKey);
            if (m_field110.empty() || it2 == m_field110.end())
            {
                m_field110.insert(std::make_pair(uKey, key.m_field0));
            }
        }
    }
}
void StatisticManager::SendDBTingUserTimeCheck(CServerHandler* handler)
{
    Packet_DBMW_Ting_User_TimeCheck_Write_Query pkt;
    int idx = 0;
    if (!m_field110.empty())
    {
        for (std::map<unsigned int, int>::iterator it = m_field110.begin();
             it != m_field110.end(); ++it)
        {
            pkt.m_typed.m_items[idx].m_field0 = it->first;
            pkt.m_typed.m_items[idx].m_field4 = it->second;
            idx++;
            bool over = (idx > 0x2fd);
            if (over)
            {
                pkt.m_typed.m_count = 0x2fe;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x327, "./log/Statistic", "Packet_DBMW_Ting_User_TimeCheck_Write_Query : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            pkt.m_typed.m_count = idx;
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x331, "./log/Statistic", "Packet_DBMW_Ting_User_TimeCheck_Write_Query : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb", idx);
        }
    }
}
void StatisticManager::ResetTingUserTimeCheckMap()
{
    m_field110.clear();
}
void StatisticManager::SendDBUserTingTimeCheckStatistic(CServerHandler* handler)
{
    Packet_DBMW_User_Ting_TimeCheck_Write_Query pkt;
    int idx = 0;
    if (!m_userTing.empty())
    {
        for (std::map<STUserTingTimeCheckKey, int>::iterator it = m_userTing.begin();
             it != m_userTing.end(); ++it)
        {
            pkt.m_items[idx].m_field0 = it->first.m_field0;
            pkt.m_items[idx].m_field4 = it->second;
            idx++;
            if (0x2fd < idx)
            {
                pkt.m_count = 0x2fe;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x34c, "./log/Statistic", "Packet_DBMW_User_Ting_TimeCheck_Write_Query : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            pkt.m_count = idx;
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x356, "./log/Statistic", "Packet_DBMW_User_Ting_TimeCheck_Write_Query : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb", idx);
        }
    }
}
void StatisticManager::ResetUserTIngTimeCheckMap()
{
    m_userTing.clear();
}
void StatisticManager::WriteHellPartyStatisticItem(Packet_HellParty_Statistic_Item* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        char m_f0a;
        unsigned int m_f0b;
        char m_f0f;
        char m_f10;
        char m_f11;
    };
    STHellPartyStatisticItemKey key;
    key.m_field0 = ((Wire*)pkt)->m_f0a;
    key.m_field4 = ((Wire*)pkt)->m_f0b;
    key.m_field8 = ((Wire*)pkt)->m_f0f;
    key.m_field9 = ((Wire*)pkt)->m_f10;
    key.m_fielda = ((Wire*)pkt)->m_f11;
    HellPartyItenmData value;
    memcpy(value.m_data, (char*)pkt + 0x12, 0x18);
    std::map<STHellPartyStatisticItemKey, HellPartyItenmData>::iterator it =
        m_hellParty.find(key);
    if (m_hellParty.empty() || it == m_hellParty.end())
    {
        m_hellParty.insert(std::make_pair(key, value));
    }
    else
    {
        it->second += value;
    }
}
void StatisticManager::SendDBHellPartyStatisticItem(CServerHandler* handler)
{
    Packet_DBMW_HellParty_Statistic_Item pkt;
    int idx = 0;
    if (!m_hellParty.empty())
    {
        for (std::map<STHellPartyStatisticItemKey, HellPartyItenmData>::iterator it =
                 m_hellParty.begin(); it != m_hellParty.end(); ++it)
        {
            *((char*)&pkt + 0xe + idx * 0x24 + 0) = it->first.m_field0;
            *(unsigned int*)((char*)&pkt + 0xe + idx * 0x24 + 1) = it->first.m_field4;
            *((char*)&pkt + 0xe + idx * 0x24 + 5) = it->first.m_field8;
            *((char*)&pkt + 0xe + idx * 0x24 + 6) = it->first.m_field9;
            *((char*)&pkt + 0xe + idx * 0x24 + 7) = it->first.m_fielda;
            *(int*)((char*)&pkt + 0xe + idx * 0x24 + 8) = it->second.m_count;
            for (int k = 0; k < 6; k++)
            {
                *(int*)((char*)&pkt + 0xe + idx * 0x24 + 0xc + k * 4) = it->second.m_data[k];
            }
            idx++;
            if (0xa7 < idx)
            {
                *(unsigned int*)((char*)&pkt + 0xa) = 0xa8;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x391, "./log/statistic", "Packet_DBMW_HellParty_Statistic_Item : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            *(unsigned int*)((char*)&pkt + 0xa) = idx;
            DNF_LOG_SCOPE_LINE(0x39a, "./log/statistic", "Packet_DBMW_HellParty_Statistic_Item : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
}
void StatisticManager::AddLoadingTimeReportStatistics(Packet_Loading_Time_Report_Statistics* pkt)
{
    for (int i = 0; i <= 8; i++)
    {
        if (*(int*)((char*)pkt + i * 4 + 10) != 0)
        {
            m_loading.m_data[i] += *(int*)((char*)pkt + i * 4 + 10);
            m_loading.m_data2[i] += 1;
        }
        if (i == 7)
        {
            unsigned short lcount = *(unsigned short*)((char*)pkt + 0x32);
            if (lcount != 0)
            {
                STPowerwarFightLoadingKey lkey;
                lkey.m_field0 = *(unsigned int*)((char*)pkt + 0x2e);
                for (int j = 0; j < (int)lcount; j++)
                {
                    lkey.m_field4 = *(unsigned short*)((char*)pkt + j * 9 + 0x34);
                    if (m_pwLoading.size() < 0x3e9)
                    {
                        std::map<STPowerwarFightLoadingKey, STPowerwarFightLoadingData>::iterator it =
                            m_pwLoading.find(lkey);
                        bool isNew = (m_pwLoading.empty() || it == m_pwLoading.end());
                        if (isNew)
                        {
                            STPowerwarFightLoadingData v;
                            v.m_field0 = *(char*)((char*)pkt + j * 9 + 0x36);
                            v.m_field2 = *(unsigned short*)((char*)pkt + j * 9 + 0x37);
                            v.m_field4 = *(unsigned short*)((char*)pkt + j * 9 + 0x39);
                            v.m_field6 = *(unsigned short*)((char*)pkt + j * 9 + 0x3b);
                            m_pwLoading.insert(std::make_pair(lkey, v));
                        }
                    }
                }
            }
        }
        if (i == 8)
        {
            unsigned short gcount = *(unsigned short*)((char*)pkt + 0x3b8);
            if (gcount != 0)
            {
                STPowerwarFightLagKey gkey;
                gkey.m_field0 = *(unsigned int*)((char*)pkt + 0x2e);
                for (int j = 0; j < (int)gcount; j++)
                {
                    gkey.m_field4 = *(unsigned short*)((char*)pkt + j * 0xb + 0x3ba);
                    if (m_pwLag.size() < 0x3e9)
                    {
                        std::map<STPowerwarFightLagKey, STPowerwarFightLagData>::iterator it =
                            m_pwLag.find(gkey);
                        bool isNew = (m_pwLag.empty() || it == m_pwLag.end());
                        if (isNew)
                        {
                            STPowerwarFightLagData v;
                            v.m_field0 = *(unsigned char*)((char*)pkt + j * 0xb + 0x3bc);
                            v.m_field4 = *(unsigned int*)((char*)pkt + j * 0xb + 0x3bd);
                            v.m_field8 = *(unsigned int*)((char*)pkt + j * 0xb + 0x3c1);
                            m_pwLag.insert(std::make_pair(gkey, v));
                        }
                    }
                }
            }
        }
    }
}
void StatisticManager::SendDBLoadingTimeReport(CServerHandler* handler)
{
    Packet_DBMW_Loading_Time_Report pkt;
    for (int i = 0; i < 9; i++)
    {
        pkt.m_group[i] = handler->GetServerGroupNo();
        if (m_loading.m_data2[i] == 0)
        {
            pkt.m_value[i] = 0;
        }
        else
        {
            pkt.m_value[i] =
                (unsigned int)m_loading.m_data[i] / (unsigned int)m_loading.m_data2[i];
        }
    }
    handler->SendToDB((PacketHeader*)&pkt);
    m_loading.Reset();
}
void StatisticManager::SendDBPowerwarLoadingTimeReport(CServerHandler* handler)
{
    Packet_DBMW_Powerwar_Loading_Time_Report pkt;
    int idx = 0;
    if (!m_pwLoading.empty())
    {
        for (std::map<STPowerwarFightLoadingKey, STPowerwarFightLoadingData>::iterator it =
                 m_pwLoading.begin(); it != m_pwLoading.end(); ++it)
        {
            snprintf(pkt.m_sql[idx], 0x100,
                "inSert into powerwar_loading (m_id,occ_time,round,player,my_loading,other_loading,vs_loading) values (%s,now(),%d,%d,%d,%d,%d)",
                NumberToString(it->first.m_field0, 0),
                it->first.m_field4,
                it->second.m_field0,
                it->second.m_field2,
                it->second.m_field4,
                it->second.m_field6);
            if (++idx > 0x16u)
            {
                pkt.m_count = 0x17;
                handler->SendToDB((PacketHeader*)&pkt);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            pkt.m_count = idx;
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
    m_pwLoading.clear();
}
void StatisticManager::SendDBPowerwarLagReport(CServerHandler* handler)
{
    Packet_DBMW_Powerwar_Lag_Report pkt;
    int idx = 0;
    if (!m_pwLag.empty())
    {
        for (std::map<STPowerwarFightLagKey, STPowerwarFightLagData>::iterator it =
                 m_pwLag.begin(); it != m_pwLag.end(); ++it)
        {
            float* p4 = (float*)&it->second.m_field4;
            float* p8 = (float*)&it->second.m_field8;
            float f4 = *p4;
            float f8 = *p8;
            snprintf(pkt.m_sql[idx], 0x100,
                "inSert into powerwar_lag (m_id,occ_time,round,player,lag_avg,lag_cnt) values (%s,now(),%d,%d,%.2f,%.2f)",
                NumberToString(it->first.m_field0, 0),
                it->first.m_field4,
                it->second.m_field0,
                f4,
                f8);
            if (++idx > 0x16u)
            {
                pkt.m_count = 0x17;
                handler->SendToDB((PacketHeader*)&pkt);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            pkt.m_count = idx;
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
    m_pwLag.clear();
}
void StatisticManager::DBSaveProcess(CServerHandler* handler)
{
    time_t now;
    tm t;
    char ts[20];
    char ts2[20];
    now = time(0);
    t = *localtime(&now);
    printf("---Time : %d, %d ----\n", t.tm_hour, t.tm_min);
    snprintf(ts, 0x13, "%d-%d-%d %d:%d:0", t.tm_year + 0x76c, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min);
    SendDBP2PStatistic(handler);
    ResetP2PStatistic();
    if (t.tm_hour == 5 && t.tm_min == 0)
    {
        SendDBHellPartyStatisticItem(handler);
        ResetHellPartyStatisticItemMap();
    }
    if (t.tm_hour == 5 && t.tm_min == 0)
    {
        SendDBFatigueBattery(handler);
        ResetFatigueBattery();
    }
    if (t.tm_hour == 5 && t.tm_min == 0)
    {
        SendDBDisjointAvatarInfoTotal(handler);
        ResetDisjointAvatarInfoTotal();
    }
    if (t.tm_hour == 5 && t.tm_min == 0)
    {
        SendDBCreateEmblemInfo(handler);
        ResetCreateEmblemInfo();
    }
    if (t.tm_hour == 5 && t.tm_min == 0)
    {
        SendDBRandomboxStatistic(handler);
        ResetRandomboxStatistic();
    }
    SendDBTingUserTimeCheck(handler);
    ResetTingUserTimeCheckMap();
    SendDBPowerwarLoadingTimeReport(handler);
    SendDBPowerwarLagReport(handler);
    if (t.tm_min % 10 == 0)
    {
        SendDBServerMatchData(handler);
        ResetServerMatchData();
    }
    if (t.tm_min == 0 || t.tm_min == 0x1e)
    {
        snprintf(ts2, 0x13, "%d-%d-%d %d:%d:0", t.tm_year + 0x76c, t.tm_mon + 1, t.tm_mday, t.tm_hour, t.tm_min);
        SendDBPacketOverflowStatistic(handler);
        ResetPacketOverflowMap();
        SendDBAssertManagerStatistic(handler);
        ResetAssertManagerMap();
        SendDBUserTingTimeCheckStatistic(handler);
        ResetUserTIngTimeCheckMap();
        SendDBLagStatistics(handler, ts2);
        statistc_proxy::sendDBStatisticProxy();
        statistc_proxy::resetStatisticProxy();
    }
    if (t.tm_hour % 3 == 0 && t.tm_min == 0)
    {
        SendDBLoadingTimeReport(handler);
    }
    if (t.tm_hour == 5)
    {
        if (t.tm_min == 10)
        {
            SendDBPartyStatistic(handler);
            ResetPartyMap();
        }
        if (t.tm_min == 0xf)
        {
            SendDBPartyJobStatistic(handler);
            ResetPartyJobMap();
        }
        if (t.tm_min == 0x14)
        {
            SendDBPartyCharacStatistic(handler);
            ResetPartyCharacMap();
        }
        if (t.tm_min == 0x19)
        {
            SendDBDeathTowerValueStatistic(handler);
            ResetDeathTowerValueMap();
        }
        if (t.tm_min == 0x1e)
        {
            SendDBDeathTowerPlayDataJobStatistic(handler);
            ResetDeathTowerPlayDataJobMap();
        }
        if (t.tm_min == 0x23)
        {
            SendDBDeathTowerPlayDataPartyStatistic(handler);
            ResetDeathTowerPlayDataPartyMap();
        }
        if (t.tm_min == 0x28)
        {
            SendDBBloodDungeonStatistic(handler);
            ResetBloodDungeon();
        }
        if (t.tm_min == 0x2d)
        {
            SendDBValueStatistic(handler);
            ResetValueStatistic();
        }
        if (t.tm_min == 0x32)
        {
            SendDBCirculationStatistic(handler);
            ResetCirculationStatistic();
        }
    }
    if (t.tm_hour == 6 && t.tm_min == 0)
    {
        SendDBSecretShopStatistic(handler);
        ResetSecretShopStatistic();
    }
    if (t.tm_hour == 6 && t.tm_min == 0)
    {
        SendDBTowerOfDespairStatistic(handler);
        ResetTowerOfDespair();
    }
    if (t.tm_hour == 5 && t.tm_min == 0)
    {
        SendDBGoldcardEventStatistic(handler);
        ResetGoldcardEventStatistic();
    }
    if (t.tm_hour == 6)
    {
        ((CCubeStatistic*)getCubeStatisticObject())->sendStatisticData(handler);
        ((CCubeStatistic*)getCubeStatisticObject())->resetStatisticData();
    }
}
void StatisticManager::AddFatigueBatteryStatistics(Packet_Fatigue_Battery_Money_Statistic* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        char m_f0a;
        int m_f0b;
        unsigned short m_f0f;
    };
    STFatigueBattery value;
    value.m_field0 = ((Wire*)pkt)->m_f0b;
    value.m_field4 = ((Wire*)pkt)->m_f0f;
    std::map<unsigned char, STFatigueBattery>::iterator it =
        m_fatigue.find(*(unsigned char*)((char*)pkt + 10));
    if (it != m_fatigue.end())
    {
        it->second.m_field0 += ((Wire*)pkt)->m_f0b;
        it->second.m_field4 += ((Wire*)pkt)->m_f0f;
    }
    else
    {
        m_fatigue.insert(std::make_pair(*(unsigned char*)((char*)pkt + 10), value));
    }
}
void StatisticManager::ResetFatigueBattery()
{
    m_fatigue.clear();
}
void StatisticManager::SendDBFatigueBattery(CServerHandler* handler)
{
    if (m_fatigue.empty())
    {
        return;
    }
    Packet_DBMW_Fatigue_Battery_Money_Statistic pkt;
    for (std::map<unsigned char, STFatigueBattery>::iterator it = m_fatigue.begin();
         it != m_fatigue.end(); ++it)
    {
        pkt.m_typed.m_items[(unsigned int)it->first].m_field0 = it->second.m_field0;
        pkt.m_typed.m_items[(unsigned int)it->first].m_field4 = it->second.m_field4;
    }
    handler->SendToDB((PacketHeader*)&pkt);
}
void StatisticManager::AddBloodDungeonStatistics(Packet_Blood_dungeon_statistic* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xe];
        unsigned int m_key;
        unsigned char m_f0;
        unsigned char m_f1;
    };
    std::map<unsigned int, STBloodDungeonStatistic>::iterator it =
        m_blood.find(*(unsigned int*)((char*)pkt + 0xe));
    if (it != m_blood.end())
    {
        it->second.m_field0 += ((Wire*)pkt)->m_f0;
        it->second.m_field4 += ((Wire*)pkt)->m_f1;
    }
    else
    {
        STBloodDungeonStatistic v;
        v.m_field0 = ((Wire*)pkt)->m_f0;
        v.m_field4 = ((Wire*)pkt)->m_f1;
        m_blood.insert(std::make_pair(*(unsigned int*)((char*)pkt + 0xe), v));
    }
}
void StatisticManager::ResetBloodDungeon()
{
    m_blood.clear();
}
void StatisticManager::SendDBBloodDungeonStatistic(CServerHandler* handler)
{
    if (m_blood.empty())
    {
        return;
    }
    Packet_DBMW_Query_String pkt;
    pkt.m_queryId = 0x4ed3;
    for (std::map<unsigned int, STBloodDungeonStatistic>::iterator it = m_blood.begin();
         it != m_blood.end(); ++it)
    {
        memset(pkt.m_query, 0, sizeof(pkt.m_query));
        snprintf(pkt.m_query, 0x400,
            "inSert into log_blood_dungeon(occ_date,level,try_count,clear_count) values (now(),%d,%d,%d)",
            it->first, it->second.m_field0, it->second.m_field4);
        handler->SendToDB((PacketHeader*)&pkt);
    }
}
void StatisticManager::ResetReasonCrashDownInfoMap()
{
    m_reasonCrash.clear();
}
void StatisticManager::AddReasonCrashDownData(Packet_Reason_Crash_Down_Info* pkt,
                                              CServerHandler* handler)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        unsigned int m_f0a;
        unsigned int m_f0e;
        unsigned int m_f12;
        unsigned int m_f16;
    };
    Packet_DBMW_Reason_Crash_Down_Query query;
    memset(query.m_query, 0, sizeof(query.m_query));
    snprintf(query.m_query, 0xff,
             "inSert into log_client_ting_stat (occ_time,channel_no,reason,cnt) values (from_unixtime(%d),%d,%d,%d)",
             ((Wire*)pkt)->m_f0a, ((Wire*)pkt)->m_f0e, ((Wire*)pkt)->m_f12,
             ((Wire*)pkt)->m_f16);
    handler->SendToDB((PacketHeader*)&query);
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x5b8, "./log/ReasonCrashDown", "%s", query.m_query);
}
void StatisticManager::AddDisjointAvatarInfo(Packet_Avater_Disjoint_Statistic* pkt)
{
    struct __attribute__((packed)) Item
    {
        int m_a;
        int m_b;
        char m_c;
        int m_d;
    };
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        int m_count;
        Item m_items[3];
    };
    int count = ((Wire*)pkt)->m_count;
    if (count < 0)
    {
        return;
    }
    if (2 < count)
    {
        return;
    }
    for (int i = 0; i < count; i++)
    {
        int a = ((Wire*)pkt)->m_items[i].m_a;
        int b = ((Wire*)pkt)->m_items[i].m_b;
        int c = (int)(char)((Wire*)pkt)->m_items[i].m_c;
        int d = ((Wire*)pkt)->m_items[i].m_d;
        m_disjoint.incCount(a, b, c, d);
    }
}
void StatisticManager::SendDBDisjointAvatarInfoTotal(CServerHandler* handler)
{
    Packet_Avater_Disjoint_Statistic_DB pkt;
    memcpy(&pkt.m_info, &this->m_disjoint, 0x144);
    handler->SendToDB((PacketHeader*)&pkt);
    DNF_LOG_SCOPE_LINE(0x5fa, "./log/statistic", "Packet_Avater_Disjoint_Statistic_DB");
}
namespace
{
union __attribute__((packed)) EmblemCreateWire
{
    struct __attribute__((packed))
    {
        char m_hdr[2];
        int m_arrA[0x100];
    } a;
    struct __attribute__((packed))
    {
        char m_hdr2[0xa];
        int m_count;
        int m_arrB[0x100];
    } b;
};
struct __attribute__((packed)) LagModuleWire
{
    union
    {
        int m_int;    // +0
        float m_float;
    } m_u;
    float m_field4;  // +4
    unsigned short m_field8;  // +8（0xa 字节记录）
};
struct __attribute__((packed)) LagWire
{
    char m_hdr[0xb];
    LagModuleWire m_mods[8];   // +0xb，10 字节步长
    char m_tail[0x8d];
};
}
void StatisticManager::AddCreateEmblemInfo(Packet_Emblem_Create_Statistic* pkt)
{
    for (int i = 0; i < pkt->m_count; i++)
    {
        for (int j = 0; j < pkt->m_arrA[i]; j++)
        {
            m_createEmblem.increaseCount(pkt->m_arrB[i]);
        }
    }
}
void StatisticManager::SendDBCreateEmblemInfo(CServerHandler* handler)
{
    Packet_Emblem_Create_Statistic_DB pkt;
    memcpy(&pkt.m_info, (char*)this + 0x32c, sizeof(pkt.m_info));
    handler->SendToDB((PacketHeader*)&pkt);
}
void StatisticManager::AddRandomboxStatistic(Packet_Randombox_statistic* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        char m_f0a;
        char m_f0b;
    };
    if (4 < (char)((Wire*)pkt)->m_f0a)
    {
        return;
    }
    if ((char)((Wire*)pkt)->m_f0a < 0)
    {
        return;
    }
    if (((Wire*)pkt)->m_f0b == 0)
    {
        ++m_randomboxA[(char)((Wire*)pkt)->m_f0a];
    }
    else if (((Wire*)pkt)->m_f0b == 1)
    {
        ++m_randomboxB[(char)((Wire*)pkt)->m_f0a];
    }
}
void StatisticManager::SendDBRandomboxStatistic(CServerHandler* handler)
{
    Packet_Randombox_statistic_DB pkt;
    struct RBoxView1 { int m_pad[2]; int m_b[0xd8]; };
    struct RBoxView2 { int m_pad[3]; int m_b[0xd8]; };
    for (int i = 0; i < 5; i++)
    {
        pkt.m_typed.m_a[i] = m_randomboxA[i];
        pkt.m_typed.m_b[i] = m_randomboxB[i];
    }
    handler->SendToDB((PacketHeader*)&pkt);
}
void StatisticManager::ResetRandomboxStatistic()
{
    for (int i = 0; i < 5; i++)
    {
        m_randomboxA[i] = 0;
        m_randomboxB[i] = 0;
    }
}
void StatisticManager::AddUserCountStatistics(CServerHandler* handler,
                                              Packet_User_Count_Statistic* pkt)
{
    handler->SendToDB((PacketHeader*)pkt);
}
void StatisticManager::AddLagStatistics(Packet_Stat_Lag_Statistics* pkt)
{
    for (int i = 0; i < 8; i++)
    {
        if (((LagWire*)pkt)->m_mods[i].m_u.m_int != 0)
        {
            float avg_f = *(float*)((char*)pkt + i * 10 + 0xb);
            float dev_f = *(float*)((char*)pkt + i * 10 + 0xf);
            m_modules[i].m_data[0] += (int)(long long)avg_f;
            m_modules[i].m_data[1] += (int)(long long)dev_f;
            m_modules[i].m_data[2] +=
                (unsigned int)*(unsigned short*)((char*)pkt + i * 10 + 0x13);
            m_modules[i].m_data[3] += 1;
        }
    }
    unsigned char dcount = *(char*)((char*)pkt + 0x5b);
    if (dcount != 0 && dcount < 0xb)
    {
        for (int j = 0; j < (int)dcount; j++)
        {
            float a = *(float*)((char*)pkt + j * 0x18 + 0x5f);
            float b = *(float*)((char*)pkt + j * 0x18 + 99);
            float c = *(float*)((char*)pkt + j * 0x18 + 0x6a);
            float d = *(float*)((char*)pkt + j * 0x18 + 0x6e);
            unsigned short key = *(unsigned short*)((char*)pkt + j * 0x18 + 0x5c);
            std::map<unsigned short, STDungeonLagStatistics>::iterator it =
                m_dungeonLag.find(key);
            bool isNew = (m_dungeonLag.empty() || it == m_dungeonLag.end());
            if (isNew)
            {
                STDungeonLagStatistics v;
                v.m_data[0] = (int)(long long)a;
                v.m_data[1] = (int)(long long)b;
                v.m_data[2] = (unsigned int)*(unsigned short*)((char*)pkt + j * 0x18 + 0x67);
                v.m_data[3] = 1;
                v.m_data[4] = (int)(long long)c;
                v.m_data[5] = (int)(long long)d;
                v.m_data[6] = (unsigned int)*(unsigned short*)((char*)pkt + j * 0x18 + 0x72);
                v.m_data[7] = 1;
                m_dungeonLag.insert(std::make_pair(key, v));
            }
            else
            {
                it->second.m_data[0] += (int)(long long)a;
                it->second.m_data[1] += (int)(long long)b;
                it->second.m_data[2] +=
                    (unsigned int)*(unsigned short*)((char*)pkt + j * 0x18 + 0x67);
                it->second.m_data[3] += 1;
                it->second.m_data[4] += (int)(long long)c;
                it->second.m_data[5] += (int)(long long)d;
                it->second.m_data[6] +=
                    (unsigned int)*(unsigned short*)((char*)pkt + j * 0x18 + 0x72);
                it->second.m_data[7] += 1;
            }
        }
    }
}
void StatisticManager::SendDBLagStatistics(CServerHandler* handler, char* timeStr)
{
    Packet_DBMW_TechnicalReport_Common_Query pkt;
    for (int i = 0; i < 8; i++)
    {
        if (0 < m_modules[i].m_data[3])
        {
            memset(pkt.m_query, 0, sizeof(pkt.m_query));
            snprintf(pkt.m_query, 0x400,
                "inSert into lag_stat_module (occ_time, server_id, module, average, deviation, count) values ('%s', %d, %d, %d, %d, %d)",
                timeStr, handler->GetServerGroupNo() & 0xff, i,
                m_modules[i].m_data[2] / m_modules[i].m_data[3],
                (unsigned int)m_modules[i].m_data[1] / (unsigned int)m_modules[i].m_data[3],
                (unsigned int)m_modules[i].m_data[0] / (unsigned int)m_modules[i].m_data[3]);
            DNF_LOG_SCOPE_LINE(0x6a1, "./log/LagStatistics", "%s", pkt.m_query);
            handler->SendToDB((PacketHeader*)&pkt);
            m_modules[i].Reset();
        }
    }
    if (m_dungeonLag.empty())
    {
        return;
    }
    for (std::map<unsigned short, STDungeonLagStatistics>::iterator it = m_dungeonLag.begin();
         it != m_dungeonLag.end(); ++it)
    {
        memset(pkt.m_query, 0, sizeof(pkt.m_query));
        snprintf(pkt.m_query, 0x400,
            "inSert into lag_stat_dungeon (occ_time, server_id, dungeon_idx, first_average, first_deviation, first_count, boss_average, boss_deviation, boss_count) values ('%s', %d, %d, %d, %d, %d, %d, %d, %d)",
            timeStr, handler->GetServerGroupNo() & 0xff, it->first,
            it->second.m_data[6] / it->second.m_data[7],
            (unsigned int)it->second.m_data[5] / (unsigned int)it->second.m_data[7],
            (unsigned int)it->second.m_data[4] / (unsigned int)it->second.m_data[7],
            it->second.m_data[2] / it->second.m_data[3],
            (unsigned int)it->second.m_data[1] / (unsigned int)it->second.m_data[3],
            (unsigned int)it->second.m_data[0] / (unsigned int)it->second.m_data[3]);
        DNF_LOG_SCOPE_LINE(0x6b8, "./log/LagStatistics", "%s", pkt.m_query);
        handler->SendToDB((PacketHeader*)&pkt);
    }
    m_dungeonLag.clear();
}
void StatisticManager::AddValueStatistics(Packet_Value_Statistic* pkt)
{
    std::map<int, ValueStatisticData>::iterator it = m_value.find(*(int*)((char*)pkt + 10));
    if (it != m_value.end())
    {
        for (int i = 0; i < 0x1e; i++)
        {
            it->second.m_data[i] += pkt->m_data[i];
        }
    }
    else
    {
        ValueStatisticData v;
        for (int i = 0; i < 0x1e; i++)
        {
            v.m_data[i] = pkt->m_data[i];
        }
        m_value.insert(std::make_pair(*(int*)((char*)pkt + 10), v));
    }
}
void StatisticManager::ResetValueStatistic()
{
    m_value.clear();
}
void StatisticManager::SendDBValueStatistic(CServerHandler* handler)
{
    // ORIG：空 if + else 形态（call empty; test; jne），直接写 !empty() 会物化 xor。
    if (m_value.empty())
    {
    }
    else
    {
        Packet_DBMW_Query_String pkt;
        pkt.m_queryId = 0x4ef5;
        time_t now = time(0);
        for (std::map<int, ValueStatisticData>::iterator it = m_value.begin();
             it != m_value.end(); ++it)
        {
            int key = it->first;
            ValueStatisticData* v = &it->second;
            memset(pkt.m_query, 0, sizeof(pkt.m_query));
            snprintf(pkt.m_query, 0x400,
                "inSert into log_value_stat(channel_no,occ_time ,level,uv,drop_gold,drop_item,result_card_gold,result_card_item,gold_card_item,store_item_buy,jar_item,disjoint_create,upgrade_faild_forced_disjoint,quest_reward,deathtower_card_gold,deathtower_card_item,consume_store_item_buy,consume_upgrade_attempt,consume_upgrade_faild,consume_stamina_recovery,consume_quest_consume,consume_auction_commision,consume_item_disjoint,consume_item_repair,consume_item_use,consume_item_drop,consume_gold_drop,consume_gold_card_price,consume_qp_init) values(%d,from_unixtime(%d),%d,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u)",
                1, now, key, v->m_data[0], v->m_data[1], v->m_data[2], v->m_data[3], v->m_data[4],
                v->m_data[5], v->m_data[6], v->m_data[7], v->m_data[8], v->m_data[9],
                v->m_data[10], v->m_data[11], v->m_data[12], v->m_data[13], v->m_data[14],
                v->m_data[15], v->m_data[16], v->m_data[17], v->m_data[18], v->m_data[19],
                v->m_data[20], v->m_data[21], v->m_data[22], v->m_data[23], v->m_data[24],
                v->m_data[25], v->m_data[26], v->m_data[27], v->m_data[28]);
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
}
void StatisticManager::AddCirculationStatistics(Packet_Circulation_Statistic* pkt)
{
    std::map<int, CirculationStatisticData>::iterator it = m_circ.find(*(int*)((char*)pkt + 10));
    if (it != m_circ.end())
    {
        for (int i = 0; i < 0x30; i++)
        {
            it->second.m_data[i] += pkt->m_data[i];
        }
    }
    else
    {
        CirculationStatisticData v;
        for (int i = 0; i < 0x30; i++)
        {
            v.m_data[i] = pkt->m_data[i];
        }
        m_circ.insert(std::make_pair(*(int*)((char*)pkt + 10), v));
    }
}
void StatisticManager::ResetCirculationStatistic()
{
    m_circ.clear();
}
void StatisticManager::SendDBCirculationStatistic(CServerHandler* handler)
{
    if (m_circ.empty())
    {
        return;
    }
    Packet_DBMW_Query_String pkt;
    pkt.m_queryId = 0x4ef6;
    time_t now = time(0);
    for (std::map<int, CirculationStatisticData>::iterator it = m_circ.begin();
         it != m_circ.end(); ++it)
    {
            int key = it->first;
            CirculationStatisticData* v = &it->second;
            memset(pkt.m_query, 0, sizeof(pkt.m_query));
            snprintf(pkt.m_query, 0x1000,
                "inSert into log_gold_stat(channel_no,occ_time ,level,dungeon_drop,result_card,sell_store,quest_reward,death_tower_reward,illusion_tower_reward,war_area_drop,member_tax,blood_dungeon_reward,blood_dungeon_lotto,power_dungeon_drop,power_dungeon_result_card,buy_store,stamina_recovery,repair_item,private_store_commission,gold_card,gold_drop,upgrade,quest_use,mail_commission,punish_user,restrict_trade,guild_level_up,guild_skill,guild_mail,item_compound,blood_dungeon_enter,buy_cerashop,war_area_enter,assault_gold,upgrade_guild_agit,upgrade_guild_cargo,break_away_reward,link_charac_bonus,ultimate_dungeon_reward,guild_fund,guild_fund_dungeon,quest_shop_init_cost,unseal, lottery, amplify,roi_regen) values(%d,from_unixtime(%d),%d,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u)",
                1, now, key,
                v->m_data[0], v->m_data[1], v->m_data[2], v->m_data[3], v->m_data[4],
                v->m_data[5], v->m_data[6], v->m_data[7], v->m_data[8], v->m_data[9],
                v->m_data[10], v->m_data[11], v->m_data[12], v->m_data[13], v->m_data[14],
                v->m_data[15], v->m_data[16], v->m_data[17], v->m_data[18], v->m_data[19],
                v->m_data[20], v->m_data[21], v->m_data[22], v->m_data[23], v->m_data[24],
                v->m_data[25], v->m_data[26], v->m_data[27], v->m_data[28], v->m_data[29],
                v->m_data[30], v->m_data[31], v->m_data[32], v->m_data[33], v->m_data[34],
                v->m_data[35], v->m_data[36], v->m_data[37], v->m_data[38], v->m_data[39],
                v->m_data[40], v->m_data[41], v->m_data[42], v->m_data[43], v->m_data[44],
                v->m_data[45], v->m_data[46], v->m_data[47]);
            handler->SendToDB((PacketHeader*)&pkt);
        }
}
void StatisticManager::AddServerMatchData(Packet_Server_Match_data* pkt)
{
    m_serverMatch[0] = (int)pkt->m_fieldA;
    m_serverMatch[1] += (int)pkt->m_fieldB;
    m_serverMatch[2] += (int)pkt->m_fieldC;
}
void StatisticManager::ResetServerMatchData()
{
    m_serverMatch[0] = 0;
    m_serverMatch[1] = 0;
    m_serverMatch[2] = 0;
}
void StatisticManager::SendDBServerMatchData(CServerHandler* handler)
{
    for (;;)
    {
        if (m_serverMatch[0] != 0)
        {
            Packet_Server_Match_data_DBMW pkt;
            pkt.m_fieldA = m_serverMatch[0];
            pkt.m_fieldB = m_serverMatch[1];
            pkt.m_fieldC = m_serverMatch[2];
            handler->SendToDB((PacketHeader*)&pkt);
        }
        break;
    }
}
void StatisticManager::AddSecretShopStatistic(Packet_Secret_Shop_Statistic* pkt)
{
    if (pkt->m_count == 0)
    {
        return;
    }
    if (2 < pkt->m_shopIdx)
    {
        return;
    }
    for (int i = 0; i < pkt->m_count; i++)
    {
        std::map<int, SECRET_SHOP_STATISTIC_DATA>::iterator it =
            m_secretShop[pkt->m_shopIdx].find(pkt->m_items[i].m_data[0]);
        if (it != m_secretShop[pkt->m_shopIdx].end())
        {
            it->second.m_data[3] += pkt->m_items[i].m_data[3];
            it->second.m_data[1] += pkt->m_items[i].m_data[1];
            it->second.m_data[2] += pkt->m_items[i].m_data[2];
            it->second.m_data[4] += pkt->m_items[i].m_data[4];
        }
        else
        {
            m_secretShop[pkt->m_shopIdx].insert(
                std::pair<int, SECRET_SHOP_STATISTIC_DATA>(
                    pkt->m_items[i].m_data[0], pkt->m_items[i]));
        }
    }
}
void StatisticManager::SendDBSecretShopStatistic(CServerHandler* handler)
{
    for (int s = 0; s < 3; s++)
    {
        if (m_secretShop[s].empty())
        {
            continue;
        }
        Packet_Secret_Shop_Statistic pkt;
        pkt.m_count = m_secretShop[s].size();
        pkt.m_shopIdx = s;
        int idx = 0;
        for (std::map<int, SECRET_SHOP_STATISTIC_DATA>::iterator it = m_secretShop[s].begin();
             it != m_secretShop[s].end(); ++it)
        {
            pkt.m_items[idx].m_data[0] = it->first;
            for (int k = 1; k < 5; k++)
            {
                pkt.m_items[idx].m_data[k] = it->second.m_data[k];
            }
            idx++;
        }
        handler->SendToDB((PacketHeader*)&pkt);
    }
}
void StatisticManager::ResetSecretShopStatistic()
{
    for (int i = 0; i < 3; i++)
    {
        m_secretShop[i].clear();
    }
}
void StatisticManager::AddGoldcardEventStatistic(Packet_Goldcard_Event_Statistic_GTS* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        char m_f0a;
        int m_f0b;
        int m_f0f;
    };
    int idx = (unsigned char)((Wire*)pkt)->m_f0a;
    if (0 <= idx && idx < 100)
    {
        m_goldcard[idx].m_field1 += ((Wire*)pkt)->m_f0b;
        m_goldcard[idx].m_field5 += ((Wire*)pkt)->m_f0f;
    }
}
void StatisticManager::SendDBGoldcardEventStatistic(CServerHandler* handler)
{
    Packet_Goldcard_Event_Statistic_STD pkt;
    memcpy(&pkt.m_items, this->m_goldcard, 0x37b);
    handler->SendToDB((PacketHeader*)&pkt);
}
void StatisticManager::ResetGoldcardEventStatistic()
{
    memset(m_goldcard, 0, 0x37b);
}
void StatisticManager::AddTowerOfDespairStatistic(Packet_TowerOfDespair_Statistic_GTS* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xe];
        int m_f0e;
        char m_f12;
    };
    if (pkt == 0)
    {
        return;
    }
    if (((Wire*)pkt)->m_f0e <= 0)
    {
        return;
    }
    if (0x64 < ((Wire*)pkt)->m_f0e)
    {
        return;
    }
    if (((Wire*)pkt)->m_f12 == 0)
    {
        *(int*)((char*)this + (((Wire*)pkt)->m_f0e + 0x100) * 8 + 7) += 1;
    }
    else
    {
        *(int*)((char*)this + (((Wire*)pkt)->m_f0e + 0x100) * 8 + 0xb) += 1;
        m_serverList.insert(((Wire*)pkt)->m_f0e);
    }
}
void StatisticManager::SendDBTowerOfDespairStatistic(CServerHandler* handler)
{
    Packet_TowerOfDespair_Statistic_STD pkt;
    pkt.m_countA = handler->GetServerGroupNo();
    memcpy(&pkt.m_items, this->m_tower, 0x328);
    pkt.m_countB = m_serverList.size();
    handler->SendToDB((PacketHeader*)&pkt);
    DNF_LOG_SCOPE_LINE(0x837, "./log/statistic", "TOD Send to DB");
}
void StatisticManager::ResetTowerOfDespair()
{
    DNF_LOG_SCOPE_LINE(0x83c, "./log/statistic", "TOD reset");
    memset(m_tower, 0, 0x328);
    m_serverList.clear();
}
void StatisticManager::AddMoneyLog(MoneyLogPacket* pkt, CServerHandler* handler)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        unsigned int m_f0a;   // +0xa
        unsigned int m_f0e;   // +0xe
        unsigned int m_f12;   // +0x12
        unsigned int m_f16;   // +0x16
        unsigned int m_f1a;   // +0x1a
    };
    PacketInsertUpdate p;
    p.m_handleIdx = 4;
    p.m_updateQueryId = 0x4f2d;
    // ORIG：参数按从右到左求值，a/b 在 NumberToString 之前载入 esi/ebx 并跨调用保留；
    // 写局部变量会被 GCC 压栈，须内联。
    snprintf(p.m_updateSql, 0x800,
        "inSert into log_charac_money(charac_no,occ_date,m_id,money_plus,money_minus) values(%u,cast(from_unixtime(%d) as date),%s,%u,%u)",
        ((Wire*)pkt)->m_f0a, ((Wire*)pkt)->m_f1a,
        NumberToString(((Wire*)pkt)->m_f16, 0),
        ((Wire*)pkt)->m_f0e, ((Wire*)pkt)->m_f12);
    p.m_insertQueryId = 0x4f2e;
    snprintf(p.m_insertSql, 0x800,
        "update log_charac_money set money_plus=money_plus+%u,money_minus=money_minus+%u where charac_no=%u and occ_date=cast(from_unixtime(%d) as date)",
        ((Wire*)pkt)->m_f0e, ((Wire*)pkt)->m_f12,
        ((Wire*)pkt)->m_f0a, ((Wire*)pkt)->m_f1a);
    handler->SendToDB((PacketHeader*)&p);
}
void StatisticManager::AddCompatibilityIndex(Packet_Stat_Compatibility_Index* pkt,
                                             CServerHandler* handler)
{
    for (;;)
    {
        if (pkt == 0) break;
        if (handler == 0) break;
        handler->SendToDB((PacketHeader*)pkt);
        break;
    }
}
void StatisticManager::minPing(short& a, short& b)
{
    for (;;)
    {
        printf("minPing(%d, %d)\n", a, b);
        if (-1 < b)
        {
            if (a > b)
            {
                a = b;
            }
            printf("minPing Res(%d)\n", a);
        }
        break;
    }
}
void StatisticManager::maxPing(short& a, short& b)
{
    for (;;)
    {
        printf("maxPing(%d, %d)\n", a, b);
        if (-1 < b)
        {
            if (a < b)
            {
                a = b;
            }
            printf("maxPing Res(%d)\n", a);
        }
        break;
    }
}
void StatisticManager::sumPing(int& a, short& b, int& c)
{
    for (;;)
    {
        printf("sumPing(%d, %d, %d)\n", a, (int)b, c);
        if (-1 < b)
        {
            if (a + (int)b < 0)
            {
                a = 0x7fffffff;
            }
            else
            {
                a = a + (int)b;
                if (-1 < c + 1)
                {
                    c = c + 1;
                }
            }
            printf("sumPing Res(%d, %d, %d)\n", a, (int)b, c);
        }
        break;
    }
}
void StatisticManager::avgPing(int& a, int& b, short& c)
{
    printf("avgPing(%d, %d, %d)\n", a, b, (int)c);
    double avg = 0.0;
    if (a == 0 || b == 0)
    {
        avg = 0.0;
    }
    else
    {
        avg = (double)a / (double)b;
    }
    c = (short)(int)avg;
    printf("avgPing Res(%d)\n", (int)c);
}
void StatisticManager::AddP2PStatistic(Packet_P2P_Statistics* pkt)
{
    m_p2p.m_field0 += pkt->m_fieldA;
    m_p2p.m_field4 += pkt->m_fieldB;
    m_p2p.m_field8 = pkt->m_fieldC;
    minPing(*(short*)((char*)this + 0xb3a), *(short*)((char*)pkt + 0x13));
    maxPing(*(short*)((char*)this + 0xb3c), *(short*)((char*)pkt + 0x15));
    sumPing(*(int*)((char*)this + 0xb40), *(short*)((char*)pkt + 0x17),
            *(int*)((char*)this + 0xb44));
    m_p2p.m_field18 += pkt->m_fieldG;
    m_p2p.m_field1c += pkt->m_fieldH;
    m_p2p.m_field20 += pkt->m_fieldI;
    m_p2p.m_field24 += pkt->m_fieldJ;
    minPing(*(short*)((char*)this + 0xb58), *(short*)((char*)pkt + 0x29));
    maxPing(*(short*)((char*)this + 0xb5a), *(short*)((char*)pkt + 0x2b));
    sumPing(*(int*)((char*)this + 0xb60), *(short*)((char*)pkt + 0x2d),
            *(int*)((char*)this + 0xb64));
    m_p2p.m_field38 += pkt->m_fieldN;
    m_p2p.m_field3c += pkt->m_fieldO;
    m_p2p.m_field40 += pkt->m_fieldP;
    m_p2p.m_field44 += pkt->m_fieldQ;
}
void StatisticManager::SendDBP2PStatistic(CServerHandler* handler)
{
    Packet_P2P_Statistics pkt;
    pkt.m_fieldC = m_p2p.m_field8;
    pkt.m_fieldA = m_p2p.m_field0;
    if (m_p2p.m_fieldA == 0x7fff)
    {
        pkt.m_fieldD = 0;
    }
    else
    {
        pkt.m_fieldD = m_p2p.m_fieldA;
    }
    avgPing(*(int*)((char*)this + 0xb40), *(int*)((char*)this + 0xb44),
            *(short*)((char*)this + 0xb3e));
    pkt.m_fieldE = m_p2p.m_fieldC;
    pkt.m_fieldF = m_p2p.m_fieldE;
    pkt.m_fieldG = m_p2p.m_field18;
    pkt.m_fieldH = m_p2p.m_field1c;
    pkt.m_fieldI = m_p2p.m_field20;
    pkt.m_fieldJ = m_p2p.m_field24;
    pkt.m_fieldB = m_p2p.m_field4;
    if (m_p2p.m_fieldB == 0x7fff)
    {
        pkt.m_fieldK = 0;
    }
    else
    {
        pkt.m_fieldK = m_p2p.m_fieldB;
    }
    avgPing(*(int*)((char*)this + 0xb60), *(int*)((char*)this + 0xb64),
            *(short*)((char*)this + 0xb5c));
    pkt.m_fieldL = m_p2p.m_field2a;
    pkt.m_fieldM = m_p2p.m_field2c;
    pkt.m_fieldN = m_p2p.m_field38;
    pkt.m_fieldO = m_p2p.m_field3c;
    pkt.m_fieldP = m_p2p.m_field40;
    pkt.m_fieldQ = m_p2p.m_field44;
    handler->SendToDB((PacketHeader*)&pkt);
}
void StatisticManager::ResetP2PStatistic()
{
    m_p2p.Init();
}
Packet_DBMW_HellParty_Statistic_Item::Packet_DBMW_HellParty_Statistic_Item()
    : PacketHeader(0xc32, 0x17ae)
{
}
void StatisticManager::ResetCreateEmblemInfo()
{
    m_createEmblem.clear();
}
bool STCubeStatisticKey::operator<(STCubeStatisticKey other) const
{
    if (m_field0 < other.m_field0) return true;
    if (m_field0 == other.m_field0)
    {
        if (m_field4 < other.m_field4) return true;
        if (m_field4 == other.m_field4)
        {
            if (m_field8 < other.m_field8) return true;
            if (m_field8 == other.m_field8 && (unsigned char)m_fieldc <
                                                  (unsigned char)other.m_fieldc) return true;
        }
    }
    return false;
}
