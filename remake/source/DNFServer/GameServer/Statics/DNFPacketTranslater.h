#ifndef DNF_PACKET_TRANSLATER_H_
#define DNF_PACKET_TRANSLATER_H_

#include "PacketHeader.h"

class CApplication;

// ---- 互通包（OnHolePunchingSuccessRateStatistic 组装）----
#pragma pack(push, 1)
class Packet_GameServer2Statisctics2DBServer : public PacketHeader
{
public:
    Packet_GameServer2Statisctics2DBServer();
    unsigned short m_fieldA;  // +0xa
    char m_fieldB;            // +0xc
    int m_fieldC;             // +0xd
    int m_fieldD;             // +0x11
    char m_restE[0x10];       // +0x15
    char m_restF[0x10];       // +0x25
} __attribute__((packed));
#pragma pack(pop)

// ---- CPacketTranslater：静态 m_pclApp + 49 个处理函数 ----
class CPacketTranslater
{
public:
    static void attach(CApplication* app);
    static void OnEventEnd(PacketHeader* pkt);
    static void OnMoneyLog(PacketHeader* pkt);
    static void OnHeartBeat(PacketHeader* pkt);
    static void OnStatistic(PacketHeader* pkt);
    static void OnEventStart(PacketHeader* pkt);
    static void OnP2PStatistic(PacketHeader* pkt);
    static void OnCubeStatistic(PacketHeader* pkt);
    static void OnFileStatistic(PacketHeader* pkt);
    static void OnStatisticGmCmd(PacketHeader* pkt);
    static void OnValueStatistic(PacketHeader* pkt);
    static void OnServerMatchData(PacketHeader* pkt);
    static void OnLagStatisticsAdd(PacketHeader* pkt);
    static void OnPvpPingStatistic(PacketHeader* pkt);
    static void OnUserTingTimeCheck(PacketHeader* pkt);
    static void OnCompatibilityIndex(PacketHeader* pkt);
    static void OnPartyPingStatistic(PacketHeader* pkt);
    static void OnRandomboxStatistic(PacketHeader* pkt);
    static void OnUserCountStatistic(PacketHeader* pkt);
    static void OnClientSpecStatistic(PacketHeader* pkt);
    static void OnReasonCrashDownData(PacketHeader* pkt);
    static void OnSecretShopStatistic(PacketHeader* pkt);
    static void OnCirculationStatistic(PacketHeader* pkt);
    static void OnFairPvpPingStatistic(PacketHeader* pkt);
    static void OnPartyResultStatistic(PacketHeader* pkt);
    static void OnAbnormalExitStatistic(PacketHeader* pkt);
    static void OnBloodDungeonStatistic(PacketHeader* pkt);
    static void OnDungeonStatisticParty(PacketHeader* pkt);
    static void OnFrameLagStatisticsAdd(PacketHeader* pkt);
    static void OnAssertManagerStatistic(PacketHeader* pkt);
    static void OnGoldCardEventStatistic(PacketHeader* pkt);
    static void OnHellPartyStatisticItem(PacketHeader* pkt);
    static void OnMonitorManagerConnectOK(PacketHeader* pkt);
    static void OnPacketOverflowStatistic(PacketHeader* pkt);
    static void OnTowerOfDespairStatistic(PacketHeader* pkt);
    static void OnDeathTowerStatisticValue(PacketHeader* pkt);
    static void OnDungeonStatisticPartyJob(PacketHeader* pkt);
    static void OnUpdateCreateEmblemStatic(PacketHeader* pkt);
    static void OnUpdateDisjointAvatarStatic(PacketHeader* pkt);
    static void OnDungeonStatisticPartyCharac(PacketHeader* pkt);
    static void OnLoadingTimeReportStatistics(PacketHeader* pkt);
    static void OnFatigueBatteryMoneyStatistics(PacketHeader* pkt);
    static void OnDeathTowerStatisticPlayDataJob(PacketHeader* pkt);
    static void OnDeathTowerStatisticPlayDataParty(PacketHeader* pkt);
    static void OnFrameLagStatisticsResultLoadSpec(PacketHeader* pkt);
    static void OnHolePunchingSuccessRateStatistic(PacketHeader* pkt);
    static void OnFrameLagStatisticsResultReloadSpec(PacketHeader* pkt);
    static void OnFrameLagStatisticsSpecDeleteNotify(PacketHeader* pkt);
    static void OnFrameLagStatisticsCollectIntervalCheck(PacketHeader* pkt);
    static CApplication* m_pclApp;
};

#endif // DNF_PACKET_TRANSLATER_H_
