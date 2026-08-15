#ifndef GM_ACCOUNTS_H_
#define GM_ACCOUNTS_H_

#include <list>
#include <algorithm>
#include <string.h>

#include "PacketHeader.h"

// ==================== 统计数据键/值结构（ORIG GMAccounts.cpp 数据区）====================

// ---- STCubeStatisticKey：0xd ----
struct STCubeStatisticKey
{
    STCubeStatisticKey();
    STCubeStatisticKey(const STCubeStatisticKey& other);
    ~STCubeStatisticKey() throw();
    bool operator<(STCubeStatisticKey other) const;
    int m_channel;          // +0（printStatisticData: channel）
    int m_index;            // +4（printStatisticData: index）
    int m_level;            // +8（printStatisticData: level）
    unsigned char m_type;   // +0xc（printStatisticData: type）
};

// ---- STPartyStatisticKey：0xe ----
struct STPartyStatisticKey
{
    STPartyStatisticKey();
    ~STPartyStatisticKey();
    bool operator<(const STPartyStatisticKey& other) const;
    unsigned short m_channelNo;        // +0（log_dungeon_party.channel_no）
    int m_dungeonIndex;                // +4（log_dungeon_party.dungeon_index）
    char m_dungeonDiff;                // +8（log_dungeon_party.dungeon_diff）
    bool m_dungeonStandardLevel;       // +9
    bool m_abuseParty;                 // +0xa
    bool m_balkunParty;                // +0xb
    bool m_success;                    // +0xc
    char m_partyUserCount;             // +0xd（log_dungeon_party.party_user_count）
};

// ---- PartyStatistic：0x30 ----
struct PartyStatistic
{
    PartyStatistic();
    ~PartyStatistic();
    void Reset();
    void operator+=(const PartyStatistic& other);
    int m_data[12];
};

// ---- STPartyJobStatisticKey：0x15 ----
struct STPartyJobStatisticKey
{
    STPartyJobStatisticKey();
    ~STPartyJobStatisticKey();
    bool operator<(const STPartyJobStatisticKey& other) const;
    unsigned short m_channelNo;        // +0（log_dungeon_party_job.channel_no）
    int m_dungeonIndex;                // +4（log_dungeon_party_job.dungeon_index）
    char m_dungeonDiff;                // +8（log_dungeon_party_job.dungeon_diff）
    char m_dungeonStandardLevel;       // +9（ORIG 按字节拷贝，非 bool setne）
    char m_abuseParty;                 // +0xa
    char m_balkunParty;                // +0xb
    char m_success;                    // +0xc
    char m_partyUserCount;             // +0xd（log_dungeon_party_job.party_user_count）
    enum STPartyJobKeyField10 { ST_PARTY_JOB_KEY_FIELD10_0 = 0 } m_characJob;  // +0x10（log_dungeon_party_job.charac_job）
    char m_characGrow;                // +0x14（log_dungeon_party_job.charac_grow）
};

// ---- PartyJobStatistic：0x8 ----
struct PartyJobStatistic
{
    PartyJobStatistic();
    void reset();
    void operator+=(const PartyJobStatistic& other);
    int m_data[2];
};

// ---- STPartyCharacKey：0x12 ----
struct STPartyCharacKey
{
    STPartyCharacKey();
    ~STPartyCharacKey();
    bool operator<(const STPartyCharacKey& other) const;
    unsigned short m_channelNo;        // +0（log_dungeon_charac.channel_no）
    int m_dungeonIndex;                // +4（log_dungeon_charac.dungeon_index）
    char m_dungeonDiff;                // +8（log_dungeon_charac.dungeon_diff）
    bool m_dungeonStandardLevel;       // +9
    bool m_success;                    // +0xa
    enum STPartyCharacKeyFieldC { ST_PARTY_CHARAC_KEY_FIELDC_0 = 0 } m_characJob;  // +0xc（log_dungeon_charac.charac_job）
    char m_characGrow;                // +0x10（log_dungeon_charac.charac_grow）
    char m_partyUserCount;            // +0x11（log_dungeon_charac.party_user_count）
};

// ---- PartyCharacStatistic：0x34 ----
struct PartyCharacStatistic
{
    PartyCharacStatistic();
    ~PartyCharacStatistic();
    void Reset();
    void operator+=(const PartyCharacStatistic& other);
    int m_data[13];
};

