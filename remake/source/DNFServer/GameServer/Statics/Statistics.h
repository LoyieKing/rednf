#ifndef STATISTICS_H_
#define STATISTICS_H_

#include <set>
#include <map>
#include <list>
#include <algorithm>
#include <string>
#include <vector>

#include "GMAccounts.h"
#include "CubeStatistics.h"

class MoneyLogPacket;
class Packet_P2P_Statistics;
class Packet_Stat_Lag_Statistics;
class Packet_Server_Match_data;
class Packet_Value_Statistic;
class Packet_Emblem_Create_Statistic;
class Packet_Stat_Compatibility_Index;
class Packet_Avater_Disjoint_Statistic;
class Packet_Randombox_statistic;
class Packet_Reason_Crash_Down_Info;
class Packet_Secret_Shop_Statistic;
class Packet_User_Count_Statistic;
class Packet_Circulation_Statistic;
class Packet_Blood_dungeon_statistic;
class Packet_Goldcard_Event_Statistic_GTS;
class Packet_TowerOfDespair_Statistic_GTS;
class Packet_Dungeon_Statistic_Party;
class Packet_Fatigue_Battery_Money_Statistic;
class Packet_Assert_Manager_Info;
class Packet_HellParty_Statistic_Item;
class Packet_Overflow_Statistic_Add;
class Packet_DeathTower_Statistic_Value;
class Packet_Dungeon_Statistic_Party_Job;
class Packet_Loading_Time_Report_Statistics;
class Packet_Dungeon_Statistic_Party_Charac;
class Packet_DeathTower_Statistic_Playdata_Job;
class Packet_DeathTower_Statistic_Playdata_Party;
class Packet_User_Ting_TimeCheck_Statistic_Add;
class Packet_Cube_Statistic;
class Packet_Server_Match_data_DBMW;

class CServerHandler;
class CApplication;

// ---- HellParty 统计上报包（ORIG Statistics.cpp 发射）----
#pragma pack(push, 1)
class Packet_DBMW_HellParty_Statistic_Item : public PacketHeader
{
public:
    Packet_DBMW_HellParty_Statistic_Item();
    char m_data[0x17a4];
} __attribute__((packed));
#pragma pack(pop)

