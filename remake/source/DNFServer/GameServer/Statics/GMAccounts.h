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
    int m_field0;           // +0
    int m_field4;           // +4
    int m_field8;           // +8
    unsigned char m_fieldc; // +0xc
};

// ---- STPartyStatisticKey：0xe ----
struct STPartyStatisticKey
{
    STPartyStatisticKey();
    ~STPartyStatisticKey();
    bool operator<(const STPartyStatisticKey& other) const;
    unsigned short m_field0;  // +0
    int m_field4;             // +4
    char m_field8;            // +8
    bool m_field9;            // +9（ORIG 汇编为 bool 提升比较）
    bool m_fielda;            // +0xa
    bool m_fieldb;            // +0xb
    bool m_fieldc;            // +0xc
    char m_fieldd;            // +0xd
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
    unsigned short m_field0;  // +0
    int m_field4;             // +4
    char m_field8;            // +8
    bool m_field9;            // +9（ORIG 汇编为 bool 提升比较）
    bool m_fielda;            // +0xa
    bool m_fieldb;            // +0xb
    bool m_fieldc;            // +0xc
    char m_fieldd;            // +0xd
    enum STPartyJobKeyField10 { ST_PARTY_JOB_KEY_FIELD10_0 = 0 } m_field10;  // +0x10（ORIG 汇编 enum 形态）
    char m_field14;           // +0x14
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
    unsigned short m_field0;  // +0
    int m_field4;             // +4
    char m_field8;            // +8
    bool m_field9;            // +9（ORIG 汇编为 bool 提升比较）
    bool m_fielda;            // +0xa
    enum STPartyCharacKeyFieldC { ST_PARTY_CHARAC_KEY_FIELDC_0 = 0 } m_fieldc;  // +0xc（ORIG 汇编 enum 形态）
    char m_field10;           // +0x10
    char m_field11;           // +0x11
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
    char m_field0;            // +0
    short m_field2;           // +2
    unsigned int m_field4;    // +4
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
    char m_field0;            // +0
    short m_field2;           // +2
    unsigned int m_field4;    // +4
    char m_field8;            // +8
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
    char m_field0;  // +0
    char m_field1;  // +1
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
    unsigned char m_field0;   // +0
    unsigned short m_field2; // +2
};

// ---- STAssertManagerKey：0x202 ----
struct STAssertManagerKey
{
    STAssertManagerKey();
    ~STAssertManagerKey();
    void Reset();
    bool operator<(const STAssertManagerKey& other) const;
    char m_str0[0x100];     // +0
    unsigned short m_field100;  // +0x100
    char m_str2[0x100];     // +0x102
};

// ---- STUserTingTimeCheckKey：0x4 ----
struct STUserTingTimeCheckKey
{
    STUserTingTimeCheckKey();
    ~STUserTingTimeCheckKey();
    void Reset();
    bool operator<(const STUserTingTimeCheckKey& other) const;
    int m_field0;           // +0
};

// ---- STHellPartyStatisticItemKey：0xb ----
struct STHellPartyStatisticItemKey
{
    STHellPartyStatisticItemKey();
    ~STHellPartyStatisticItemKey();
    bool operator<(const STHellPartyStatisticItemKey& other) const;
    bool m_field0;           // +0（ORIG 汇编为 bool 提升比较，双零扩展+32 位 cmp）
    int m_field4;            // +4
    char m_field8;           // +8
    char m_field9;           // +9
    char m_fielda;           // +0xa
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
    unsigned int m_field0;    // +0
    unsigned short m_field4;  // +4
};

// ---- STPowerwarFightLoadingData：0x8 ----
struct STPowerwarFightLoadingData
{
    STPowerwarFightLoadingData();
    ~STPowerwarFightLoadingData();
    void Reset();
    unsigned char m_field0;   // +0
    unsigned short m_field2;  // +2
    unsigned short m_field4;  // +4
    unsigned short m_field6;  // +6
};

// ---- STPowerwarFightLagKey：0x6 ----
struct STPowerwarFightLagKey
{
    STPowerwarFightLagKey();
    ~STPowerwarFightLagKey();
    void Reset();
    bool operator<(const STPowerwarFightLagKey& other) const;
    unsigned int m_field0;    // +0
    unsigned short m_field4;  // +4
};

// ---- STPowerwarFightLagData：0xc ----
struct STPowerwarFightLagData
{
    STPowerwarFightLagData();
    ~STPowerwarFightLagData();
    void Reset();
    unsigned char m_field0; // +0
    unsigned int m_field4;  // +4
    unsigned int m_field8;  // +8
};

// ---- STFatigueBattery：0x8 ----
struct STFatigueBattery
{
    STFatigueBattery();
    int m_field0;  // +0
    int m_field4;  // +4
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
    int m_field0;              // +0x0
    int m_field4;              // +0x4
    unsigned char m_field8;    // +0x8
    unsigned short m_fieldA;   // +0xa
    unsigned short m_fieldC;   // +0xc
    unsigned short m_fieldE;   // +0xe
    int m_field10;             // +0x10
    int m_field14;             // +0x14
    int m_field18;             // +0x18
    int m_field1c;             // +0x1c
    int m_field20;             // +0x20
    int m_field24;             // +0x24
    unsigned short m_fieldB;   // +0x28
    unsigned short m_field2a;  // +0x2a
    unsigned short m_field2c;  // +0x2c
    unsigned short m_field2e;  // +0x2e
    int m_field30;             // +0x30
    int m_field34;             // +0x34
    int m_field38;             // +0x38
    int m_field3c;             // +0x3c
    int m_field40;             // +0x40
    int m_field44;             // +0x44
};