// ---- STDeathTowerValueStatisticKey：0x8 ----
struct STDeathTowerValueStatisticKey
{
    STDeathTowerValueStatisticKey() throw();
    ~STDeathTowerValueStatisticKey() throw();
    bool operator<(const STDeathTowerValueStatisticKey& other) const;
    char m_type;              // +0（log_deathtower_value.type）
    short m_level;            // +2（log_deathtower_value.level）
    unsigned int m_counterIdx;  // +4（log_deathtower_value 计数器下标 try_cnt..repair_price）
};

// ---- ValueStatistic：0x8 ----
struct ValueStatistic
{
    ValueStatistic() throw();
    ~ValueStatistic() throw();
    void Reset() throw();
    void operator+=(const ValueStatistic& other);
    int m_data[2];
};

// ---- STDeathTowerPlayDataJobStatisticKey：0x9 ----
struct STDeathTowerPlayDataJobStatisticKey
{
    STDeathTowerPlayDataJobStatisticKey();
    ~STDeathTowerPlayDataJobStatisticKey();
    bool operator<(const STDeathTowerPlayDataJobStatisticKey& other) const;
    char m_deathTowerType;    // +0（日志 deathTower_type）
    short m_level;            // +2（日志 level）
    unsigned int m_characJob; // +4（log_deathtower_playdata_job.charac_job）
    char m_characGrow;        // +8（log_deathtower_playdata_job.charac_grow）
};

// ---- PlayDataJobStatistic：0x8 ----
struct PlayDataJobStatistic
{
    PlayDataJobStatistic();
    ~PlayDataJobStatistic();
    void Reset();
    void operator+=(const PlayDataJobStatistic& other);
    int m_data[2];
};

// ---- STDeathTowerPlayDataPartyStatisticKey：0x2 ----
struct STDeathTowerPlayDataPartyStatisticKey
{
    STDeathTowerPlayDataPartyStatisticKey();
    ~STDeathTowerPlayDataPartyStatisticKey();
    bool operator<(const STDeathTowerPlayDataPartyStatisticKey& other) const;
    char m_deathTowerType;  // +0（log_deathtower_playdata_party.type）
    char m_partyCount;      // +1（log_deathtower_playdata_party.party_count）
};

// ---- PlayDataPartyStatistic：0x8 ----
struct PlayDataPartyStatistic
{
    PlayDataPartyStatistic();
    ~PlayDataPartyStatistic();
    void Reset();
    void operator+=(const PlayDataPartyStatistic& other);
    int m_data[2];
};

// ---- STPacketOverflowKey：0x4 ----
struct STPacketOverflowKey
{
    STPacketOverflowKey();
    ~STPacketOverflowKey() throw();
    void Reset();
    bool operator<(const STPacketOverflowKey& other) const;
    unsigned char m_packetType;  // +0（packet_overflow.packet_type）
    unsigned short m_packetKind; // +2（packet_overflow.packet_kind）
};

// ---- STAssertManagerKey：0x202 ----
struct STAssertManagerKey
{
    STAssertManagerKey();
    ~STAssertManagerKey();
    void Reset();
    bool operator<(const STAssertManagerKey& other) const;
    char m_fileName[0x100];     // +0（assert_manager.file_name）
    unsigned short m_fileLine;  // +0x100（assert_manager.file_line）
    char m_reason[0x100];       // +0x102（assert_manager.reason）
};

// ---- STUserTingTimeCheckKey：0x4 ----
struct STUserTingTimeCheckKey
{
    STUserTingTimeCheckKey();
    ~STUserTingTimeCheckKey();
    void Reset();
    bool operator<(const STUserTingTimeCheckKey& other) const;
    int m_minute;           // +0（user_ting_timecheck.minute = 秒/0x3c）
};

// ---- STHellPartyStatisticItemKey：0xb ----
struct STHellPartyStatisticItemKey
{
    STHellPartyStatisticItemKey();
    ~STHellPartyStatisticItemKey();
    bool operator<(const STHellPartyStatisticItemKey& other) const;
    bool m_hellpartyType;    // +0
    int m_dungeonIndex;      // +4（log_hellparty_value.dungeon_index）
    char m_dungeonDiff;      // +8（log_hellparty_value.dungeon_diff）
    char m_partyCount;       // +9（log_hellparty_value.party_count）
    char m_hellpartyDiff;    // +0xa（log_hellparty_value.hellparty_diff）
};

