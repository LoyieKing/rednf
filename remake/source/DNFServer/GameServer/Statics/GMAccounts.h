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
    ~STPartyStatisticKey() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
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
    PartyStatistic() throw();  // [2026-09-15 P216] 消调用者冗余 landing pad
    ~PartyStatistic() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
    void Reset();
    void operator+=(const PartyStatistic& other);
    int m_data[12];
};

// ---- STPartyJobStatisticKey：0x15 ----
struct STPartyJobStatisticKey
{
    STPartyJobStatisticKey();
    ~STPartyJobStatisticKey() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
    bool operator<(const STPartyJobStatisticKey& other) const;
    unsigned short m_channelNo;        // +0（log_dungeon_party_job.channel_no）
    int m_dungeonIndex;                // +4（log_dungeon_party_job.dungeon_index）
    char m_dungeonDiff;                // +8（log_dungeon_party_job.dungeon_diff）
    bool m_dungeonStandardLevel;       // +9
    bool m_abuseParty;                 // +0xa
    bool m_balkunParty;                // +0xb
    bool m_success;                    // +0xc
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
    ~STPartyCharacKey() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
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
    PartyCharacStatistic() throw();  // [2026-09-15 P216] 消调用者冗余 landing pad
    ~PartyCharacStatistic() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
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
    ~STDeathTowerPlayDataJobStatisticKey() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
    bool operator<(const STDeathTowerPlayDataJobStatisticKey& other) const;
    char m_deathTowerType;    // +0（日志 deathTower_type）
    short m_level;            // +2（日志 level）
    unsigned int m_characJob; // +4（log_deathtower_playdata_job.charac_job）
    char m_characGrow;        // +8（log_deathtower_playdata_job.charac_grow）
};

// ---- PlayDataJobStatistic：0x8 ----
struct PlayDataJobStatistic
{
    PlayDataJobStatistic() throw();  // [2026-09-15 P216] 消调用者冗余 landing pad（与 dtor 同规格）
    ~PlayDataJobStatistic() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
    void Reset();
    void operator+=(const PlayDataJobStatistic& other);
    int m_data[2];
};

// ---- STDeathTowerPlayDataPartyStatisticKey：0x2 ----
struct STDeathTowerPlayDataPartyStatisticKey
{
    STDeathTowerPlayDataPartyStatisticKey();
    ~STDeathTowerPlayDataPartyStatisticKey() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
    bool operator<(const STDeathTowerPlayDataPartyStatisticKey& other) const;
    char m_deathTowerType;  // +0（log_deathtower_playdata_party.type）
    char m_partyCount;      // +1（log_deathtower_playdata_party.party_count）
};

// ---- PlayDataPartyStatistic：0x8 ----
struct PlayDataPartyStatistic
{
    PlayDataPartyStatistic() throw();  // [2026-09-15 P216] 消调用者冗余 landing pad
    ~PlayDataPartyStatistic() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
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
    ~STAssertManagerKey() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
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
    ~STUserTingTimeCheckKey() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
    void Reset();
    bool operator<(const STUserTingTimeCheckKey& other) const;
    int m_minute;           // +0（user_ting_timecheck.minute = 秒/0x3c）
};

// ---- STHellPartyStatisticItemKey：0xb ----
struct STHellPartyStatisticItemKey
{
    STHellPartyStatisticItemKey();
    ~STHellPartyStatisticItemKey() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
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
    HellPartyItenmData() throw();  // [2026-09-15 P216] 消调用者冗余 landing pad
    ~HellPartyItenmData() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
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
    ~STPowerwarFightLoadingKey() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
    void Reset();
    bool operator<(const STPowerwarFightLoadingKey& other) const;
    unsigned int m_mId;    // +0（powerwar_loading.m_id）
    unsigned short m_round;  // +4（powerwar_loading.round）
};

// ---- STPowerwarFightLoadingData：0x8 ----
struct STPowerwarFightLoadingData
{
    STPowerwarFightLoadingData();
    ~STPowerwarFightLoadingData() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
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
    ~STPowerwarFightLagKey() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
    void Reset();
    bool operator<(const STPowerwarFightLagKey& other) const;
    unsigned int m_mId;    // +0（powerwar_lag.m_id）
    unsigned short m_round;  // +4（powerwar_lag.round）
};