// ---- StatisticManager（布局见 ctor 反编译）----
class StatisticManager
{
public:
    StatisticManager();
    ~StatisticManager();
    void DBSaveProcess(CServerHandler* handler);
    void* getCubeStatisticObject();
    void ResetPartyMap();
    void ResetPartyJobMap();
    void ResetPartyCharacMap();
    void ResetBloodDungeon();
    void ResetP2PStatistic();
    void ResetFatigueBattery();
    void ResetValueStatistic();
    void ResetServerMatchData();
    void ResetTowerOfDespair();
    void ResetAssertManagerMap();
    void ResetCreateEmblemInfo();
    void ResetPacketOverflowMap();
    void ResetDeathTowerValueMap();
    void ResetRandomboxStatistic();
    void ResetSecretShopStatistic();
    void ResetCirculationStatistic();
    void ResetTingUserTimeCheckMap();
    void ResetUserTIngTimeCheckMap();
    void ResetGoldcardEventStatistic();
    void ResetReasonCrashDownInfoMap();
    void ResetDisjointAvatarInfoTotal();
    void ResetDeathTowerPlayDataJobMap();
    void ResetDeathTowerPlayDataPartyMap();
    void ResetHellPartyStatisticItemMap();
    void SendDBPartyStatistic(CServerHandler* handler);
    void SendDBPartyJobStatistic(CServerHandler* handler);
    void SendDBPartyCharacStatistic(CServerHandler* handler);
    void SendDBValueStatistic(CServerHandler* handler);
    void SendDBCirculationStatistic(CServerHandler* handler);
    void SendDBSecretShopStatistic(CServerHandler* handler);
    void SendDBGoldcardEventStatistic(CServerHandler* handler);
    void SendDBTowerOfDespairStatistic(CServerHandler* handler);
    void SendDBDisjointAvatarInfoTotal(CServerHandler* handler);
    void SendDBP2PStatistic(CServerHandler* handler);
    void SendDBFatigueBattery(CServerHandler* handler);
    void SendDBBloodDungeonStatistic(CServerHandler* handler);
    void SendDBHellPartyStatisticItem(CServerHandler* handler);
    void SendDBPacketOverflowStatistic(CServerHandler* handler);
    void SendDBDeathTowerValueStatistic(CServerHandler* handler);
    void SendDBDeathTowerPlayDataJobStatistic(CServerHandler* handler);
    void SendDBDeathTowerPlayDataPartyStatistic(CServerHandler* handler);
    void SendDBAssertManagerStatistic(CServerHandler* handler);
    void SendDBCreateEmblemInfo(CServerHandler* handler);
    void SendDBLoadingTimeReport(CServerHandler* handler);
    void SendDBRandomboxStatistic(CServerHandler* handler);
    void SendDBUserTingTimeCheckStatistic(CServerHandler* handler);
    void SendDBPowerwarLagReport(CServerHandler* handler);
    void SendDBTingUserTimeCheck(CServerHandler* handler);
    void SendDBPowerwarLoadingTimeReport(CServerHandler* handler);
    void SendDBServerMatchData(CServerHandler* handler);
    void SendDBLagStatistics(CServerHandler* handler, char* data);
    void AddMoneyLog(MoneyLogPacket* pkt, CServerHandler* handler);
    void AddP2PStatistic(Packet_P2P_Statistics* pkt);
    void AddLagStatistics(Packet_Stat_Lag_Statistics* pkt);
    void AddServerMatchData(Packet_Server_Match_data* pkt);
    void AddValueStatistics(Packet_Value_Statistic* pkt);
    void AddCreateEmblemInfo(Packet_Emblem_Create_Statistic* pkt);
    void AddCompatibilityIndex(Packet_Stat_Compatibility_Index* pkt, CServerHandler* handler);
    void AddDisjointAvatarInfo(Packet_Avater_Disjoint_Statistic* pkt);
    void AddRandomboxStatistic(Packet_Randombox_statistic* pkt);
    void AddReasonCrashDownData(Packet_Reason_Crash_Down_Info* pkt, CServerHandler* handler);
    void AddSecretShopStatistic(Packet_Secret_Shop_Statistic* pkt);
    void AddUserCountStatistics(CServerHandler* handler, Packet_User_Count_Statistic* pkt);
    void AddCirculationStatistics(Packet_Circulation_Statistic* pkt);
    void AddBloodDungeonStatistics(Packet_Blood_dungeon_statistic* pkt);
    void AddGoldcardEventStatistic(Packet_Goldcard_Event_Statistic_GTS* pkt);
    void AddTowerOfDespairStatistic(Packet_TowerOfDespair_Statistic_GTS* pkt);
    void WriteDungeonPartyStatistic(Packet_Dungeon_Statistic_Party* pkt);
    void AddFatigueBatteryStatistics(Packet_Fatigue_Battery_Money_Statistic* pkt);
    void WriteAssertManagerStatistic(Packet_Assert_Manager_Info* pkt);
    void WriteHellPartyStatisticItem(Packet_HellParty_Statistic_Item* pkt);
    void WritePacketOverflowStatistic(Packet_Overflow_Statistic_Add* pkt);
    void WriteDeathTowerValueStatistic(Packet_DeathTower_Statistic_Value* pkt);
    void WriteDungeonPartyJobStatistic(Packet_Dungeon_Statistic_Party_Job* pkt);
    void AddLoadingTimeReportStatistics(Packet_Loading_Time_Report_Statistics* pkt);
    void WriteUserTingTImeCheckStatistic(Packet_User_Ting_TimeCheck_Statistic_Add* pkt);
    void WriteDungeonPartyCharacStatistic(Packet_Dungeon_Statistic_Party_Charac* pkt);
    void WriteDeathTowerPlayDataJobStatistic(Packet_DeathTower_Statistic_Playdata_Job* pkt);
    void WriteDeathTowerPlayDataPartyStatistic(Packet_DeathTower_Statistic_Playdata_Party* pkt);
    void avgPing(int& a, int& b, short& c);
    void maxPing(short& a, short& b);
    void minPing(short& a, short& b);
    void sumPing(int& a, short& b, int& c);
    void AMDecrypt(void* data, unsigned int len);
    std::set<unsigned int> m_serverList;   // +0
    CCubeStatistic m_cube;                 // +0x18
    char m_flag;                           // +0x30
    unsigned int m_field34;                // +0x34
    std::map<STPartyStatisticKey, PartyStatistic> m_party;          // +0x38
    std::map<STPartyJobStatisticKey, PartyJobStatistic> m_partyJob; // +0x50
    std::map<STPartyCharacKey, PartyCharacStatistic> m_partyCharac; // +0x68
    std::map<STDeathTowerValueStatisticKey, ValueStatistic> m_deathTowerValue; // +0x80
    std::map<STDeathTowerPlayDataJobStatisticKey, PlayDataJobStatistic>
        m_deathTowerJob;                   // +0x98
    std::map<STDeathTowerPlayDataPartyStatisticKey, PlayDataPartyStatistic>
        m_deathTowerParty;                 // +0xb0
    std::map<STPacketOverflowKey, int> m_packetOverflow;   // +0xc8
    std::map<STAssertManagerKey, int> m_assertManager;     // +0xe0
    std::map<STUserTingTimeCheckKey, int> m_userTing;      // +0xf8
    std::map<unsigned int, int> m_field110;                // +0x110
    std::map<STHellPartyStatisticItemKey, HellPartyItenmData> m_hellParty; // +0x128
    LoadingTimeReport m_loading;           // +0x140
    std::map<STPowerwarFightLoadingKey, STPowerwarFightLoadingData> m_pwLoading; // +0x188
    std::map<STPowerwarFightLagKey, STPowerwarFightLagData> m_pwLag;  // +0x1a0
    std::map<unsigned char, STFatigueBattery> m_fatigue;   // +0x1b8
    std::map<unsigned int, STBloodDungeonStatistic> m_blood;  // +0x1d0
    stDisjointAvatarInfoTotal m_disjoint;  // +0x1e8
    stCreateEmblemStatistic m_createEmblem; // +0x32c
    char m_randombox[0x28];                // +0x348
    STModuleLagStatistics m_modules[8];    // +0x370
    std::map<unsigned short, STDungeonLagStatistics> m_dungeonLag;  // +0x3f0
    std::map<int, ValueStatisticData> m_value;       // +0x408
    std::map<int, CirculationStatisticData> m_circ;  // +0x420
    char m_serverMatch[0xc];               // +0x438
    std::map<int, SECRET_SHOP_STATISTIC_DATA> m_secretShop[3];  // +0x444
    GoldCardEventStatistic m_goldcard[0x63];         // +0x48c
    TowerOfDespairStatistic_Value m_tower[101];      // +0x807
    stP2PStatistics m_p2p;                           // +0xb30
    std::map<STReasonCrashDownKey, unsigned int> m_reasonCrash;  // +0xb78
};

#endif // STATISTICS_H_