// ---- HellPartyItenmData：0x1c ----
struct HellPartyItenmData
{
    HellPartyItenmData();
    ~HellPartyItenmData();
    void Reset();
    void operator+=(const HellPartyItenmData& other);
    int m_count;       // +0（1）
    int m_data[6];     // +4
};

// ---- LoadingTimeReport：0x48 ----
struct LoadingTimeReport
{
    LoadingTimeReport() throw();
    ~LoadingTimeReport() throw();
    void Reset() throw();
    // ORIG：两个 9 元素数组（m_data2 编译为 add $8 + 0x4(%eax,%edx,4) 形态）
    unsigned int m_data[9];   // +0
    unsigned int m_data2[9];  // +0x24
};

// ---- STPowerwarFightLoadingKey：0x6 ----
struct STPowerwarFightLoadingKey
{
    STPowerwarFightLoadingKey();
    ~STPowerwarFightLoadingKey();
    void Reset();
    bool operator<(const STPowerwarFightLoadingKey& other) const;
    unsigned int m_mId;    // +0（powerwar_loading.m_id）
    unsigned short m_round;  // +4（powerwar_loading.round）
};

// ---- STPowerwarFightLoadingData：0x8 ----
struct STPowerwarFightLoadingData
{
    STPowerwarFightLoadingData();
    ~STPowerwarFightLoadingData();
    void Reset();
    unsigned char m_player;   // +0（powerwar_loading.player）
    unsigned short m_myLoading;     // +2（powerwar_loading.my_loading）
    unsigned short m_otherLoading;  // +4（powerwar_loading.other_loading）
    unsigned short m_vsLoading;     // +6（powerwar_loading.vs_loading）
};

// ---- STPowerwarFightLagKey：0x6 ----
struct STPowerwarFightLagKey
{
    STPowerwarFightLagKey();
    ~STPowerwarFightLagKey();
    void Reset();
    bool operator<(const STPowerwarFightLagKey& other) const;
    unsigned int m_mId;    // +0（powerwar_lag.m_id）
    unsigned short m_round;  // +4（powerwar_lag.round）
};

// ---- STPowerwarFightLagData：0xc ----
struct STPowerwarFightLagData
{
    STPowerwarFightLagData();
    ~STPowerwarFightLagData();
    void Reset();
    unsigned char m_player; // +0（powerwar_lag.player）
    unsigned int m_lagAvg;  // +4（powerwar_lag.lag_avg）
    unsigned int m_lagCnt;  // +8（powerwar_lag.lag_cnt）
};

// ---- STFatigueBattery：0x8 ----
struct STFatigueBattery
{
    STFatigueBattery();
    int m_money;  // +0（log_fatigue_battery.money）
    int m_buff;   // +4（log_fatigue_battery.buff）
};

// ---- STModuleLagStatistics：0x10 ----
struct STModuleLagStatistics
{
    STModuleLagStatistics() throw();
    ~STModuleLagStatistics() throw();
    void Reset() throw();
    int m_data[4];
};

// ---- STDungeonLagStatistics：0x20 ----
struct STDungeonLagStatistics
{
    STDungeonLagStatistics();
    ~STDungeonLagStatistics();
    void Reset();
    int m_data[8];
};

// ---- ValueStatisticData：0x78 ----
struct ValueStatisticData
{
    ValueStatisticData();
    int m_data[0x1e];
};

// ---- CirculationStatisticData：0xc0 ----
struct CirculationStatisticData
{
    CirculationStatisticData();
    int m_data[0x30];
};

