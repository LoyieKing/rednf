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
                m_count[i][j][k] = 0;
            }
            m_sum[i][j] = 0;
        }
    }
}
void stDisjointAvatarInfoTotal::incCount(int a, int b, int c, int d)
{
    if (checkCondition(a, b, c))
    {
        ++m_count[a][b][c];
        m_sum[a][b] = m_sum[a][b] + d;
    }
}
bool stDisjointAvatarInfoTotal::checkCondition(int a, int b, int c)
{
    if (a < 0 || 2 < a) return 0;
    if (b < 0 || 8 < b) return 0;
    if (c < 0 || 1 < c) return 0;
    return 1;
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
    m_serverId = 0;
    m_winCount = 0;
    m_loseCount = 0;
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
    m_level = 0;
    m_createCnt = 0;
    m_openCnt = 0;
}
Packet_Goldcard_Event_Statistic_STD::Packet_Goldcard_Event_Statistic_STD()
    : PacketHeader(0x1f47, 0x385)
{
    memset(m_items, 0, 0x37b);
}
TowerOfDespairStatistic_Value::TowerOfDespairStatistic_Value()
{
    m_success = 0;
    m_enter = 0;
}
Packet_TowerOfDespair_Statistic_STD::Packet_TowerOfDespair_Statistic_STD()
    : PacketHeader(0x271d, 0x33a), m_countA(0), m_countB(0)
{
}
STPartyStatisticKey::STPartyStatisticKey()
{
    m_channelNo = 0;
    m_dungeonIndex = 0;
    m_dungeonDiff = 0;
    m_dungeonStandardLevel = 0;
    m_abuseParty = 0;
    m_balkunParty = 0;
    m_success = 0;
    m_partyUserCount = 0;
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
    m_channelNo = 0;
    m_dungeonIndex = 0;
    m_dungeonDiff = 0;
    m_dungeonStandardLevel = 0;
    m_abuseParty = 0;
    m_balkunParty = 0;
    m_success = 0;
    m_partyUserCount = 0;
    m_characJob = (STPartyJobKeyField10)0;
    m_characGrow = 0;
}
STPartyJobStatisticKey::~STPartyJobStatisticKey() throw()
{
}
STPartyCharacKey::STPartyCharacKey()
{
    m_channelNo = 0;
    m_dungeonIndex = 0;
    m_dungeonDiff = 0;
    m_dungeonStandardLevel = 0;
    m_success = 0;
    m_characJob = (STPartyCharacKeyFieldC)0;
    m_characGrow = 0;
    m_partyUserCount = 0;
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
    m_type = 0;
    m_level = 0;
    m_counterIdx = 0;
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
    m_deathTowerType = 0;
    m_level = 0;
    m_characJob = 0;
    m_characGrow = 0;
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
    m_deathTowerType = 0;
    m_partyCount = 0;
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
    m_packetType = 0;
    m_packetKind = 0;
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
    memset(m_fileName, 0, 0x100);
    m_fileLine = 0;
    memset(m_reason, 0, 0x100);
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
    m_minute = 0;
}
STHellPartyStatisticItemKey::STHellPartyStatisticItemKey()
{
    m_hellpartyType = 0;
    m_dungeonIndex = 0;
    m_partyCount = 0;
    m_dungeonDiff = 0;
    m_hellpartyDiff = 0;
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
    m_mId = 0;
    m_round = 0;
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
    m_player = 0;
    m_myLoading = 0;
    m_otherLoading = 0;
    m_vsLoading = 0;
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
    m_mId = 0;
    m_round = 0;
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
    m_player = 0;
    m_lagAvg = 0;
    m_lagCnt = 0;
}
STFatigueBattery::STFatigueBattery()
{
    m_money = 0;
    m_buff = 0;
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
    m_p2pMinPing = 0x7fff;
    m_relayMinPing = 0x7fff;
}
namespace WongWork
{
bool CGMAccounts::stGMInfo_t::operator==(const stGMInfo_t& other) const
{
    return m_id == other.m_id;
}

void CGMAccounts::LoadGmList(unsigned int group, int index)
{
    stGMInfo_t info;
    info.m_id = group;
    info.m_flag = index;
    m_list.push_back(info);
}

void CGMAccounts::clearGmList()
{
    m_list.clear();
}

void CGMAccounts::AppendGM_Sys(unsigned int id, char flag)
{
    stGMInfo_t info = {};
    info.m_id = id;
    info.m_flag = flag;
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
    key.m_flag = 3;
    key.m_id = id;
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
    out.m_id = 0;
    out.m_flag = 3;
    stGMInfo_t key = {};
    key.m_flag = 3;
    key.m_id = id;
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
    if (m_channelNo < other.m_channelNo) return true;
    if ((short)m_channelNo == (short)other.m_channelNo)
    {
        if (m_dungeonIndex < other.m_dungeonIndex) return true;
        if (m_dungeonIndex == other.m_dungeonIndex)
        {
            if (m_dungeonDiff < other.m_dungeonDiff) return true;
            if (m_dungeonDiff == other.m_dungeonDiff)
            {
                if (m_dungeonStandardLevel < other.m_dungeonStandardLevel) return true;
                if (m_dungeonStandardLevel == other.m_dungeonStandardLevel)
                {
                    if (m_abuseParty < other.m_abuseParty) return true;
                    if (m_abuseParty == other.m_abuseParty)
                    {
                        if (m_balkunParty < other.m_balkunParty) return true;
                        if (m_balkunParty == other.m_balkunParty)
                        {
                            if (m_success < other.m_success) return true;
                            if (m_success == other.m_success &&
                                m_partyUserCount < other.m_partyUserCount) return true;
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
    if (m_channelNo < other.m_channelNo) return true;
    if (m_channelNo == other.m_channelNo)
    {
        if (m_dungeonIndex < other.m_dungeonIndex) return true;
        if (m_dungeonIndex == other.m_dungeonIndex)
        {
            if (m_dungeonDiff < other.m_dungeonDiff) return true;
            if (m_dungeonDiff == other.m_dungeonDiff)
            {
                if (m_dungeonStandardLevel < other.m_dungeonStandardLevel) return true;
                if (m_dungeonStandardLevel == other.m_dungeonStandardLevel)
                {
                    if (m_abuseParty < other.m_abuseParty) return true;
                    if (m_abuseParty == other.m_abuseParty)
                    {
                        if (m_balkunParty < other.m_balkunParty) return true;
                        if (m_balkunParty == other.m_balkunParty)
                        {
                            if (m_success < other.m_success) return true;
                            if (m_success == other.m_success)
                            {
                            if (m_partyUserCount < other.m_partyUserCount)
                                return true;
                                if (m_partyUserCount == other.m_partyUserCount)
                                {
                                    if (m_characJob < other.m_characJob) return true;
                                    if (m_characJob == other.m_characJob &&
                                        m_characGrow < other.m_characGrow) return true;
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
    if (m_channelNo < other.m_channelNo) return true;
    if (m_channelNo == other.m_channelNo)
    {
        if (m_dungeonIndex < other.m_dungeonIndex) return true;
        if (m_dungeonIndex == other.m_dungeonIndex)
        {
            if (m_dungeonDiff < other.m_dungeonDiff) return true;
            if (m_dungeonDiff == other.m_dungeonDiff)
            {
                if (m_dungeonStandardLevel < other.m_dungeonStandardLevel)
                    return true;
                if (m_dungeonStandardLevel == other.m_dungeonStandardLevel)
                {
                    if (m_success < other.m_success)
                        return true;
                    if (m_success == other.m_success)
                    {
                        if (m_characJob < other.m_characJob) return true;
                        if (m_characJob == other.m_characJob)
                        {
                            if (m_characGrow < other.m_characGrow)
                                return true;
                            if (m_characGrow == other.m_characGrow &&
                                m_partyUserCount < other.m_partyUserCount)
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
    if (m_type < other.m_type) return true;
    if (m_type == other.m_type)
    {
        if (m_level < other.m_level) return true;
        if (m_level == other.m_level &&
            (int)m_counterIdx < (int)other.m_counterIdx) return true;
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
    if (m_deathTowerType < other.m_deathTowerType) return true;
    if (m_deathTowerType == other.m_deathTowerType)
    {
        if (m_level < other.m_level) return true;
        if (m_level == other.m_level)
        {
            if ((int)m_characJob < (int)other.m_characJob) return true;
            if (m_characJob == other.m_characJob &&
                m_characGrow < other.m_characGrow) return true;
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
    if ((char)m_deathTowerType < (char)other.m_deathTowerType) return true;
    if (m_deathTowerType == other.m_deathTowerType && (char)m_partyCount < (char)other.m_partyCount) return true;
    return false;
}
void PlayDataPartyStatistic::operator+=(const PlayDataPartyStatistic& other)
{
    m_data[0] += other.m_data[0];
    m_data[1] += 1;
}
bool STPacketOverflowKey::operator<(const STPacketOverflowKey& other) const
{
    if ((unsigned char)m_packetType < (unsigned char)other.m_packetType) return true;
    if (m_packetKind < other.m_packetKind) return true;
    return false;
}
bool STAssertManagerKey::operator<(const STAssertManagerKey& other) const
{
    // ORIG 0x80743c2 实测：平铺链（strcmp != 0 → true；m_fileLine < → true；
    // strcmp2 != 0 → true；否则 false），分支极性/布局与 ORIG 对齐。
    if (strcmp(m_fileName, other.m_fileName) != 0) return true;
    if (m_fileLine < other.m_fileLine) return true;
    if (strcmp(m_reason, other.m_reason) != 0) return true;
    return false;
}
bool STUserTingTimeCheckKey::operator<(const STUserTingTimeCheckKey& other) const
{
    if (m_minute < other.m_minute) return true;
    return false;
}
bool STHellPartyStatisticItemKey::operator<(const STHellPartyStatisticItemKey& other) const
{
    // ORIG 0x80768b8 实测：嵌套 == 链；m_hellpartyType / m_dungeonIndex / m_dungeonDiff / m_partyCount
    // / m_partyCount（重复）/ m_hellpartyDiff 比较链（m_partyCount 在 ORIG 中被比较两次）。
    if (m_hellpartyType < other.m_hellpartyType) return true;
    if (m_hellpartyType == other.m_hellpartyType)
    {
        if (m_dungeonIndex < other.m_dungeonIndex) return true;
        if (m_dungeonIndex == other.m_dungeonIndex)
        {
            if (m_dungeonDiff < other.m_dungeonDiff) return true;
            if (m_dungeonDiff == other.m_dungeonDiff)
            {
                if (m_partyCount < other.m_partyCount) return true;
                if (m_partyCount == other.m_partyCount)
                {
                    if (m_partyCount < other.m_partyCount) return true;
                    if (m_hellpartyDiff == other.m_hellpartyDiff)
                    {
                        if (m_hellpartyDiff < other.m_hellpartyDiff) return true;
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
    if (m_mId < other.m_mId) return true;
    if (m_mId == other.m_mId && m_round < other.m_round) return true;
    return false;
}
bool STPowerwarFightLagKey::operator<(const STPowerwarFightLagKey& other) const
{
    if (m_mId < other.m_mId) return true;
    if (m_mId == other.m_mId && m_round < other.m_round) return true;
    return false;
}