// ---- STPowerwarFightLagData：0xc ----
struct STPowerwarFightLagData
{
    STPowerwarFightLagData();
    ~STPowerwarFightLagData() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
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
    // [2026-09-15 C2] 原 `int m_data[4]`：ORIG 对 4 字段是「add $0x37 + shl $0x4」再各自位移，
    //   数组下标形会被折进索引常量锚点 ⇒ 每字段换锚。
    int m_data0;   // +0
    int m_data1;   // +4
    int m_data2;   // +8
    int m_data3;   // +0xc
};

// ---- STDungeonLagStatistics：0x20 ----
struct STDungeonLagStatistics
{
    STDungeonLagStatistics();
    ~STDungeonLagStatistics() throw();  // 与 ORIG 的 EH 区块布局一致（dtor 视为 nothrow）
    void Reset();
    // [2026-09-15 StaticsDiff G/C 轴] 保持 unsigned（Reset/ctor 与 ORIG 的 divl 组一致）。
    //   SendDBLagStatistics 里 m_data[2]/m_data[6] 两处 ORIG 为 `sar+idivl`（有符号）：
    //   **`(int)` cast 会插入转换节点** ⇒ `mov mem,%eax; mov %eax,%ebx` 中转（实测 ndiff=6，
    //   长度 304 vs ORIG 302）；字段整体改 int 又让另外 4 处 unsigned 除法中转（ndiff=9）
    //   ⇒ 二者不可兼得。**补 int 视图成员**（无转换节点直取 ⇒ `mov mem,%ebx` 直落）：
    //   微实验 e2/e3 复现 ORIG 形态（`movl 8(%eax),%ebx` + idivl，同 union 内 unsigned
    //   访问仍直落）✓。
    union
    {
        unsigned int m_data[8];
        int m_dataInt[8];   // 仅用于 ORIG 的两处有符号除法（同址视图）
    };
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
    // [W11-Statics 机理·union 视图] AddSecretShopStatistic 的 ORIG 读点形态为
    //   `add $0x10,%eax; mov 0xe(%eax),%eax`（锚恒定 0x10 + disp 随字段递增
    //   0x6/0xa/0xe），而我方 `int m_data[5]` + `m_data[k]` 会把 4k 折进锚
    //   （`add $0x1c; mov 0x2(%eax)`）。探针实证：同一 packed+ctor 类型下，
    //   **具名标量成员**给出 ORIG 形态，数组下标形不给。
    //   两侧需求相反（读点要标量、map 累加点要 `m_data[k] +=`，而 packed 字段
    //   不能绑定 int&）⇒ 用匿名 union 同时暴露两种视图：布局不变（sizeof 0x14、
    //   各字段 +0x0/+0x4/+0x8/+0xc/+0x10，探针 offsetof 实证）。
    //   字段语义 = DB log_secret_shop 列。
    union
    {
        struct
        {
            int m_dungeonIdx;       // +0x0（map 键）
            int m_showCount;        // +0x4
            int m_showCharacCount;  // +0x8
            int m_buyCount;         // +0xc
            int m_price;            // +0x10
        };
        int m_data[5];
    };
} __attribute__((packed));