// ---- stP2PStatistics：0x48 ----
struct stP2PStatistics
{
    stP2PStatistics();
    void Init();
    int m_p2pUser;             // +0x0（p2p_statistics.p2p_user）
    int m_relayUser;           // +0x4（p2p_statistics.relay_user）
    unsigned char m_serverGroup; // +0x8（p2p_statistics.server_group）
    unsigned short m_p2pMinPing; // +0xa（p2p_statistics.p2p_min_ping）
    unsigned short m_p2pMaxPing; // +0xc（p2p_statistics.p2p_max_ping）
    unsigned short m_p2pAvgPing; // +0xe（p2p_statistics.p2p_avg_ping）
    int m_p2pPingSum;            // +0x10（avgPing 累加）
    int m_p2pPingCnt;            // +0x14（avgPing 计数）
    int m_p2pOverPing100;        // +0x18（p2p_statistics.p2p_over_ping_100）
    int m_p2pOverPing200;        // +0x1c（p2p_statistics.p2p_over_ping_200）
    int m_p2pOverPing300;        // +0x20（p2p_statistics.p2p_over_ping_300）
    int m_p2pOverPing400;        // +0x24（p2p_statistics.p2p_over_ping_400）
    unsigned short m_relayMinPing; // +0x28（p2p_statistics.relay_min_ping）
    unsigned short m_relayMaxPing; // +0x2a（p2p_statistics.relay_max_ping）
    unsigned short m_relayAvgPing; // +0x2c（p2p_statistics.relay_avg_ping）
    unsigned short m_field2e;      // +0x2e（无证据，保留）
    int m_relayPingSum;            // +0x30（avgPing 累加）
    int m_relayPingCnt;            // +0x34（avgPing 计数）
    int m_relayOverPing100;        // +0x38（p2p_statistics.relay_over_ping_100）
    int m_relayOverPing200;        // +0x3c（p2p_statistics.relay_over_ping_200）
    int m_relayOverPing300;        // +0x40（p2p_statistics.relay_over_ping_300）
    int m_relayOverPing400;        // +0x44（p2p_statistics.relay_over_ping_400）
};

// ---- stDisjointAvatarInfoTotal：0x144 ----
struct stDisjointAvatarInfoTotal
{
    stDisjointAvatarInfoTotal() throw();
    void clear();
    bool checkCondition(int a, int b, int c);
    void incCount(int a, int b, int c, int d);
    int m_count[3][9][2];  // +0，54 个 int（ORIG 下标 (a*9+b)*2+c）
    int m_sum[3][9];       // +0xd8，27 个 int（ORIG 下标 a*9+b+0x36）
};

// ---- stCreateEmblemStatistic：0x1c ----
struct stCreateEmblemStatistic
{
    void increaseCount(int idx);
    void clear();
    int m_data[7];
};

// ---- SECRET_SHOP_STATISTIC_DATA：0x14 ----
struct SECRET_SHOP_STATISTIC_DATA
{
    SECRET_SHOP_STATISTIC_DATA();
    int m_data[5];
} __attribute__((packed));

// ---- GoldCardEventStatistic：0x9 ----
#pragma pack(push, 1)
struct GoldCardEventStatistic
{
    GoldCardEventStatistic();
    char m_level;   // +0（log_goldcard_event.level）
    unsigned int m_createCnt;  // +1（log_goldcard_event.create_cnt）
    unsigned int m_openCnt;  // +5（log_goldcard_event.open_cnt）
};
#pragma pack(pop)

// ---- TowerOfDespairStatistic_Value：0x8 ----
struct TowerOfDespairStatistic_Value
{
    TowerOfDespairStatistic_Value();
    unsigned int m_success;  // +0（log_tower_despair_layer_stat.success）
    unsigned int m_enter;    // +4（log_tower_despair_layer_stat.enter）
} __attribute__((packed));

// ---- STReasonCrashDownKey / STBloodDungeonStatistic ----
struct STReasonCrashDownKey
{
    bool operator<(const STReasonCrashDownKey& other) const
    {
        return memcmp(this, &other, sizeof(*this)) < 0;
    }
    unsigned int m_occTime;   // +0（log_client_ting_stat.occ_time）
    unsigned int m_channelNo; // +4（log_client_ting_stat.channel_no）
    unsigned int m_reason;    // +8（log_client_ting_stat.reason）
    unsigned int m_cnt;       // +0xc（log_client_ting_stat.cnt）
} __attribute__((packed));

struct STBloodDungeonStatistic
{
    unsigned int m_tryCount;   // +0（log_blood_dungeon.try_count）
    unsigned int m_clearCount; // +4（log_blood_dungeon.clear_count）
};

// ==================== DB 统计上报包（ORIG GMAccounts.cpp 发射）====================
#pragma pack(push, 1)