// ---- stDisjointAvatarInfoTotal：0x144 ----
struct stDisjointAvatarInfoTotal
{
    stDisjointAvatarInfoTotal() throw();
    void clear();
    bool checkCondition(int a, int b, int c);
    void incCount(int a, int b, int c, int d);
    int m_data[0x51];   // 0x144 字节（ORIG 按 int 索引寻址）
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
};

// ---- GoldCardEventStatistic：0x9 ----
#pragma pack(push, 1)
struct GoldCardEventStatistic
{
    GoldCardEventStatistic();
    char m_field0;   // +0
    unsigned int m_field1;  // +1
    unsigned int m_field5;  // +5
};
#pragma pack(pop)

// ---- TowerOfDespairStatistic_Value：0x8 ----
struct TowerOfDespairStatistic_Value
{
    TowerOfDespairStatistic_Value();
    unsigned int m_field0;  // +0
    unsigned int m_field4;  // +4
} __attribute__((packed));

// ---- STReasonCrashDownKey / STBloodDungeonStatistic ----
struct STReasonCrashDownKey
{
    bool operator<(const STReasonCrashDownKey& other) const
    {
        return memcmp(this, &other, sizeof(*this)) < 0;
    }
    char m_data[0x10];
};

struct STBloodDungeonStatistic
{
    unsigned int m_field0;  // +0
    unsigned int m_field4;  // +4
};

// ==================== DB 统计上报包（ORIG GMAccounts.cpp 发射）====================
#pragma pack(push, 1)

// ---- PartyStatistic 统计元素 wire 布局（packed，0x3c 字节）----
struct STPartyStatisticWire
{
    unsigned short m_field0;   // +0
    int m_field4;              // +2
    char m_field8;             // +6
    unsigned char m_field9;    // +7
    unsigned char m_fielda;    // +8
    unsigned char m_fieldb;    // +9
    unsigned char m_fieldc;    // +a
    char m_fieldd;             // +b
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
    char m_data[0x100];
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
    unsigned short m_field0;   // +0
    int m_field4;              // +2
    char m_field8;             // +6
    unsigned char m_field9;    // +7
    unsigned char m_fielda;    // +8
    unsigned char m_fieldb;    // +9
    unsigned char m_fieldc;    // +a
    char m_fieldd;             // +b
    int m_field10;             // +c
    char m_field14;            // +0x10
    int m_data[2];             // +0x11
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
    char m_data[0x174f];
} __attribute__((packed));

class Packet_DBMW_Packet_Overflow_Statistic : public PacketHeader
{
public:
    Packet_DBMW_Packet_Overflow_Statistic();
    union
    {
        char m_data[0x7];
        struct __attribute__((packed))
        {
            char m_field0;            // +0xa
            unsigned short m_field2;  // +0xb
            unsigned int m_field4;    // +0xd
        } m_typed;
    };
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
    char m_data[0x1c];
} __attribute__((packed));

class Packet_Randombox_statistic_DB : public PacketHeader
{
public:
    Packet_Randombox_statistic_DB();
    union
    {
        char m_data[0x28];
        struct __attribute__((packed))
        {
            unsigned int m_a[5];   // +0xa
            unsigned int m_b[5];   // +0x1e
        } m_typed;
    };
} __attribute__((packed));

class Packet_Server_Match_data_DBMW : public PacketHeader
{
public:
    Packet_Server_Match_data_DBMW();
    char m_fieldA;   // +0xa
    int m_fieldB;    // +0xb
    int m_fieldC;    // +0xf
} __attribute__((packed));

class Packet_DBMW_DeathTower_Statistic_Value : public PacketHeader
{
public:
    Packet_DBMW_DeathTower_Statistic_Value();
    char m_data[0x17dd];
} __attribute__((packed));

class Packet_DBMW_Query_String : public PacketHeader
{
public:
    Packet_DBMW_Query_String();
    unsigned int m_queryId;  // +0xa
    char m_data[0x1001];     // +0xe
} __attribute__((packed));

class Packet_DBMW_Fatigue_Battery_Money_Statistic : public PacketHeader
{
public:
    Packet_DBMW_Fatigue_Battery_Money_Statistic();
    struct FatigueBatteryItem
    {
        int m_field0;
        int m_field4;
    };
    union
    {
        char m_data[0x328];
        struct __attribute__((packed))
        {
            FatigueBatteryItem m_items[0x65];
        } m_typed;
    };
} __attribute__((packed));

class Packet_DBMW_User_Ting_TimeCheck_Write_Query : public PacketHeader
{
public:
    Packet_DBMW_User_Ting_TimeCheck_Write_Query();
    char m_data[0x17f4];
} __attribute__((packed));

class Packet_DBMW_DeathTower_Statistic_Playdata_Job : public PacketHeader
{
public:
    Packet_DBMW_DeathTower_Statistic_Playdata_Job();
    char m_data[0x17e4];
} __attribute__((packed));

class Packet_DBMW_DeathTower_Statistic_Playdata_Party : public PacketHeader
{
public:
    Packet_DBMW_DeathTower_Statistic_Playdata_Party();
    char m_data[0x17ec];
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
    char m_data[0x123a];
} __attribute__((packed));

class Packet_DBMW_Ting_User_TimeCheck_Write_Query : public PacketHeader
{
public:
    Packet_DBMW_Ting_User_TimeCheck_Write_Query();
    struct TingItem
    {
        unsigned int m_field0;
        int m_field4;
    };
    union
    {
        char m_data[0x17f4];
        struct __attribute__((packed))
        {
            unsigned int m_count;        // +0xa
            TingItem m_items[0x2fe];     // +0xe
        } m_typed;
    };
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
    char m_data[0x400];
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
        unsigned int m_field0;
        int m_field1;
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
