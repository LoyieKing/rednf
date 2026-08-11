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
STCubeStatisticKey::~STCubeStatisticKey()
{
}
StatisticManager::StatisticManager()
    : m_flag(0), m_field34(0)
{
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
    int m_padFrame;
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
        it->second += value;
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
    int m_padFrame;
    STDeathTowerPlayDataJobStatisticKey key;
    key.m_field0 = ((Wire*)pkt)->m_f0a;
    key.m_field2 = ((Wire*)pkt)->m_f0b;
    key.m_field4 = ((Wire*)pkt)->m_f0d;
    key.m_field8 = ((Wire*)pkt)->m_f11;
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
    int m_padFrame;
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
    int m_padFrame;
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
    int m_padFrame;
    STPartyJobStatisticKey key;
    key.m_field0 = 0;
    key.m_field4 = ((Wire*)pkt)->m_f0c;
    key.m_field8 = ((Wire*)pkt)->m_f10;
    key.m_field9 = ((Wire*)pkt)->m_f11;
    key.m_fielda = ((Wire*)pkt)->m_f12;
    key.m_fieldb = ((Wire*)pkt)->m_f13;
    key.m_fieldc = ((Wire*)pkt)->m_f14;
    key.m_fieldd = ((Wire*)pkt)->m_f15;
    key.m_field10 = ((Wire*)pkt)->m_f16;
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
        char m_pad[3];
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
    int m_padFrame;
    STPartyCharacKey key;
    key.m_field0 = 0;
    key.m_field4 = ((Wire*)pkt)->m_f0c;
    key.m_field8 = ((Wire*)pkt)->m_f10;
    key.m_field9 = ((Wire*)pkt)->m_f11;
    key.m_fielda = ((Wire*)pkt)->m_f12;
    key.m_fieldc = ((Wire*)pkt)->m_f13;
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
    int m_padFrame;
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
        it->second += 1;
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
            char* slot = (char*)&pkt + 0xe + idx * 0x19;
            *(unsigned short*)(slot + 0) = it->first.m_field0;
            *(unsigned int*)(slot + 4) = it->first.m_field4;
            slot[8] = it->first.m_field8;
            slot[9] = it->first.m_field9;
            slot[10] = it->first.m_fielda;
            slot[11] = it->first.m_fieldb;
            slot[12] = it->first.m_fieldc;
            slot[13] = it->first.m_fieldd;
            *(unsigned int*)(slot + 0x10) = it->first.m_field10;
            slot[0x14] = it->first.m_field14;
            *(int*)(slot + 0x15) = it->second.m_data[1];
            idx++;
            if (99 < idx)
            {
                *(unsigned int*)((char*)&pkt + 0xa) = 100;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x1b3, "./log/statistic", "Packet_DBMW_Dungeon_Statistic_Party_Job : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            *(unsigned int*)((char*)&pkt + 0xa) = idx;
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x1bd, "./log/statistic", "Packet_DBMW_Dungeon_Statistic_Party_Job : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
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
            char* slot = (char*)&pkt + 0xe + idx * 0x38;
            *(unsigned short*)(slot + 0) = it->first.m_field0;
            *(unsigned int*)(slot + 4) = it->first.m_field4;
            slot[8] = it->first.m_field8;
            slot[9] = it->first.m_field9;
            slot[10] = it->first.m_fielda;
            *(unsigned int*)(slot + 0xc) = it->first.m_fieldc;
            slot[0x10] = it->first.m_field10;
            slot[0x11] = it->first.m_field11;
            for (int k = 0; k < 12; k++)
            {
                *(int*)(slot + 0x12 + k * 4) = it->second.m_data[k];
            }
            idx++;
            if (99 < idx)
            {
                *(unsigned int*)((char*)&pkt + 0xa) = 100;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x1f0, "./log/statistic", "Packet_DBMW_Dungeon_Statistic_Party_Charac : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            *(unsigned int*)((char*)&pkt + 0xa) = idx;
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x1fa, "./log/statistic", "Packet_DBMW_Dungeon_Statistic_Party_Charac : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
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
            char* slot = (char*)&pkt + 0xe + idx * 0x10;
            slot[0] = it->first.m_field0;
            *(unsigned short*)(slot + 2) = it->first.m_field2;
            *(unsigned int*)(slot + 4) = it->first.m_field4;
            *(int*)(slot + 8) = it->second.m_data[0];
            *(int*)(slot + 0xc) = it->second.m_data[1];
            idx++;
            if (99 < idx)
            {
                *(unsigned int*)((char*)&pkt + 0xa) = 100;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x217, "./log/statistic", "Packet_DBMW_DeathTower_Statistic_Value : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            *(unsigned int*)((char*)&pkt + 0xa) = idx;
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x220, "./log/statistic", "Packet_DBMW_DeathTower_Statistic_Value : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
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
            char* slot = (char*)&pkt + 0xe + idx * 0xc;
            slot[0] = it->first.m_field0;
            *(unsigned short*)(slot + 2) = it->first.m_field2;
            *(unsigned int*)(slot + 4) = it->first.m_field4;
            slot[8] = it->first.m_field8;
            *(int*)(slot + 0xc) = it->second.m_data[0];
            *(int*)(slot + 0x10) = it->second.m_data[1];
            idx++;
            if (99 < idx)
            {
                *(unsigned int*)((char*)&pkt + 0xa) = 100;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x236, "./log/statistic", "DeathTowerPlayDataJob DB Sent %d", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            *(unsigned int*)((char*)&pkt + 0xa) = idx;
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x240, "./log/statistic", "DeathTowerPlayDataJob DB Sent %d", idx);
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
            char* slot = (char*)&pkt + 0xe + idx * 0xa;
            slot[0] = it->first.m_field0;
            slot[1] = it->first.m_field1;
            *(int*)(slot + 2) = it->second.m_data[0];
            *(int*)(slot + 6) = it->second.m_data[1];
            idx++;
            if (99 < idx)
            {
                *(unsigned int*)((char*)&pkt + 0xa) = 100;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x26d, "./log/statistic", "DeathTowerPlayDataParty DB Sent %d", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            *(unsigned int*)((char*)&pkt + 0xa) = idx;
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x277, "./log/statistic", "DeathTowerPlayDataParty DB Sent %d", idx);
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
            *(char*)((char*)&pkt + 0xa) = it->first.m_field0;
            *(unsigned short*)((char*)&pkt + 0xb) = it->first.m_field2;
            *(unsigned int*)((char*)&pkt + 0xd) = it->second;
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
        char m_pad[0x104];
        unsigned short m_f10e;
        int m_f110;
    };
    int m_padFrame;
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
            char* slot = (char*)&pkt + 0xe + idx * 0x206;
            memcpy(slot, it->first.m_str0, 0x100);
            *(unsigned short*)(slot + 0x100) = it->first.m_field100;
            memcpy(slot + 0x102, it->first.m_str2, 0x100);
            *(int*)(slot + 0x202) = it->second;
            idx++;
            if (99 < idx)
            {
                *(unsigned int*)((char*)&pkt + 0xa) = 100;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x2e1, "./log/Statistic", "Packet_DBMW_Assert_Manager_Info_Write_Query : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            *(unsigned int*)((char*)&pkt + 0xa) = idx;
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
    int m_padFrame;
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
            std::map<unsigned int, int>::iterator it2 = m_field110.find(key.m_field0);
            bool isNew2 = (m_field110.empty() || it2 == m_field110.end());
            if (isNew2)
            {
                m_field110.insert(std::make_pair(key.m_field0, 1));
            }
            else
            {
                it2->second += 1;
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
            *(unsigned int*)((char*)&pkt + 0xe + idx * 8) = it->first;
            *(int*)((char*)&pkt + 0xe + idx * 8 + 4) = it->second;
            idx++;
            if (99 < idx)
            {
                *(unsigned int*)((char*)&pkt + 0xa) = 100;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x327, "./log/Statistic", "Packet_DBMW_Ting_User_TimeCheck_Write_Query : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            *(unsigned int*)((char*)&pkt + 0xa) = idx;
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
            *(unsigned int*)((char*)&pkt + 0xe + idx * 8) = it->first.m_field0;
            *(int*)((char*)&pkt + 0xe + idx * 8 + 4) = it->second;
            idx++;
            if (99 < idx)
            {
                *(unsigned int*)((char*)&pkt + 0xa) = 100;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x34c, "./log/Statistic", "Packet_DBMW_User_Ting_TimeCheck_Write_Query : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            *(unsigned int*)((char*)&pkt + 0xa) = idx;
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
    int m_padFrame;
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
            char* slot = (char*)&pkt + 0xe + idx * 0x24;
            slot[0] = it->first.m_field0;
            *(unsigned int*)(slot + 4) = it->first.m_field4;
            slot[8] = it->first.m_field8;
            slot[9] = it->first.m_field9;
            slot[10] = it->first.m_fielda;
            *(int*)(slot + 0x14) = it->second.m_count;
            for (int k = 0; k < 6; k++)
            {
                *(int*)(slot + 0x18 + k * 4) = it->second.m_data[k];
            }
            idx++;
            if (99 < idx)
            {
                *(unsigned int*)((char*)&pkt + 0xa) = 100;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x391, "./log/statistic", "Packet_DBMW_HellParty_Statistic_Item : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            *(unsigned int*)((char*)&pkt + 0xa) = idx;
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x39b, "./log/statistic", "Packet_DBMW_HellParty_Statistic_Item : (%d) \xb0\xb3 \xc6\xd0\xc5\xb6 \xc0\xfc\xbc\xdb\n", idx);
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
            m_loading.m_data[i + 9] += 1;
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
                        else
                        {
                            it->second.m_field2 += *(unsigned short*)((char*)pkt + j * 9 + 0x37);
                            it->second.m_field4 += *(unsigned short*)((char*)pkt + j * 9 + 0x39);
                            it->second.m_field6 += *(unsigned short*)((char*)pkt + j * 9 + 0x3b);
                        }
                    }
                }
            }
            unsigned short gcount = *(unsigned short*)((char*)pkt + 0x4e);
            if (gcount != 0)
            {
                STPowerwarFightLagKey gkey;
                gkey.m_field0 = *(unsigned int*)((char*)pkt + 0x4a);
                for (int j = 0; j < (int)gcount; j++)
                {
                    gkey.m_field4 = *(unsigned short*)((char*)pkt + j * 0xd + 0x50);
                    if (m_pwLag.size() < 0x3e9)
                    {
                        std::map<STPowerwarFightLagKey, STPowerwarFightLagData>::iterator it =
                            m_pwLag.find(gkey);
                        bool isNew = (m_pwLag.empty() || it == m_pwLag.end());
                        if (isNew)
                        {
                            STPowerwarFightLagData v;
                            v.m_field0 = *(unsigned int*)((char*)pkt + j * 0xd + 0x52);
                            v.m_field4 = *(unsigned int*)((char*)pkt + j * 0xd + 0x56);
                            v.m_field8 = *(unsigned int*)((char*)pkt + j * 0xd + 0x5a);
                            m_pwLag.insert(std::make_pair(gkey, v));
                        }
                        else
                        {
                            it->second.m_field0 += *(unsigned int*)((char*)pkt + j * 0xd + 0x52);
                            it->second.m_field4 += *(unsigned int*)((char*)pkt + j * 0xd + 0x56);
                            it->second.m_field8 += *(unsigned int*)((char*)pkt + j * 0xd + 0x5a);
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
        *(char*)((char*)&pkt + 0xa + i) = handler->GetServerGroupNo();
        if (m_loading.m_data[i + 9] == 0)
        {
            *(unsigned int*)((char*)&pkt + 0x13 + i * 4) = 0;
        }
        else
        {
            *(unsigned int*)((char*)&pkt + 0x13 + i * 4) =
                m_loading.m_data[i] / m_loading.m_data[i + 9];
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
            char* slot = (char*)&pkt + 0xe + idx * 0x14;
            *(unsigned int*)(slot + 0) = it->first.m_field0;
            *(unsigned short*)(slot + 4) = it->first.m_field4;
            slot[6] = it->second.m_field0;
            *(unsigned short*)(slot + 8) = it->second.m_field2;
            *(unsigned short*)(slot + 10) = it->second.m_field4;
            *(unsigned short*)(slot + 12) = it->second.m_field6;
            idx++;
            if (99 < idx)
            {
                *(unsigned int*)((char*)&pkt + 0xa) = 100;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x3cf, "./log/statistic", "PowerwarLoading DB Sent %d", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            *(unsigned int*)((char*)&pkt + 0xa) = idx;
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x3d9, "./log/statistic", "PowerwarLoading DB Sent %d", idx);
        }
        m_pwLoading.clear();
    }
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
            char* slot = (char*)&pkt + 0xe + idx * 0x14;
            *(unsigned int*)(slot + 0) = it->first.m_field0;
            *(unsigned short*)(slot + 4) = it->first.m_field4;
            *(unsigned int*)(slot + 8) = it->second.m_field0;
            *(unsigned int*)(slot + 12) = it->second.m_field4;
            *(unsigned int*)(slot + 16) = it->second.m_field8;
            idx++;
            if (99 < idx)
            {
                *(unsigned int*)((char*)&pkt + 0xa) = 100;
                handler->SendToDB((PacketHeader*)&pkt);
                DNF_LOG_SCOPE_LINE(0x33b, "./log/statistic", "PowerwarLag DB Sent %d", idx);
                idx = 0;
            }
        }
        if (idx != 0)
        {
            *(unsigned int*)((char*)&pkt + 0xa) = idx;
            handler->SendToDB((PacketHeader*)&pkt);
            DNF_LOG_SCOPE_LINE(0x345, "./log/statistic", "PowerwarLag DB Sent %d", idx);
        }
        m_pwLag.clear();
    }
}
void StatisticManager::DBSaveProcess(CServerHandler* handler)
{
    time_t now = time(0);
    tm* pt = localtime(&now);
    int min = pt->tm_min;
    int hour = pt->tm_hour;
    int mday = pt->tm_mday;
    int mon = pt->tm_mon;
    int year = pt->tm_year;
    printf("---Time : %d, %d ----\n", hour, min);
    char ts[20];
    snprintf(ts, 0x13, "%d-%d-%d %d:%d:0", year + 0x76c, mon + 1, mday, hour, min);
    SendDBP2PStatistic(handler);
    ResetP2PStatistic();
    if (hour == 5 && min == 0)
    {
        SendDBHellPartyStatisticItem(handler);
        ResetHellPartyStatisticItemMap();
    }
    if (hour == 5 && min == 0)
    {
        SendDBFatigueBattery(handler);
        ResetFatigueBattery();
    }
    if (hour == 5 && min == 0)
    {
        SendDBDisjointAvatarInfoTotal(handler);
        ResetDisjointAvatarInfoTotal();
    }
    if (hour == 5 && min == 0)
    {
        SendDBCreateEmblemInfo(handler);
        ResetCreateEmblemInfo();
    }
    if (hour == 5 && min == 0)
    {
        SendDBRandomboxStatistic(handler);
        ResetRandomboxStatistic();
    }
    SendDBTingUserTimeCheck(handler);
    ResetTingUserTimeCheckMap();
    SendDBPowerwarLoadingTimeReport(handler);
    SendDBPowerwarLagReport(handler);
    if (min % 10 == 0)
    {
        SendDBServerMatchData(handler);
        ResetServerMatchData();
    }
    if (min == 0 || min == 0x1e)
    {
        snprintf(ts, 0x13, "%d-%d-%d %d:%d:0", year + 0x76c, mon + 1, mday, hour, min);
        SendDBPacketOverflowStatistic(handler);
        ResetPacketOverflowMap();
        SendDBAssertManagerStatistic(handler);
        ResetAssertManagerMap();
        SendDBUserTingTimeCheckStatistic(handler);
        ResetUserTIngTimeCheckMap();
        SendDBLagStatistics(handler, ts);
        statistc_proxy::sendDBStatisticProxy();
        statistc_proxy::resetStatisticProxy();
    }
    if (hour % 3 == 0 && min == 0)
    {
        SendDBLoadingTimeReport(handler);
    }
    if (hour == 5)
    {
        if (min == 10)
        {
            SendDBPartyStatistic(handler);
            ResetPartyMap();
        }
        if (min == 0xf)
        {
            SendDBPartyJobStatistic(handler);
            ResetPartyJobMap();
        }
        if (min == 0x14)
        {
            SendDBPartyCharacStatistic(handler);
            ResetPartyCharacMap();
        }
        if (min == 0x19)
        {
            SendDBDeathTowerValueStatistic(handler);
            ResetDeathTowerValueMap();
        }
        if (min == 0x1e)
        {
            SendDBDeathTowerPlayDataJobStatistic(handler);
            ResetDeathTowerPlayDataJobMap();
        }
        if (min == 0x23)
        {
            SendDBDeathTowerPlayDataPartyStatistic(handler);
            ResetDeathTowerPlayDataPartyMap();
        }
        if (min == 0x28)
        {
            SendDBBloodDungeonStatistic(handler);
            ResetBloodDungeon();
        }
        if (min == 0x2d)
        {
            SendDBValueStatistic(handler);
            ResetValueStatistic();
        }
        if (min == 0x32)
        {
            SendDBCirculationStatistic(handler);
            ResetCirculationStatistic();
        }
    }
    if (hour == 6 && min == 0)
    {
        SendDBSecretShopStatistic(handler);
        ResetSecretShopStatistic();
    }
    if (hour == 6 && min == 0)
    {
        SendDBTowerOfDespairStatistic(handler);
        ResetTowerOfDespair();
    }
    if (hour == 5 && min == 0)
    {
        SendDBGoldcardEventStatistic(handler);
        ResetGoldcardEventStatistic();
    }
    if (hour == 6)
    {
        CCubeStatistic* cube = (CCubeStatistic*)getCubeStatisticObject();
        cube->sendStatisticData(handler);
        cube = (CCubeStatistic*)getCubeStatisticObject();
        cube->resetStatisticData();
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
    value.m_field4 = (unsigned int)((Wire*)pkt)->m_f0f;
    std::map<unsigned char, STFatigueBattery>::iterator it = m_fatigue.find(*(char*)((char*)pkt + 10));
    if (it == m_fatigue.end())
    {
        m_fatigue.insert(std::make_pair(*(char*)((char*)pkt + 10), value));
    }
    else
    {
        it->second.m_field0 += ((Wire*)pkt)->m_f0b;
        it->second.m_field4 += (unsigned int)((Wire*)pkt)->m_f0f;
    }
}
void StatisticManager::ResetFatigueBattery()
{
    m_fatigue.clear();
}
void StatisticManager::SendDBFatigueBattery(CServerHandler* handler)
{
    if (!m_fatigue.empty())
    {
        Packet_DBMW_Fatigue_Battery_Money_Statistic pkt;
        for (std::map<unsigned char, STFatigueBattery>::iterator it = m_fatigue.begin();
             it != m_fatigue.end(); ++it)
        {
            unsigned int idx = (unsigned int)it->first;
            *(unsigned int*)((char*)&pkt + 0xa + idx * 8) = it->second.m_field0;
            *(unsigned int*)((char*)&pkt + 0xa + idx * 8 + 4) = it->second.m_field4;
        }
        handler->SendToDB((PacketHeader*)&pkt);
    }
}
void StatisticManager::AddBloodDungeonStatistics(Packet_Blood_dungeon_statistic* pkt)
{
    std::map<unsigned int, STBloodDungeonStatistic>::iterator it =
        m_blood.find(*(unsigned int*)((char*)pkt + 10));
    if (it == m_blood.end())
    {
        m_blood.insert(std::make_pair(*(unsigned int*)((char*)pkt + 10),
                                      *(STBloodDungeonStatistic*)((char*)pkt + 0xe)));
    }
    else
    {
        it->second.m_field0 += (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x12);
        it->second.m_field4 += (unsigned int)(unsigned char)*(char*)((char*)pkt + 0x13);
    }
}
void StatisticManager::ResetBloodDungeon()
{
    m_blood.clear();
}
void StatisticManager::SendDBBloodDungeonStatistic(CServerHandler* handler)
{
    if (!m_blood.empty())
    {
        Packet_DBMW_Query_String pkt;
        *(unsigned int*)((char*)&pkt + 0xa) = 0x4ed3;
        for (std::map<unsigned int, STBloodDungeonStatistic>::iterator it = m_blood.begin();
             it != m_blood.end(); ++it)
        {
            memset((char*)&pkt + 0xe, 0, 0x1001);
            snprintf((char*)&pkt + 0xe, 0x400,
                "inSert into log_blood_dungeon(occ_date,level,try_count,clear_count) values (now(),%d,%d,%d)",
                it->first, it->second.m_field0, it->second.m_field4);
            handler->SendToDB((PacketHeader*)&pkt);
        }
    }
}
void StatisticManager::ResetReasonCrashDownInfoMap()
{
    m_reasonCrash.clear();
}
void StatisticManager::AddReasonCrashDownData(Packet_Reason_Crash_Down_Info* pkt,
                                              CServerHandler* handler)
{
    Packet_DBMW_Reason_Crash_Down_Query query;
    char sql[256];
    memset(sql, 0, 0x100);
    snprintf(sql, 0xff,
             "inSert into log_client_ting_stat (occ_time,channel_no,reason,cnt) values (from_unixtime(%d),%d,%d,%d)",
             *(unsigned int*)((char*)pkt + 10), *(unsigned int*)((char*)pkt + 0xe),
             *(unsigned int*)((char*)pkt + 0x12), *(unsigned int*)((char*)pkt + 0x16));
    handler->SendToDB((PacketHeader*)&query);
    DNF_LOG_SCOPE_AT(__FUNCTION__, 0x5b8, "./log/ReasonCrashDown", "%s", sql);
}
void StatisticManager::AddDisjointAvatarInfo(Packet_Avater_Disjoint_Statistic* pkt)
{
    int count = *(int*)((char*)pkt + 10);
    if (-1 < count && count < 3)
    {
        for (int i = 0; i < count; i++)
        {
            m_disjoint.incCount(*(int*)((char*)pkt + i * 0xd + 0xe),
                                *(int*)((char*)pkt + i * 0xd + 0x12),
                                (int)(char)*(char*)((char*)pkt + i * 0xd + 0x16),
                                *(int*)((char*)pkt + i * 0xd + 0x17));
        }
    }
}
void StatisticManager::SendDBDisjointAvatarInfoTotal(CServerHandler* handler)
{
    Packet_Avater_Disjoint_Statistic_DB pkt;
    memcpy((char*)&pkt + 0xa, (char*)this + 0x1e8, 0x144);
    handler->SendToDB((PacketHeader*)&pkt);
    DNF_LOG_SCOPE_LINE(0x5fa, "./log/statistic", "Packet_Avater_Disjoint_Statistic_DB");
}
void StatisticManager::AddCreateEmblemInfo(Packet_Emblem_Create_Statistic* pkt)
{
    for (int i = 0; i < *(int*)((char*)pkt + 10); i++)
    {
        for (int j = 0; j < *(int*)((char*)pkt + (i + 8) * 4 + 2); j++)
        {
            m_createEmblem.increaseCount(*(int*)((char*)pkt + i * 4 + 0xe));
        }
    }
}
void StatisticManager::SendDBCreateEmblemInfo(CServerHandler* handler)
{
    Packet_Emblem_Create_Statistic_DB pkt;
    memcpy((char*)&pkt + 0xa, (char*)this + 0x32c, 0x1c);
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
    if ((char)((Wire*)pkt)->m_f0a < 5 && -1 < (char)((Wire*)pkt)->m_f0a)
    {
        if (((Wire*)pkt)->m_f0b == 0)
        {
            *(int*)((char*)this + ((char)((Wire*)pkt)->m_f0a + 0xd0) * 4 + 8) += 1;
        }
        else if (((Wire*)pkt)->m_f0b == 1)
        {
            *(int*)((char*)this + ((char)((Wire*)pkt)->m_f0a + 0xd4) * 4 + 0xc) += 1;
        }
    }
}
void StatisticManager::SendDBRandomboxStatistic(CServerHandler* handler)
{
    Packet_Randombox_statistic_DB pkt;
    for (int i = 0; i < 5; i++)
    {
        *(unsigned int*)((char*)&pkt + 0xa + i * 4) =
            *(unsigned int*)((char*)this + (i + 0xd0) * 4 + 8);
        *(unsigned int*)((char*)&pkt + 0x1e + i * 4) =
            *(unsigned int*)((char*)this + (i + 0xd4) * 4 + 0xc);
    }
    handler->SendToDB((PacketHeader*)&pkt);
}
void StatisticManager::ResetRandomboxStatistic()
{
    for (int i = 0; i < 5; i++)
    {
        *(unsigned int*)((char*)this + (i + 0xd0) * 4 + 8) = 0;
        *(unsigned int*)((char*)this + (i + 0xd4) * 4 + 0xc) = 0;
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
        if (*(int*)((char*)pkt + i * 10 + 0xb) != 0)
        {
            float avg_f = *(float*)((char*)pkt + i * 10 + 0xb);
            float dev_f = *(float*)((char*)pkt + i * 10 + 0xf);
            m_modules[i].m_data[0] += (int)(long long)(avg_f + 0.5);
            m_modules[i].m_data[1] += (int)(long long)(dev_f + 0.5);
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
                v.m_data[0] = (int)(long long)(a + 0.5);
                v.m_data[1] = (int)(long long)(b + 0.5);
                v.m_data[2] = (unsigned int)*(unsigned short*)((char*)pkt + j * 0x18 + 0x67);
                v.m_data[3] = 1;
                v.m_data[4] = (int)(long long)(c + 0.5);
                v.m_data[5] = (int)(long long)(d + 0.5);
                v.m_data[6] = (unsigned int)*(unsigned short*)((char*)pkt + j * 0x18 + 0x72);
                v.m_data[7] = 1;
                m_dungeonLag.insert(std::make_pair(key, v));
            }
            else
            {
                it->second.m_data[0] += (int)(long long)(a + 0.5);
                it->second.m_data[1] += (int)(long long)(b + 0.5);
                it->second.m_data[2] +=
                    (unsigned int)*(unsigned short*)((char*)pkt + j * 0x18 + 0x67);
                it->second.m_data[3] += 1;
                it->second.m_data[4] += (int)(long long)(c + 0.5);
                it->second.m_data[5] += (int)(long long)(d + 0.5);
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
    char sql[1024];
    for (int i = 0; i < 8; i++)
    {
        if (0 < m_modules[i].m_data[3])
        {
            memset(sql, 0, 0x400);
            int d1 = m_modules[i].m_data[2];
            int d2 = m_modules[i].m_data[3];
            unsigned int a = m_modules[i].m_data[1];
            unsigned int b = m_modules[i].m_data[3];
            unsigned int c = m_modules[i].m_data[0];
            unsigned int d = m_modules[i].m_data[3];
            unsigned int group = handler->GetServerGroupNo();
            snprintf(sql, 0x400,
                "inSert into lag_stat_module (occ_time, server_id, module, average, deviation, count) values ('%s', %d, %d, %d, %d, %d)",
                timeStr, group & 0xff, i, c / d, a / b, d1 / d2);
            DNF_LOG_SCOPE_LINE(0x6a1, "./log/LagStatistics", "%s", sql);
            handler->SendToDB((PacketHeader*)&pkt);
            m_modules[i].Reset();
        }
    }
    if (!m_dungeonLag.empty())
    {
        for (std::map<unsigned short, STDungeonLagStatistics>::iterator it = m_dungeonLag.begin();
             it != m_dungeonLag.end(); ++it)
        {
            memset(sql, 0, 0x400);
            int d1 = it->second.m_data[6];
            int d2 = it->second.m_data[7];
            unsigned int a = it->second.m_data[5];
            unsigned int b = it->second.m_data[7];
            unsigned int c = it->second.m_data[4];
            unsigned int d = it->second.m_data[7];
            unsigned int e = it->second.m_data[2];
            int f = it->second.m_data[3];
            unsigned int g = it->second.m_data[1];
            unsigned int group = handler->GetServerGroupNo();
            snprintf(sql, 0x400,
                "inSert into lag_stat_dungeon (occ_time, server_id, dungeon_idx, first_average, first_deviation, first_count, boss_average, boss_deviation, boss_count) values ('%s', %d, %d, %d, %d, %d, %d, %d, %d)",
                timeStr, group & 0xff, it->first, c / d, a / b, d1 / d2, e, f, g);
            DNF_LOG_SCOPE_LINE(0x6b8, "./log/LagStatistics", "%s", sql);
            handler->SendToDB((PacketHeader*)&pkt);
        }
        m_dungeonLag.clear();
    }
}
void StatisticManager::AddValueStatistics(Packet_Value_Statistic* pkt)
{
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        int m_f0a;
        int m_data[0x1e];
    };
    std::map<int, ValueStatisticData>::iterator it = m_value.find(*(int*)((char*)pkt + 10));
    if (it != m_value.end())
    {
        for (int i = 0; i < 0x1e; i++)
        {
            it->second.m_data[i] += ((Wire*)pkt)->m_data[i];
        }
    }
    else
    {
        ValueStatisticData v;
        for (int i = 0; i < 0x1e; i++)
        {
            v.m_data[i] = ((Wire*)pkt)->m_data[i];
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
    if (!m_value.empty())
    {
        Packet_DBMW_Query_String pkt;
        *(unsigned int*)((char*)&pkt + 0xa) = 0x4ef5;
        time_t now = time(0);
        for (std::map<int, ValueStatisticData>::iterator it = m_value.begin();
             it != m_value.end(); ++it)
        {
            int key = it->first;
            ValueStatisticData* v = &it->second;
            memset((char*)&pkt + 0xe, 0, 0x1001);
            snprintf((char*)&pkt + 0xe, 0x400,
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
    struct __attribute__((packed)) Wire
    {
        char m_hdr[0xa];
        int m_f0a;
        int m_data[0x30];
    };
    std::map<int, CirculationStatisticData>::iterator it = m_circ.find(*(int*)((char*)pkt + 10));
    if (it != m_circ.end())
    {
        for (int i = 0; i < 0x30; i++)
        {
            it->second.m_data[i] += ((Wire*)pkt)->m_data[i];
        }
    }
    else
    {
        CirculationStatisticData v;
        for (int i = 0; i < 0x30; i++)
        {
            v.m_data[i] = ((Wire*)pkt)->m_data[i];
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
    if (!m_circ.empty())
    {
        Packet_DBMW_Query_String pkt;
        *(unsigned int*)((char*)&pkt + 0xa) = 0x4ef6;
        time_t now = time(0);
        for (std::map<int, CirculationStatisticData>::iterator it = m_circ.begin();
             it != m_circ.end(); ++it)
        {
            int key = it->first;
            CirculationStatisticData* v = &it->second;
            memset((char*)&pkt + 0xe, 0, 0x1001);
            snprintf((char*)&pkt + 0xe, 0x1000,
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
    if (*(int*)((char*)pkt + 10) != 0 && *(int*)((char*)pkt + 0xe) < 3)
    {
        int shopIdx = *(int*)((char*)pkt + 0xe);
        for (int i = 0; i < *(int*)((char*)pkt + 10); i++)
        {
            std::map<int, SECRET_SHOP_STATISTIC_DATA>::iterator it =
                m_secretShop[shopIdx].find(*(int*)((char*)pkt + i * 0x14 + 0x12));
            if (it == m_secretShop[shopIdx].end())
            {
                m_secretShop[shopIdx].insert(std::make_pair(
                    *(int*)((char*)pkt + i * 0x14 + 0x12),
                    *(SECRET_SHOP_STATISTIC_DATA*)((char*)pkt + i * 0x14 + 0x12)));
            }
            else
            {
                it->second.m_data[3] += *(int*)((char*)pkt + i * 0x14 + 0x1e);
                it->second.m_data[1] += *(int*)((char*)pkt + i * 0x14 + 0x16);
                it->second.m_data[2] += *(int*)((char*)pkt + i * 0x14 + 0x1a);
                it->second.m_data[4] += *(int*)((char*)pkt + i * 0x14 + 0x22);
            }
        }
    }
}
void StatisticManager::SendDBSecretShopStatistic(CServerHandler* handler)
{
    Packet_Secret_Shop_Statistic pkt;
    if (!m_secretShop[0].empty() || !m_secretShop[1].empty() || !m_secretShop[2].empty())
    {
        for (int s = 0; s < 3; s++)
        {
            for (std::map<int, SECRET_SHOP_STATISTIC_DATA>::iterator it = m_secretShop[s].begin();
                 it != m_secretShop[s].end(); ++it)
            {
                *(int*)((char*)&pkt + 0xe + it->first * 0x14 + 0) = it->first;
                for (int k = 0; k < 4; k++)
                {
                    *(int*)((char*)&pkt + 0xe + it->first * 0x14 + 4 + k * 4) =
                        it->second.m_data[k + 1];
                }
            }
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
    unsigned int idx = (unsigned int)(unsigned char)((Wire*)pkt)->m_f0a;
    if (idx < 100)
    {
        *(int*)((char*)this + idx * 9 + 0x48d) += ((Wire*)pkt)->m_f0b;
        *(int*)((char*)this + idx * 9 + 0x491) += ((Wire*)pkt)->m_f0f;
    }
}
void StatisticManager::SendDBGoldcardEventStatistic(CServerHandler* handler)
{
    Packet_Goldcard_Event_Statistic_STD pkt;
    memcpy((char*)&pkt + 10, (char*)this + 0x48c, 0x37b);
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
    if (pkt != 0 && 0 < ((Wire*)pkt)->m_f0e && ((Wire*)pkt)->m_f0e < 0x65)
    {
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
}
void StatisticManager::SendDBTowerOfDespairStatistic(CServerHandler* handler)
{
    Packet_TowerOfDespair_Statistic_STD pkt;
    unsigned int group = handler->GetServerGroupNo();
    group = group & 0xff;
    memcpy((char*)&pkt + 0x12, (char*)this + 0x807, 0x328);
    unsigned int size = m_serverList.size();
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
    PacketInsertUpdate p;
    p.m_handleIdx = 4;
    p.m_updateQueryId = 0x4f2d;
    unsigned int a = *(unsigned int*)((char*)pkt + 0x12);
    unsigned int b = *(unsigned int*)((char*)pkt + 0xe);
    char* uid = NumberToString(*(unsigned int*)((char*)pkt + 0x16), 0);
    snprintf(p.m_updateSql, 0x800,
        "inSert into log_charac_money(charac_no,occ_date,m_id,money_plus,money_minus) values(%u,cast(from_unixtime(%d) as date),%s,%u,%u)",
        *(unsigned int*)((char*)pkt + 10), *(unsigned int*)((char*)pkt + 0x1a), uid, b, a);
    p.m_insertQueryId = 0x4f2e;
    snprintf(p.m_insertSql, 0x800,
        "update log_charac_money set money_plus=money_plus+%u,money_minus=money_minus+%u where charac_no=%u and occ_date=cast(from_unixtime(%d) as date)",
        *(unsigned int*)((char*)pkt + 0xe), *(unsigned int*)((char*)pkt + 0x12),
        *(unsigned int*)((char*)pkt + 10), *(unsigned int*)((char*)pkt + 0x1a));
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
    *(int*)((char*)&m_p2p + 0) += *(int*)((char*)pkt + 10);
    *(int*)((char*)&m_p2p + 4) += *(int*)((char*)pkt + 0xe);
    *(char*)((char*)&m_p2p + 8) = *(char*)((char*)pkt + 0x12);
    minPing(*(short*)((char*)&m_p2p + 0xa), *(short*)((char*)pkt + 0x13));
    maxPing(*(short*)((char*)&m_p2p + 0xc), *(short*)((char*)pkt + 0x15));
    sumPing(*(int*)((char*)&m_p2p + 0x10), *(short*)((char*)pkt + 0x17),
            *(int*)((char*)&m_p2p + 0x14));
    *(int*)((char*)&m_p2p + 0x18) += *(int*)((char*)pkt + 0x19);
    *(int*)((char*)&m_p2p + 0x1c) += *(int*)((char*)pkt + 0x1d);
    *(int*)((char*)&m_p2p + 0x20) += *(int*)((char*)pkt + 0x21);
    *(int*)((char*)&m_p2p + 0x24) += *(int*)((char*)pkt + 0x25);
    minPing(*(short*)((char*)&m_p2p + 0x28), *(short*)((char*)pkt + 0x29));
    maxPing(*(short*)((char*)&m_p2p + 0x2a), *(short*)((char*)pkt + 0x2b));
    sumPing(*(int*)((char*)&m_p2p + 0x30), *(short*)((char*)pkt + 0x2d),
            *(int*)((char*)&m_p2p + 0x34));
    *(int*)((char*)&m_p2p + 0x38) += *(int*)((char*)pkt + 0x2f);
    *(int*)((char*)&m_p2p + 0x3c) += *(int*)((char*)pkt + 0x33);
    *(int*)((char*)&m_p2p + 0x40) += *(int*)((char*)pkt + 0x37);
    *(int*)((char*)&m_p2p + 0x44) += *(int*)((char*)pkt + 0x3b);
}
void StatisticManager::SendDBP2PStatistic(CServerHandler* handler)
{
    Packet_P2P_Statistics pkt;
    *(unsigned int*)((char*)&pkt + 0xa) = *(unsigned int*)((char*)&m_p2p + 0);
    *(char*)((char*)&pkt + 0xe) = *(char*)((char*)&m_p2p + 8);
    if (*(short*)((char*)&m_p2p + 0xa) == 0x7fff)
    {
        *(short*)((char*)&pkt + 0xf) = 0;
    }
    else
    {
        *(short*)((char*)&pkt + 0xf) = *(short*)((char*)&m_p2p + 0xa);
    }
    avgPing(*(int*)((char*)&m_p2p + 0x10), *(int*)((char*)&m_p2p + 0x14),
            *(short*)((char*)&m_p2p + 0xe));
    *(short*)((char*)&pkt + 0x11) = *(short*)((char*)&m_p2p + 0xc);
    *(short*)((char*)&pkt + 0x13) = *(short*)((char*)&m_p2p + 0xe);
    *(int*)((char*)&pkt + 0x15) = *(int*)((char*)&m_p2p + 0x18);
    *(int*)((char*)&pkt + 0x19) = *(int*)((char*)&m_p2p + 0x1c);
    *(int*)((char*)&pkt + 0x1d) = *(int*)((char*)&m_p2p + 0x20);
    *(int*)((char*)&pkt + 0x21) = *(int*)((char*)&m_p2p + 0x24);
    *(int*)((char*)&pkt + 0x25) = *(int*)((char*)&m_p2p + 4);
    if (*(short*)((char*)&m_p2p + 0x28) == 0x7fff)
    {
        *(short*)((char*)&pkt + 0x29) = 0;
    }
    else
    {
        *(short*)((char*)&pkt + 0x29) = *(short*)((char*)&m_p2p + 0x28);
    }
    avgPing(*(int*)((char*)&m_p2p + 0x30), *(int*)((char*)&m_p2p + 0x34),
            *(short*)((char*)&m_p2p + 0x2e));
    *(short*)((char*)&pkt + 0x2b) = *(short*)((char*)&m_p2p + 0x2a);
    *(short*)((char*)&pkt + 0x2d) = *(short*)((char*)&m_p2p + 0x2e);
    *(int*)((char*)&pkt + 0x2f) = *(int*)((char*)&m_p2p + 0x38);
    *(int*)((char*)&pkt + 0x33) = *(int*)((char*)&m_p2p + 0x3c);
    *(int*)((char*)&pkt + 0x37) = *(int*)((char*)&m_p2p + 0x40);
    *(int*)((char*)&pkt + 0x3b) = *(int*)((char*)&m_p2p + 0x44);
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