// ---- PartyStatistic 统计元素 wire 布局（packed，0x3c 字节）----
struct STPartyStatisticWire
{
    unsigned short m_channelNo;        // +0（log_dungeon_party.channel_no）
    int m_dungeonIndex;                // +2（log_dungeon_party.dungeon_index）
    char m_dungeonDiff;                // +6（log_dungeon_party.dungeon_diff）
    unsigned char m_dungeonStandardLevel; // +7（log_dungeon_party.dungeon_standard_level）
    unsigned char m_abuseParty;        // +8（log_dungeon_party.abuse_party）
    unsigned char m_balkunParty;       // +9（log_dungeon_party.balkun_party）
    unsigned char m_success;           // +a（log_dungeon_party.success）
    char m_partyUserCount;             // +b（log_dungeon_party.party_user_count）
    int m_data[12];            // +c
};

class Packet_Goldcard_Event_Statistic_STD : public PacketHeader
{
public:
    Packet_Goldcard_Event_Statistic_STD();
    GoldCardEventStatistic m_items[99];  // +0xa（99 * 0x9）
} __attribute__((packed));

class Packet_TowerOfDespair_Statistic_STD : public PacketHeader
{
public:
    Packet_TowerOfDespair_Statistic_STD();
    int m_countA;                                   // +0xa
    int m_countB;                                   // +0xe
    TowerOfDespairStatistic_Value m_items[101];     // +0x12（101 * 0x8）
} __attribute__((packed));

class Packet_DBMW_Reason_Crash_Down_Query : public PacketHeader
{
public:
    Packet_DBMW_Reason_Crash_Down_Query();
    char m_query[0x100];
} __attribute__((packed));

class Packet_DBMW_Dungeon_Statistic_Party : public PacketHeader
{
public:
    Packet_DBMW_Dungeon_Statistic_Party();
    unsigned int m_count;                 // +0xa
    STPartyStatisticWire m_elem[100];     // +0xe（100 * 0x3c）
} __attribute__((packed));

struct STPartyJobWireItem
{
    unsigned short m_channelNo;        // +0（log_dungeon_party_job.channel_no）
    int m_dungeonIndex;                // +2（log_dungeon_party_job.dungeon_index）
    char m_dungeonDiff;                // +6（log_dungeon_party_job.dungeon_diff）
    unsigned char m_dungeonStandardLevel; // +7（log_dungeon_party_job.dungeon_standard_level）
    unsigned char m_abuseParty;        // +8（log_dungeon_party_job.abuse_party）
    unsigned char m_balkunParty;       // +9（log_dungeon_party_job.balkun_party）
    unsigned char m_success;           // +a（log_dungeon_party_job.success）
    char m_partyUserCount;             // +b（log_dungeon_party_job.party_user_count）
    int m_characJob;                   // +c（log_dungeon_party_job.charac_job）
    char m_characGrow;                 // +0x10（log_dungeon_party_job.charac_grow）
    int m_data[2];             // +0x11
};

struct STPartyCharacWireItem
{
    unsigned short m_channelNo;        // +0（log_dungeon_charac.channel_no）
    int m_dungeonIndex;                // +2（log_dungeon_charac.dungeon_index）
    char m_dungeonDiff;                // +6（log_dungeon_charac.dungeon_diff）
    unsigned char m_dungeonStandardLevel; // +7（log_dungeon_charac.dungeon_standard_level）
    unsigned char m_success;           // +8（log_dungeon_charac.success）
    unsigned int m_characJob;          // +9（log_dungeon_charac.charac_job）
    char m_characGrow;                 // +0xd（log_dungeon_charac.charac_grow）
    char m_partyUserCount;             // +0xe（log_dungeon_charac.party_user_count）
    int m_data[13];            // +0xf
};

struct STDeathTowerValueWireItem
{
    unsigned char m_type;   // +0（log_deathtower_value.type）
    unsigned short m_level; // +1（log_deathtower_value.level）
    unsigned int m_counterIdx; // +3（log_deathtower_value 计数器下标）
    char m_pad[4];          // +7
    int m_value;            // +0xb（对应计数器的增量）
};