// ---- GoldCardEventStatistic：0x9 ----
// §4.4 合并：唯一声明点 = Game/GoldCardEventStatistic.h（本头原副本删除，
// 与同时包含两头的 TU（CPartyDepsD2.cpp 等）冲突）。布局一致：packed、
// sizeof 0x9、字段 +0x00/+0x01/+0x05；C1 为头内 inline（ORIG W 0x85bee76）。
#include "../Game/GoldCardEventStatistic.h"

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
    // [2026-09-15 W10-Static 机理 W-A] 原 `int m_data[12]`：数组下标形会被 4.4 折进
    //   索引常量锚点（每字段换锚：`sub $0x1782/0x177e/0x1776/...` + disp 恒 0x1a），
    //   而 ORIG 锚恒定（`sub $0x177a`）+ disp 递增（0xe/0x12/0x16/...）⇒ 独立标量成员
    //   （同 STPartyJobWireItem 的 m_data0/m_data1 先例，布局/sizeof 0x3c 不变）。
    int m_data0;               // +0xc
    int m_data1;               // +0x10
    int m_data2;               // +0x14
    int m_data3;               // +0x18
    int m_data4;               // +0x1c
    int m_data5;               // +0x20
    int m_data6;               // +0x24
    int m_data7;               // +0x28
    int m_data8;               // +0x2c
    int m_data9;               // +0x30
    int m_data10;              // +0x34
    int m_data11;              // +0x38
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
    // ORIG 0x806f6xx 实证：+0x11/+0x15 两字段是**独立标量成员**（ORIG 只把 +4 折进 disp，
    // 数组下标形会被 4.4 折进索引常量锚点 ⇒ 每字段换锚，DIFF）。布局/sizeof 不变。
    int m_data0;               // +0x11
    int m_data1;               // +0x15
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
    // [2026-09-15 C2/Scalarize] 原 `int m_data[13]`。ORIG 对 +0x11/+0x15/+0x19… 是独立标量成员
    // （统一 `sub $0x1765` + 各自位移），数组下标形会被折进索引常量锚点 ⇒ 每字段换锚（DIFF）。
    // 布局/sizeof 不变：0xf + 13*4 = 0x43。先例 = 同头 STPartyJobWireItem。
    int m_data0;               // +0xf
    int m_data1;               // +0x13
    int m_data2;               // +0x17
    int m_data3;               // +0x1b
    int m_data4;               // +0x1f
    int m_data5;               // +0x23
    int m_data6;               // +0x27
    int m_data7;               // +0x2b
    int m_data8;               // +0x2f
    int m_data9;               // +0x33
    int m_data10;              // +0x37
    int m_data11;              // +0x3b
    int m_data12;              // +0x3f
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
// §4.4 合并：本头为跨域唯一声明点（CGuildServerProxy.h 内重复定义已删除，
// 改为 include 本头）。接口/字段名以 Game 域实际消费方
// （Gm_List.cpp/GlobalData.cpp/W6_SecMiscDeps.cpp/WongWorkSkillGM_A.cpp +
// CUser.cpp/DisPatcherCore.cpp 等 isGM 调用点）为准；sizeof=8、
// m_gmList@+0x00、全部 mangled 不变。
namespace WongWork
{
class CGMAccounts
{
public:
    struct stGMInfo_t
    {
        unsigned int mid;    // +0x00（GM 账号 id）
        unsigned int type;   // +0x04（GM flag/index，isGM 用 3）

        // ORIG W 0x08109610：仅比较 mid（+0x00），不比较 type。
        bool operator==(const stGMInfo_t& o) const
        {
            return mid == o.mid;
        }
    };

    CGMAccounts();                         // 0x082a73a4 W（独立 TU）
    ~CGMAccounts();                        // 0x082a73b8 W（独立 TU）
    bool isGM(unsigned int mid);           // 0x08109346
    int removeGM(unsigned int mid, unsigned int param);   // 0x081093fc（mov $0,%eax 返 0）
    // ORIG 0x082a7388 W：mov this→eax 直返 ⇒ 返回 m_gmList（+0x00）引用，非拷贝
    const std::list<stGMInfo_t>& GetGMList() const { return m_gmList; }
    void LoadGmList(unsigned int mid, int level);   // 0x081094ea T
    void clearGmList();                             // 0x08109510 T
    int loadGMAccounts(char const* path);           // 0x0810933c T（恒返 1）
    int appendGM(unsigned int mid, unsigned int type);   // 0x081093ee T
    void AppendGM_Sys(unsigned int mid, char type);      // 0x08109524 T
    // ORIG 0x0810940a _ZNK8WongWork11CGMAccounts9getGMInfoEj：sret；{mid,type=3}
    stGMInfo_t getGMInfo(unsigned int mid) const;
private:
    std::list<stGMInfo_t> m_gmList;        // +0x00（sizeof 8，与 ORIG 一致）
};
}

#endif // GM_ACCOUNTS_H_
