// df_statics_r — GMAccounts（ORIG GMAccounts.o 拆分）
#include <stdio.h>
#include <string.h>

#include "GMAccounts.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

Packet_DBMW_Dungeon_Statistic_Party::Packet_DBMW_Dungeon_Statistic_Party()
    : PacketHeader(0xc36, 0x177e)
{
}
Packet_DBMW_Dungeon_Statistic_Party_Job::Packet_DBMW_Dungeon_Statistic_Party_Job()
    : PacketHeader(0xc38, 0x17c9)
{
}
Packet_DBMW_Dungeon_Statistic_Party_Charac::Packet_DBMW_Dungeon_Statistic_Party_Charac()
    : PacketHeader(0xc3a, 0x1759)
{
}
Packet_DBMW_DeathTower_Statistic_Value::Packet_DBMW_DeathTower_Statistic_Value()
    : PacketHeader(0xc3c, 0x17e7)
{
}
Packet_DBMW_DeathTower_Statistic_Playdata_Job::Packet_DBMW_DeathTower_Statistic_Playdata_Job()
    : PacketHeader(0xc3e, 0x17ee)
{
}
Packet_DBMW_DeathTower_Statistic_Playdata_Party::Packet_DBMW_DeathTower_Statistic_Playdata_Party()
    : PacketHeader(0xc40, 0x17f6)
{
}
Packet_DBMW_Assert_Manager_Info_Write_Query::Packet_DBMW_Assert_Manager_Info_Write_Query()
    : PacketHeader(0xc42, 0x1244)
{
}
Packet_DBMW_Packet_Overflow_Statistic::Packet_DBMW_Packet_Overflow_Statistic()
    : PacketHeader(0xc44, 0x11)
{
}
Packet_DBMW_User_Ting_TimeCheck_Write_Query::Packet_DBMW_User_Ting_TimeCheck_Write_Query()
    : PacketHeader(0xc46, 0x17fe)
{
}
Packet_DBMW_Ting_User_TimeCheck_Write_Query::Packet_DBMW_Ting_User_TimeCheck_Write_Query()
    : PacketHeader(0xc47, 0x17fe)
{
}
Packet_DBMW_Loading_Time_Report::Packet_DBMW_Loading_Time_Report()
    : PacketHeader(0xfb1, 0x37)
{
}
Packet_DBMW_Powerwar_Loading_Time_Report::Packet_DBMW_Powerwar_Loading_Time_Report()
    : PacketHeader(0xfb2, 0x170e)
{
}
Packet_DBMW_Powerwar_Lag_Report::Packet_DBMW_Powerwar_Lag_Report()
    : PacketHeader(0xfb3, 0x170e)
{
}
Packet_DBMW_Fatigue_Battery_Money_Statistic::Packet_DBMW_Fatigue_Battery_Money_Statistic()
    : PacketHeader(0xc4a, 0x332)
{
    memset((char*)this + 10, 0, 0x328);
}
Packet_DBMW_Query_String::Packet_DBMW_Query_String()
    : PacketHeader(0x1037, 0x100f)
{
}
Packet_DBMW_Reason_Crash_Down_Query::Packet_DBMW_Reason_Crash_Down_Query()
    : PacketHeader(0xc49, 0x10a)
{
}
stDisjointAvatarInfoTotal::stDisjointAvatarInfoTotal() throw()
{
    clear();
}
void stDisjointAvatarInfoTotal::clear()
{
    int i;
    int j;
    int k;
    for (i = 0; i < 3; i++)
    {
        for (j = 0; j < 9; j++)
        {
            for (k = 0; k < 2; k++)
            {
                m_data[(i * 9 + j) * 2 + k] = 0;
            }
            m_data[(i * 9 + j + 0x34) + 2] = 0;
        }
    }
}
bool stDisjointAvatarInfoTotal::checkCondition(int a, int b, int c)
{
    if (a < 0 || 2 < a) return 0;
    if (b < 0 || 8 < b) return 0;
    if (c < 0 || 1 < c) return 0;
    return 1;
}
void stDisjointAvatarInfoTotal::incCount(int a, int b, int c, int d)
{
    if (checkCondition(a, b, c))
    {
        m_data[(a * 9 + b) * 2 + c] += 1;
        m_data[a * 9 + b + 0x34 + 2] += d;
    }
}
Packet_Avater_Disjoint_Statistic_DB::Packet_Avater_Disjoint_Statistic_DB()
    : PacketHeader(0x17a3, 0x14e)
{
}
Packet_Randombox_statistic_DB::Packet_Randombox_statistic_DB()
    : PacketHeader(0x17b7, 0x32)
{
}
void stCreateEmblemStatistic::increaseCount(int idx)
{
    // ORIG 0x807579a：两段独立早退（js/jg 各自跳向带 nop 的返回路径）。
    if (idx < 0) return;
    if (6 < idx) return;
    m_data[idx]++;
}
void stCreateEmblemStatistic::clear()
{
    for (int i = 0; i < 7; i++)
    {
        m_data[i] = 0;
    }
}
Packet_Emblem_Create_Statistic_DB::Packet_Emblem_Create_Statistic_DB()
    : PacketHeader(0x17a5, 0x26)
{
}
Packet_DBMW_TechnicalReport_Common_Query::Packet_DBMW_TechnicalReport_Common_Query()
    : PacketHeader(0x17c5, 0x40a)
{
}
Packet_Server_Match_data_DBMW::Packet_Server_Match_data_DBMW()
    : PacketHeader(0x1b68, 0x13)
{
    m_fieldA = 0;
    m_fieldB = 0;
    m_fieldC = 0;
}
SECRET_SHOP_STATISTIC_DATA::SECRET_SHOP_STATISTIC_DATA()
{
    m_data[0] = 0;
    m_data[1] = 0;
    m_data[2] = 0;
    m_data[3] = 0;
    m_data[4] = 0;
}
Packet_Secret_Shop_Statistic::Packet_Secret_Shop_Statistic()
    : PacketHeader(0x1b69, 0xfb2), m_count(0)
{
}
GoldCardEventStatistic::GoldCardEventStatistic()
{
    m_field0 = 0;
    m_field1 = 0;
    m_field5 = 0;
}
Packet_Goldcard_Event_Statistic_STD::Packet_Goldcard_Event_Statistic_STD()
    : PacketHeader(0x1f47, 0x385)
{
    memset(m_items, 0, 0x37b);
}
TowerOfDespairStatistic_Value::TowerOfDespairStatistic_Value()
{
    m_field0 = 0;
    m_field4 = 0;
}
Packet_TowerOfDespair_Statistic_STD::Packet_TowerOfDespair_Statistic_STD()
    : PacketHeader(0x271d, 0x33a), m_countA(0), m_countB(0)
{
}
STPartyStatisticKey::STPartyStatisticKey()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_field9 = 0;
    m_fielda = 0;
    m_fieldb = 0;
    m_fieldc = 0;
    m_fieldd = 0;
}
STPartyStatisticKey::~STPartyStatisticKey()
{
}
PartyStatistic::PartyStatistic()
{
    Reset();
}
PartyStatistic::~PartyStatistic()
{
}
void PartyStatistic::Reset()
{
    m_data[0] = 0;
    m_data[1] = 0;
    m_data[2] = 0;
    m_data[3] = 0;
    m_data[4] = 0;
    m_data[5] = 0;
    m_data[6] = 0;
    m_data[7] = 0;
    m_data[8] = 0;
    m_data[9] = 0;
    m_data[10] = 0;
    m_data[11] = 1;
}
PartyJobStatistic::PartyJobStatistic()
{
    reset();
}
void PartyJobStatistic::reset()
{
    m_data[0] = 1;
    m_data[1] = 0;
}
STPartyJobStatisticKey::STPartyJobStatisticKey()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_field9 = 0;
    m_fielda = 0;
    m_fieldb = 0;
    m_fieldc = 0;
    m_fieldd = 0;
    m_field10 = (STPartyJobKeyField10)0;
    m_field14 = 0;
}
STPartyJobStatisticKey::~STPartyJobStatisticKey()
{
}
STPartyCharacKey::STPartyCharacKey()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_field9 = 0;
    m_fielda = 0;
    m_fieldc = (STPartyCharacKeyFieldC)0;
    m_field10 = 0;
    m_field11 = 0;
}
STPartyCharacKey::~STPartyCharacKey()
{
}
PartyCharacStatistic::PartyCharacStatistic()
{
    Reset();
}
PartyCharacStatistic::~PartyCharacStatistic()
{
}
void PartyCharacStatistic::Reset()
{
    m_data[0] = 0;
    m_data[1] = 0;
    m_data[2] = 0;
    m_data[3] = 0;
    m_data[4] = 0;
    m_data[5] = 0;
    m_data[6] = 0;
    m_data[7] = 0;
    m_data[8] = 0;
    m_data[9] = 0;
    m_data[10] = 0;
    m_data[11] = 1;
    m_data[12] = 0;
}
STDeathTowerValueStatisticKey::STDeathTowerValueStatisticKey() throw()
{
    m_field0 = 0;
    m_field2 = 0;
    m_field4 = 0;
}
STDeathTowerValueStatisticKey::~STDeathTowerValueStatisticKey() throw()
{
}
ValueStatistic::ValueStatistic() throw()
{
    Reset();
}
ValueStatistic::~ValueStatistic() throw()
{
}
void ValueStatistic::Reset() throw()
{
    m_data[0] = 0;
    m_data[1] = 1;
}
STDeathTowerPlayDataJobStatisticKey::STDeathTowerPlayDataJobStatisticKey()
{
    m_field0 = 0;
    m_field2 = 0;
    m_field4 = 0;
    m_field8 = 0;
}
STDeathTowerPlayDataJobStatisticKey::~STDeathTowerPlayDataJobStatisticKey()
{
}
PlayDataJobStatistic::PlayDataJobStatistic()
{
    Reset();
}
PlayDataJobStatistic::~PlayDataJobStatistic()
{
}
void PlayDataJobStatistic::Reset()
{
    m_data[0] = 0;
    m_data[1] = 1;
}
STDeathTowerPlayDataPartyStatisticKey::STDeathTowerPlayDataPartyStatisticKey()
{
    m_field0 = 0;
    m_field1 = 0;
}
STDeathTowerPlayDataPartyStatisticKey::~STDeathTowerPlayDataPartyStatisticKey()
{
}
PlayDataPartyStatistic::PlayDataPartyStatistic()
{
    Reset();
}
PlayDataPartyStatistic::~PlayDataPartyStatistic()
{
}
void PlayDataPartyStatistic::Reset()
{
    m_data[0] = 0;
    m_data[1] = 1;
}
STPacketOverflowKey::STPacketOverflowKey()
{
    Reset();
}
STPacketOverflowKey::~STPacketOverflowKey() throw()
{
}
void STPacketOverflowKey::Reset()
{
    m_field0 = 0;
    m_field2 = 0;
}
STAssertManagerKey::STAssertManagerKey()
{
    Reset();
}
STAssertManagerKey::~STAssertManagerKey()
{
}
void STAssertManagerKey::Reset()
{
    memset(m_str0, 0, 0x100);
    m_field100 = 0;
    memset(m_str2, 0, 0x100);
}
STUserTingTimeCheckKey::STUserTingTimeCheckKey()
{
    Reset();
}
STUserTingTimeCheckKey::~STUserTingTimeCheckKey()
{
}
void STUserTingTimeCheckKey::Reset()
{
    m_field0 = 0;
}
STHellPartyStatisticItemKey::STHellPartyStatisticItemKey()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field9 = 0;
    m_field8 = 0;
    m_fielda = 0;
}
STHellPartyStatisticItemKey::~STHellPartyStatisticItemKey()
{
}
HellPartyItenmData::HellPartyItenmData()
{
    Reset();
}
HellPartyItenmData::~HellPartyItenmData()
{
}
void HellPartyItenmData::Reset()
{
    memset(m_data, 0, 0x18);
    m_count = 1;
}
LoadingTimeReport::LoadingTimeReport() throw()
{
    Reset();
}
LoadingTimeReport::~LoadingTimeReport() throw()
{
}
void LoadingTimeReport::Reset() throw()
{
    for (int i = 0; i < 9; i++)
    {
        m_data[i] = 0;
        m_data2[i] = 0;
    }
}
STPowerwarFightLoadingKey::STPowerwarFightLoadingKey()
{
    Reset();
}
STPowerwarFightLoadingKey::~STPowerwarFightLoadingKey()
{
}
void STPowerwarFightLoadingKey::Reset()
{
    m_field0 = 0;
    m_field4 = 0;
}
STPowerwarFightLoadingData::STPowerwarFightLoadingData()
{
    Reset();
}
STPowerwarFightLoadingData::~STPowerwarFightLoadingData()
{
}
void STPowerwarFightLoadingData::Reset()
{
    m_field0 = 0;
    m_field2 = 0;
    m_field4 = 0;
    m_field6 = 0;
}
STPowerwarFightLagKey::STPowerwarFightLagKey()
{
    Reset();
}
STPowerwarFightLagKey::~STPowerwarFightLagKey()
{
}
void STPowerwarFightLagKey::Reset()
{
    m_field0 = 0;
    m_field4 = 0;
}
STPowerwarFightLagData::STPowerwarFightLagData()
{
    Reset();
}
STPowerwarFightLagData::~STPowerwarFightLagData()
{
}
void STPowerwarFightLagData::Reset()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
}
STFatigueBattery::STFatigueBattery()
{
    m_field0 = 0;
    m_field4 = 0;
}
STModuleLagStatistics::STModuleLagStatistics() throw()
{
    Reset();
}
STModuleLagStatistics::~STModuleLagStatistics() throw()
{
}
void STModuleLagStatistics::Reset() throw()
{
    m_data[0] = 0;
    m_data[1] = 0;
    m_data[2] = 0;
    m_data[3] = 0;
}
STDungeonLagStatistics::STDungeonLagStatistics()
{
    Reset();
}
STDungeonLagStatistics::~STDungeonLagStatistics()
{
}
void STDungeonLagStatistics::Reset()
{
    m_data[0] = 0;
    m_data[1] = 0;
    m_data[2] = 0;
    m_data[3] = 0;
    m_data[4] = 0;
    m_data[5] = 0;
    m_data[6] = 0;
    m_data[7] = 0;
}
ValueStatisticData::ValueStatisticData()
{
    for (int i = 0; i < 0x1e; i++)
    {
        m_data[i] = 0;
    }
}
CirculationStatisticData::CirculationStatisticData()
{
    for (int i = 0; i < 0x30; i++)
    {
        m_data[i] = 0;
    }
}
stP2PStatistics::stP2PStatistics()
{
    Init();
}
void stP2PStatistics::Init()
{
    memset(this, 0, 0x48);
    m_fieldA = 0x7fff;
    m_fieldB = 0x7fff;
}
namespace WongWork
{
bool CGMAccounts::stGMInfo_t::operator==(const stGMInfo_t& other) const
{
    return m_field0 == other.m_field0;
}

void CGMAccounts::LoadGmList(unsigned int group, int index)
{
    stGMInfo_t info;
    info.m_field0 = group;
    info.m_field1 = index;
    m_list.push_back(info);
}

void CGMAccounts::clearGmList()
{
    m_list.clear();
}

void CGMAccounts::AppendGM_Sys(unsigned int id, char flag)
{
    stGMInfo_t info = {};
    info.m_field0 = id;
    info.m_field1 = flag;
    m_list.push_back(info);
    register char* mid = NumberToString(id, 0);
    DNF_LOG_SCOPE_LINE(0xcd, "./log/Init", "GM List Add mid:%s", mid);
}

bool CGMAccounts::loadGMAccounts(const char* path)
{
    return true;
}

int CGMAccounts::isGM(unsigned int id)
{
    stGMInfo_t key = {};
    key.m_field1 = 3;
    key.m_field0 = id;
    if (std::find(m_list.begin(), m_list.end(), key) != m_list.end())
        return 1;
    else
        return 0;
}

int CGMAccounts::appendGM(unsigned int id, unsigned int value)
{
    // ORIG 实测：返回 0，帧 0x10（两个未用局部变量槽）
    int l0;
    int l1;
    return 0;
}

int CGMAccounts::removeGM(unsigned int id, unsigned int value)
{
    // ORIG 实测：返回 0，帧 0x20（五个未用局部变量槽）
    int l0;
    int l1;
    int l2;
    int l3;
    int l4;
    return 0;
}

CGMAccounts::stGMInfo_t CGMAccounts::getGMInfo(unsigned int id) const
{
    stGMInfo_t out;
    out.m_field0 = 0;
    out.m_field1 = 3;
    stGMInfo_t key = {};
    key.m_field1 = 3;
    key.m_field0 = id;
    std::list<stGMInfo_t>::const_iterator it =
        std::find(m_list.begin(), m_list.end(), key);
    if (it != m_list.end())
        return *it;
    else
        return out;
}
}
bool STPartyStatisticKey::operator<(const STPartyStatisticKey& other) const
{
    if (m_field0 < other.m_field0) return true;
    if ((short)m_field0 == (short)other.m_field0)
    {
        if (m_field4 < other.m_field4) return true;
        if (m_field4 == other.m_field4)
        {
            if (m_field8 < other.m_field8) return true;
            if (m_field8 == other.m_field8)
            {
                if (m_field9 < other.m_field9) return true;
                if (m_field9 == other.m_field9)
                {
                    if (m_fielda < other.m_fielda) return true;
                    if (m_fielda == other.m_fielda)
                    {
                        if (m_fieldb < other.m_fieldb) return true;
                        if (m_fieldb == other.m_fieldb)
                        {
                            if (m_fieldc < other.m_fieldc) return true;
                            if (m_fieldc == other.m_fieldc &&
                                m_fieldd < other.m_fieldd) return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}
void PartyStatistic::operator+=(const PartyStatistic& other)
{
    m_data[0] += other.m_data[0];
    m_data[1] += other.m_data[1];
    m_data[2] += other.m_data[2];
    m_data[3] += other.m_data[3];
    m_data[4] += other.m_data[4];
    m_data[5] += other.m_data[5];
    m_data[6] += other.m_data[6];
    m_data[7] += other.m_data[7];
    m_data[8] += other.m_data[8];
    m_data[9] += other.m_data[9];
    m_data[10] += other.m_data[10];
    m_data[11] += 1;
}
bool STPartyJobStatisticKey::operator<(const STPartyJobStatisticKey& other) const
{
    if (m_field0 < other.m_field0) return true;
    if (m_field0 == other.m_field0)
    {
        if (m_field4 < other.m_field4) return true;
        if (m_field4 == other.m_field4)
        {
            if (m_field8 < other.m_field8) return true;
            if (m_field8 == other.m_field8)
            {
                if (m_field9 < other.m_field9) return true;
                if (m_field9 == other.m_field9)
                {
                    if (m_fielda < other.m_fielda) return true;
                    if (m_fielda == other.m_fielda)
                    {
                        if (m_fieldb < other.m_fieldb) return true;
                        if (m_fieldb == other.m_fieldb)
                        {
                            if (m_fieldc < other.m_fieldc) return true;
                            if (m_fieldc == other.m_fieldc)
                            {
                            if (m_fieldd < other.m_fieldd)
                                return true;
                                if (m_fieldd == other.m_fieldd)
                                {
                                    if (m_field10 < other.m_field10) return true;
                                    if (m_field10 == other.m_field10 &&
                                        m_field14 < other.m_field14) return true;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return false;
}
void PartyJobStatistic::operator+=(const PartyJobStatistic& other)
{
    m_data[0] += 1;
    m_data[1] += other.m_data[1];
}
bool STPartyCharacKey::operator<(const STPartyCharacKey& other) const
{
    if (m_field0 < other.m_field0) return true;
    if (m_field0 == other.m_field0)
    {
        if (m_field4 < other.m_field4) return true;
        if (m_field4 == other.m_field4)
        {
            if (m_field8 < other.m_field8) return true;
            if (m_field8 == other.m_field8)
            {
                if (m_field9 < other.m_field9)
                    return true;
                if (m_field9 == other.m_field9)
                {
                    if (m_fielda < other.m_fielda)
                        return true;
                    if (m_fielda == other.m_fielda)
                    {
                        if (m_fieldc < other.m_fieldc) return true;
                        if (m_fieldc == other.m_fieldc)
                        {
                            if (m_field10 < other.m_field10)
                                return true;
                            if (m_field10 == other.m_field10 &&
                                m_field11 < other.m_field11)
                                return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}
void PartyCharacStatistic::operator+=(const PartyCharacStatistic& other)
{
    m_data[0] += other.m_data[0];
    m_data[1] += other.m_data[1];
    m_data[2] += other.m_data[2];
    m_data[3] += other.m_data[3];
    m_data[4] += other.m_data[4];
    m_data[5] += other.m_data[5];
    m_data[6] += other.m_data[6];
    m_data[7] += other.m_data[7];
    m_data[8] += other.m_data[8];
    m_data[9] += other.m_data[9];
    m_data[10] += other.m_data[10];
    m_data[11] += 1;
    m_data[12] += other.m_data[12];
}
bool STDeathTowerValueStatisticKey::operator<(const STDeathTowerValueStatisticKey& other) const
{
    if (m_field0 < other.m_field0) return true;
    if (m_field0 == other.m_field0)
    {
        if (m_field2 < other.m_field2) return true;
        if (m_field2 == other.m_field2 &&
            (int)m_field4 < (int)other.m_field4) return true;
    }
    return false;
}
void ValueStatistic::operator+=(const ValueStatistic& other)
{
    m_data[0] += other.m_data[0];
    m_data[1] += 1;
}
bool STDeathTowerPlayDataJobStatisticKey::operator<(
    const STDeathTowerPlayDataJobStatisticKey& other) const
{
    if (m_field0 < other.m_field0) return true;
    if (m_field0 == other.m_field0)
    {
        if (m_field2 < other.m_field2) return true;
        if (m_field2 == other.m_field2)
        {
            if ((int)m_field4 < (int)other.m_field4) return true;
            if (m_field4 == other.m_field4 &&
                m_field8 < other.m_field8) return true;
        }
    }
    return false;
}
void PlayDataJobStatistic::operator+=(const PlayDataJobStatistic& other)
{
    m_data[0] += other.m_data[0];
    m_data[1] += 1;
}
bool STDeathTowerPlayDataPartyStatisticKey::operator<(
    const STDeathTowerPlayDataPartyStatisticKey& other) const
{
    if ((char)m_field0 < (char)other.m_field0) return true;
    if (m_field0 == other.m_field0 && (char)m_field1 < (char)other.m_field1) return true;
    return false;
}
void PlayDataPartyStatistic::operator+=(const PlayDataPartyStatistic& other)
{
    m_data[0] += other.m_data[0];
    m_data[1] += 1;
}
bool STPacketOverflowKey::operator<(const STPacketOverflowKey& other) const
{
    if ((unsigned char)m_field0 < (unsigned char)other.m_field0) return true;
    if (m_field2 < other.m_field2) return true;
    return false;
}
bool STAssertManagerKey::operator<(const STAssertManagerKey& other) const
{
    // ORIG 0x80743c2 实测：平铺链（strcmp != 0 → true；m_field100 < → true；
    // strcmp2 != 0 → true；否则 false），分支极性/布局与 ORIG 对齐。
    if (strcmp(m_str0, other.m_str0) != 0) return true;
    if (m_field100 < other.m_field100) return true;
    if (strcmp(m_str2, other.m_str2) != 0) return true;
    return false;
}
bool STUserTingTimeCheckKey::operator<(const STUserTingTimeCheckKey& other) const
{
    if (m_field0 < other.m_field0) return true;
    return false;
}
bool STHellPartyStatisticItemKey::operator<(const STHellPartyStatisticItemKey& other) const
{
    // ORIG 0x80768b8 实测：嵌套 == 链；m_field0 / m_field4 / m_field8 / m_field9
    // / m_field9（重复）/ m_fielda 比较链（m_field9 在 ORIG 中被比较两次）。
    if (m_field0 < other.m_field0) return true;
    if (m_field0 == other.m_field0)
    {
        if (m_field4 < other.m_field4) return true;
        if (m_field4 == other.m_field4)
        {
            if (m_field8 < other.m_field8) return true;
            if (m_field8 == other.m_field8)
            {
                if (m_field9 < other.m_field9) return true;
                if (m_field9 == other.m_field9)
                {
                    if (m_field9 < other.m_field9) return true;
                    if (m_fielda == other.m_fielda)
                    {
                        if (m_fielda < other.m_fielda) return true;
                    }
                }
            }
        }
    }
    return false;
}
void HellPartyItenmData::operator+=(const HellPartyItenmData& other)
{
    for (int i = 0; i < 6; i++)
    {
        m_data[i] += other.m_data[i];
    }
    m_count += 1;
}
bool STPowerwarFightLoadingKey::operator<(const STPowerwarFightLoadingKey& other) const
{
    if (m_field0 < other.m_field0) return true;
    if (m_field0 == other.m_field0 && m_field4 < other.m_field4) return true;
    return false;
}
bool STPowerwarFightLagKey::operator<(const STPowerwarFightLagKey& other) const
{
    if (m_field0 < other.m_field0) return true;
    if (m_field0 == other.m_field0 && m_field4 < other.m_field4) return true;
    return false;
}