struct STDeathTowerPlayDataJobWireItem
{
    char m_deathTowerType;            // +0（log_deathtower_playdata_job.type）
    short m_level;                    // +1（log_deathtower_playdata_job.level）
    unsigned int m_characJob;         // +3（log_deathtower_playdata_job.charac_job）
    char m_characGrow;                // +7（log_deathtower_playdata_job.charac_grow）
    int m_avgClearCount;              // +8（log_deathtower_playdata_job.avg_clear_count）
    int m_playCount;                  // +0xc（log_deathtower_playdata_job.playcount）
};

struct STDeathTowerPlayDataPartyWireItem
{
    char m_deathTowerType;    // +0（log_deathtower_playdata_party.type）
    char m_partyCount;        // +1（log_deathtower_playdata_party.party_count）
    int m_avgClearCount;      // +2（log_deathtower_playdata_party.avg_clear_count）
    int m_playCount;          // +6（ORIG 按 int 读作 playcount）
};

struct STAssertManagerWriteItem
{
    char m_fileName[0x100];       // +0（assert_manager.file_name）
    unsigned short m_fileLine;    // +0x100（assert_manager.file_line）
    int m_count;                  // +0x102（assert_manager.cnt）
    char m_reason[0x100];         // +0x106（assert_manager.reason）
};

struct STUserTingTimeCheckWriteItem
{
    unsigned int m_minute;    // +0（user_ting_timecheck.minute）
    int m_cnt;                // +4（user_ting_timecheck.cnt）
};

class Packet_DBMW_Dungeon_Statistic_Party_Job : public PacketHeader
{
public:
    Packet_DBMW_Dungeon_Statistic_Party_Job();
    unsigned int m_count;                  // +0xa
    STPartyJobWireItem m_items[0xf3];      // +0xe（0xf3 * 0x19）
} __attribute__((packed));

class Packet_DBMW_Dungeon_Statistic_Party_Charac : public PacketHeader
{
public:
    Packet_DBMW_Dungeon_Statistic_Party_Charac();
    unsigned int m_count;                  // +0xa
    STPartyCharacWireItem m_items[0x59];   // +0xe
} __attribute__((packed));

class Packet_DBMW_Packet_Overflow_Statistic : public PacketHeader
{
public:
    Packet_DBMW_Packet_Overflow_Statistic();
    struct __attribute__((packed))
    {
        char m_packetType;            // +0xa（packet_overflow.packet_type）
        unsigned short m_packetKind;  // +0xb（packet_overflow.packet_kind）
        unsigned int m_cnt;           // +0xd（packet_overflow.cnt）
    } m_typed;
} __attribute__((packed));

class Packet_Avater_Disjoint_Statistic_DB : public PacketHeader
{
public:
    Packet_Avater_Disjoint_Statistic_DB();
    stDisjointAvatarInfoTotal m_info;  // +0xa（0x144）
} __attribute__((packed));

class Packet_Emblem_Create_Statistic_DB : public PacketHeader
{
public:
    Packet_Emblem_Create_Statistic_DB();
    stCreateEmblemStatistic m_info;  // +0xa
} __attribute__((packed));

class Packet_Randombox_statistic_DB : public PacketHeader
{
public:
    Packet_Randombox_statistic_DB();
    struct __attribute__((packed))
    {
        unsigned int m_a[5];   // +0xa
        unsigned int m_b[5];   // +0x1e
    } m_typed;
} __attribute__((packed));

class Packet_Server_Match_data_DBMW : public PacketHeader
{
public:
    Packet_Server_Match_data_DBMW();
    char m_serverId;   // +0xa（pvp_score.server_id）
    int m_winCount;    // +0xb（pvp_score.win_count）
    int m_loseCount;   // +0xf（pvp_score.lose_count）
} __attribute__((packed));

class Packet_DBMW_DeathTower_Statistic_Value : public PacketHeader
{
public:
    Packet_DBMW_DeathTower_Statistic_Value();
    unsigned int m_count;                // +0xa
    STDeathTowerValueWireItem m_items[0x197];  // +0xe
} __attribute__((packed));

class Packet_DBMW_Query_String : public PacketHeader
{
public:
    Packet_DBMW_Query_String();
    unsigned int m_queryId;  // +0xa
    char m_query[0x1001];    // +0xe
} __attribute__((packed));

class Packet_DBMW_Fatigue_Battery_Money_Statistic : public PacketHeader
{
public:
    Packet_DBMW_Fatigue_Battery_Money_Statistic();
    struct FatigueBatteryItem
    {
        int m_money;   // log_fatigue_battery.money
        int m_buff;    // log_fatigue_battery.buff
    };
    struct __attribute__((packed))
    {
        FatigueBatteryItem m_items[0x65];
    } m_typed;
} __attribute__((packed));

class Packet_DBMW_User_Ting_TimeCheck_Write_Query : public PacketHeader
{
public:
    Packet_DBMW_User_Ting_TimeCheck_Write_Query();
    unsigned int m_count;                      // +0xa
    STUserTingTimeCheckWriteItem m_items[0x2fe];  // +0xe
} __attribute__((packed));

class Packet_DBMW_DeathTower_Statistic_Playdata_Job : public PacketHeader
{
public:
    Packet_DBMW_DeathTower_Statistic_Playdata_Job();
    unsigned int m_count;                         // +0xa
    STDeathTowerPlayDataJobWireItem m_items[0x17e];  // +0xe
} __attribute__((packed));

class Packet_DBMW_DeathTower_Statistic_Playdata_Party : public PacketHeader
{
public:
    Packet_DBMW_DeathTower_Statistic_Playdata_Party();
    unsigned int m_count;                           // +0xa
    STDeathTowerPlayDataPartyWireItem m_items[0x264];  // +0xe
} __attribute__((packed));

class Packet_Secret_Shop_Statistic : public PacketHeader
{
public:
    Packet_Secret_Shop_Statistic();
    int m_count;                              // +0xa
    int m_shopIdx;                            // +0xe
    SECRET_SHOP_STATISTIC_DATA m_items[200];  // +0x12（200 * 0x14）
} __attribute__((packed));

class Packet_DBMW_Loading_Time_Report : public PacketHeader
{
public:
    Packet_DBMW_Loading_Time_Report();
    char m_group[9];         // +0xa
    unsigned int m_value[9]; // +0x13
} __attribute__((packed));

class Packet_DBMW_Assert_Manager_Info_Write_Query : public PacketHeader
{
public:
    Packet_DBMW_Assert_Manager_Info_Write_Query();
    unsigned int m_count;                  // +0xa
    STAssertManagerWriteItem m_items[9];   // +0xe
} __attribute__((packed));

class Packet_DBMW_Ting_User_TimeCheck_Write_Query : public PacketHeader
{
public:
    Packet_DBMW_Ting_User_TimeCheck_Write_Query();
    struct TingItem
    {
        unsigned int m_id;     // ting_user_account.m_id
        int m_minute;          // ting_user_account.minute
    };
    struct __attribute__((packed))
    {
        unsigned int m_count;        // +0xa
        TingItem m_items[0x2fe];     // +0xe
    } m_typed;
} __attribute__((packed));

class Packet_DBMW_Powerwar_Loading_Time_Report : public PacketHeader
{
public:
    Packet_DBMW_Powerwar_Loading_Time_Report();
    int m_count;             // +0xa
    char m_sql[23][0x100];   // +0xe
} __attribute__((packed));

class Packet_DBMW_Powerwar_Lag_Report : public PacketHeader
{
public:
    Packet_DBMW_Powerwar_Lag_Report();
    int m_count;             // +0xa
    char m_sql[23][0x100];   // +0xe
} __attribute__((packed));

class Packet_DBMW_TechnicalReport_Common_Query : public PacketHeader
{
public:
    Packet_DBMW_TechnicalReport_Common_Query();
    char m_query[0x400];
} __attribute__((packed));

#pragma pack(pop)

// ---- WongWork::CGMAccounts ----
namespace WongWork
{
class CGMAccounts
{
public:
    struct stGMInfo_t
    {
        bool operator==(const stGMInfo_t& other) const;
        unsigned int m_id;    // GM 账号 id（isGM/AppendGM_Sys）
        int m_flag;           // GM flag/index（isGM 用 3）
    };
    void LoadGmList(unsigned int group, int index);
    void clearGmList();
    void AppendGM_Sys(unsigned int id, char flag);
    bool loadGMAccounts(const char* path);
    int isGM(unsigned int id);
    int appendGM(unsigned int id, unsigned int value);
    int removeGM(unsigned int id, unsigned int value);
    stGMInfo_t getGMInfo(unsigned int id) const;
    std::list<stGMInfo_t> m_list;
};
}

#endif // GM_ACCOUNTS_H_
