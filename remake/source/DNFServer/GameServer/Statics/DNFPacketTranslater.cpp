// df_statics_r — DNFPacketTranslater（ORIG DNFPacketTranslater.o 拆分）
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define THROW_IF_NO_APP(msg) \
    if (m_pclApp == 0) \
    { \
        throw CDNFException(msg); \
    }

#include "DNFPacketTranslater.h"
#include "DNFApplication.h"
#include "DNFServerHandler.h"
#include "Statistics.h"
#include "UdpCharacteristic.h"
#include "FrameLagCollector.h"
#include "StatisticProxy.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "PacketHeader.h"
#include "Packet_Cube_Statistic.h"
#include "Packet_P2P_Statistics.h"
#include "Packet_Value_Statistic.h"
#include "Packet_Server_Match_data.h"
#include "Packet_Stat_Lag_Statistics.h"
#include "Packet_Pvp_Ping_Statistic.h"
#include "Packet_Party_Ping_Statistic.h"
#include "Packet_Fair_Pvp_Ping_Statistic.h"
#include "Packet_Party_Result_Statistic.h"
#include "Packet_Abnormal_Exit_Statistic.h"
#include "Packet_User_Ting_TimeCheck_Statistic_Add.h"
#include "Packet_Randombox_statistic.h"
#include "Packet_Reason_Crash_Down_Info.h"
#include "Packet_Avater_Disjoint_Statistic.h"
#include "Packet_Emblem_Create_Statistic.h"
#include "Packet_Server_Match_data.h"
#include "Packet_Circulation_Statistic.h"
#include "Packet_Blood_dungeon_statistic.h"
#include "Packet_Dungeon_Statistic_Party.h"
#include "Packet_Dungeon_Statistic_Party_Job.h"
#include "Packet_Dungeon_Statistic_Party_Charac.h"
#include "Packet_DeathTower_Statistic_Value.h"
#include "Packet_DeathTower_Statistic_Playdata_Job.h"
#include "Packet_DeathTower_Statistic_Playdata_Party.h"
#include "Packet_Overflow_Statistic_Add.h"
#include "Packet_Assert_Manager_Info.h"
#include "Packet_HellParty_Statistic_Item.h"
#include "Packet_Loading_Time_Report_Statistics.h"
#include "Packet_Fatigue_Battery_Money_Statistic.h"
#include "Packet_Goldcard_Event_Statistic_GTS.h"
#include "Packet_TowerOfDespair_Statistic_GTS.h"
#include "Packet_Frame_Lag_Statistic_Add.h"

static char chk_ting[8];
void CPacketTranslater::attach(CApplication* app)
{
    m_pclApp = app;
}
void CPacketTranslater::OnHeartBeat(PacketHeader* pkt)
{
}
void CPacketTranslater::OnEventStart(PacketHeader* pkt)
{
}
void CPacketTranslater::OnEventEnd(PacketHeader* pkt)
{
}
void CPacketTranslater::OnMonitorManagerConnectOK(PacketHeader* pkt)
{
}
void CPacketTranslater::OnClientSpecStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnClientSpecStatistic : 0 == m_pclApp")
        char* pb = (char*)pkt;
        CHWSpecResearcher* hw = m_pclApp->Get_HWspecResearch();
        for (int i = 0; i < (int)(unsigned char)pb[0xb]; i++)
        {
            hw->WriteSpecStatics(pb[10], *(HWSpec*)(pb + i * 0xc + 0x12));
        }
        if (pb[10] == 1)
        {
            hw->WriteErrorLineStatics(*(unsigned short*)(pb + 0x10), *(int*)(pb + 0xc));
        }

    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnClientSpecStatistic", 254);
        log("./log/Except", "CPacketTranslater::OnClientSpecStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnClientSpecStatistic", 259);
        log("./log/Except", "CPacketTranslater::OnClientSpecStatistic() Exception Break\n");
    }
}
void CPacketTranslater::OnFrameLagStatisticsAdd(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnFrameLagStatisticsAdd() : 0 == m_pclApp")
        FrameLagCollector* flc = m_pclApp->Get_FrameLagCollector();
        char* pb = (char*)pkt;
        if (access("./SHOW_PACKET", 0) == 0)
        {
            CMyFileLog log1("OnFrameLagStatisticsAdd", 0x124);
            log1("./log/FrameLag.log", "packet->m_wSize\t\t: %hu", *(unsigned short*)(pb + 2));
            CMyFileLog log2("OnFrameLagStatisticsAdd", 0x125);
            log2("./log/FrameLag.log", "crashCount\t\t\t\t: %hhd", (int)(char)pb[0x1f]);
            CMyFileLog log3("OnFrameLagStatisticsAdd", 0x126);
            log3("./log/FrameLag.log", "cpuInfo.cpuClock       : %d", *(int*)(pb + 0x28));
            CMyFileLog log4("OnFrameLagStatisticsAdd", 0x127);
            log4("./log/FrameLag.log", "cpuInfo.numOfProcessor : %hhd", (int)(char)pb[0x38]);
            CMyFileLog log5("OnFrameLagStatisticsAdd", 0x128);
            log5("./log/FrameLag.log", "cpuInfo.cpuVendor      : %hhd", (int)(char)pb[0x3c]);
            CMyFileLog log6("OnFrameLagStatisticsAdd", 0x129);
            log6("./log/FrameLag.log", "videoCardVendorId      : %hu", (unsigned int)*(unsigned short*)(pb + 0x48));
            CMyFileLog log7("OnFrameLagStatisticsAdd", 0x12a);
            log7("./log/FrameLag.log", "videoCardDeviceId      : %hu", (unsigned int)*(unsigned short*)(pb + 0x50));
            CMyFileLog log8("OnFrameLagStatisticsAdd", 299);
            log8("./log/FrameLag.log", "availableTextureMemory : %hd", (int)*(short*)(pb + 0x58));
            CMyFileLog log9("OnFrameLagStatisticsAdd", 300);
            log9("./log/FrameLag.log", "ramMemory              : %hd", (int)*(short*)(pb + 0x60));
            CMyFileLog log10("OnFrameLagStatisticsAdd", 0x12d);
            log10("./log/FrameLag.log", "osVersion              : %hhd", (int)(char)pb[0x68]);
            CMyFileLog log11("OnFrameLagStatisticsAdd", 0x12e);
            log11("./log/FrameLag.log", "directxVersion         : %x", *(unsigned int*)(pb + 0x6c));
            CMyFileLog log12("OnFrameLagStatisticsAdd", 0x130);
            log12("./log/FrameLag.log", "crash\t\t\t\t\t: %hhd", (int)(char)pb[0x7c]);
            if (-1 < (char)pb[0x7c] && (char)pb[0x7c] < 8)
            {
                chk_ting[(char)pb[0x7c]]++;
            }
            unsigned int t5 = *(unsigned int*)(pb + 0x14c);
            unsigned int t4 = *(unsigned int*)(pb + 0x148);
            unsigned int t3 = *(unsigned int*)(pb + 0x144);
            unsigned int t2 = *(unsigned int*)(pb + 0x140);
            unsigned int t1 = *(unsigned int*)(pb + 0x13c);
            unsigned int t0 = *(unsigned int*)(pb + 0x138);
            for (int i = 0; i < 6; i++)
            {
                CMyFileLog log13("OnFrameLagStatisticsAdd", 0x138);
                log13("./log/FrameLag.log", "window_fps fps[%d]             : %hd", i,
                      (int)*(short*)(pb + i * 0x38 + 0x24));
                CMyFileLog log14("OnFrameLagStatisticsAdd", 0x139);
                log14("./log/FrameLag.log", "full_fps fps[%d]               : %hd", i,
                      (int)*(short*)(pb + i * 0x38 + 0x26));
                CMyFileLog log15("OnFrameLagStatisticsAdd", 0x13a);
                log15("./log/FrameLag.log", "full_window_fps fps[%d]        : %hd", i,
                      (int)*(short*)(pb + i * 0x38 + 0x28));
                CMyFileLog log16("OnFrameLagStatisticsAdd", 0x13b);
                log16("./log/FrameLag.log", "full_window_nosync_fps fps[%d] : %hd", i,
                      (int)*(short*)(pb + i * 0x38 + 0x2a));
                for (int j = 0; j < 6; j++)
                {
                    CMyFileLog log17("OnFrameLagStatisticsAdd", 0x13e);
                    log17("./log/FrameLag.log",
                          "m_frameLagArray[%d].framelag[%d].frame : %d", i, j,
                          *(int*)(pb + (i * 7 + j + 4) * 8 + 0xc));
                    CMyFileLog log18("OnFrameLagStatisticsAdd", 0x13f);
                    log18("./log/FrameLag.log",
                          "m_frameLagArray[%d].framelag[%d].time : %.3f", i, j,
                          (double)*(float*)(pb + (i * 7 + j + 4) * 8 + 0x10));
                }
            }
            CMyFileLog log19("OnFrameLagStatisticsAdd", 0x142);
            log19("./log/FrameLag.log", "TOTAL TING : %u, %u, %u, %u, %u, %u",
                  t0, t1, t2, t3, t4, t5);
            unsigned int valid = flc->is_valid_statistic_packet((Packet_Frame_Lag_Statistic_Add*)pkt);
            CMyFileLog log20("OnFrameLagStatisticsAdd", 0x144);
            log20("./log/FrameLag.log", "packet validation : %d", valid & 0xff);
            CMyFileLog log21("OnFrameLagStatisticsAdd", 0x146);
            log21("./log/FrameLag.log", "\n");
        }
        else
        {
            for (int i = 0; i < 8; i++)
            {
                chk_ting[i] = 0;
            }
        }
        flc->PushOneFrameLagData((Packet_Frame_Lag_Statistic_Add*)pkt);

    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnFrameLagStatisticsAdd", 340);
        log("./log/Except", "CPacketTranslater::OnFrameLagStatisticsAdd() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnFrameLagStatisticsAdd", 345);
        log("./log/Except", "CPacketTranslater::OnFrameLagStatisticsAdd() Exception Break\n");
    }
}
void CPacketTranslater::OnFrameLagStatisticsResultLoadSpec(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnFrameLagStatisticsResultLoadSpec() : 0 == m_pclApp")
        Packet_Frame_Lag_Statistic_Result_Load_Spec* pck = (Packet_Frame_Lag_Statistic_Result_Load_Spec*)pkt;
        FrameLagCollector* flc = m_pclApp->Get_FrameLagCollector();
        flc->PushMonitoringSpecData(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnFrameLagStatisticsResultLoadSpec", 370);
        log("./log/Except", "CPacketTranslater::OnFrameLagStatisticsResultLoadSpec() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnFrameLagStatisticsResultLoadSpec", 375);
        log("./log/Except", "CPacketTranslater::OnFrameLagStatisticsResultLoadSpec() Exception Break\n");
    }
}
void CPacketTranslater::OnFrameLagStatisticsResultReloadSpec(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnFrameLagStatisticsResultReloadSpec() : 0 == m_pclApp")
        Packet_Frame_Lag_Statistic_Result_Reload_Spec* pck = (Packet_Frame_Lag_Statistic_Result_Reload_Spec*)pkt;
        FrameLagCollector* flc = m_pclApp->Get_FrameLagCollector();
        flc->PushMonitoringSpecData(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnFrameLagStatisticsResultReloadSpec", 396);
        log("./log/Except", "CPacketTranslater::OnFrameLagStatisticsResultReloadSpec() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnFrameLagStatisticsResultReloadSpec", 401);
        log("./log/Except", "CPacketTranslater::OnFrameLagStatisticsResultReloadSpec() Exception Break\n");
    }
}
void CPacketTranslater::OnFrameLagStatisticsCollectIntervalCheck(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnFrameLagStatisticsCollectIntervalCheck() : 0 == m_pclApp")
        Packet_Frame_Lag_Collect_Interval_Check* pck = (Packet_Frame_Lag_Collect_Interval_Check*)pkt;
        FrameLagCollector* flc = m_pclApp->Get_FrameLagCollector();
        flc->CollectIntervalCheck(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnFrameLagStatisticsCollectIntervalCheck", 422);
        log("./log/Except", "CPacketTranslater::OnFrameLagStatisticsCollectIntervalCheck() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnFrameLagStatisticsCollectIntervalCheck", 427);
        log("./log/Except", "CPacketTranslater::OnFrameLagStatisticsCollectIntervalCheck() Exception Break\n");
    }
}
void CPacketTranslater::OnFrameLagStatisticsSpecDeleteNotify(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnFrameLagStatisticsSpecDeleteNotify() : 0 == m_pclApp")
        Packet_Frame_Lag_Spec_Delete_Notify* pck = (Packet_Frame_Lag_Spec_Delete_Notify*)pkt;
        FrameLagCollector* flc = m_pclApp->Get_FrameLagCollector();
        flc->PopMonitoringSpecData(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnFrameLagStatisticsSpecDeleteNotify", 450);
        log("./log/Except", "CPacketTranslater::OnFrameLagStatisticsSpecDeleteNotify() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnFrameLagStatisticsSpecDeleteNotify", 455);
        log("./log/Except", "CPacketTranslater::OnFrameLagStatisticsSpecDeleteNotify() Exception Break\n");
    }
}
void CPacketTranslater::OnPartyResultStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnPartyResultStatistic() : 0 == m_pclApp")
        Packet_Party_Result_Statistic* pck = (Packet_Party_Result_Statistic*)pkt;
        UdpCharacteristic* uc = m_pclApp->Get_UdpCharacteristic();
        uc->PushPartyResultData(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnPartyResultStatistic", 478);
        log("./log/Except", "CPacketTranslater::OnPartyResultStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnPartyResultStatistic", 484);
        log("./log/Except", "CPacketTranslater::OnPartyResultStatistic() Exception Break\n");
    }
}
void CPacketTranslater::OnPartyPingStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnPartyPingStatistic() : 0 == m_pclApp")
        Packet_Party_Ping_Statistic* pck = (Packet_Party_Ping_Statistic*)pkt;
        UdpCharacteristic* uc = m_pclApp->Get_UdpCharacteristic();
        uc->PushPartyPingData(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnPartyPingStatistic", 508);
        log("./log/Except", "CPacketTranslater::OnPartyPingStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnPartyPingStatistic", 514);
        log("./log/Except", "CPacketTranslater::OnPartyPingStatistic() Exception Break\n");
    }
}
void CPacketTranslater::OnPvpPingStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnPvpPingStatistic() : 0 == m_pclApp")
        Packet_Pvp_Ping_Statistic* pck = (Packet_Pvp_Ping_Statistic*)pkt;
        UdpCharacteristic* uc = m_pclApp->Get_UdpCharacteristic();
        uc->PushPvpPingData(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnPvpPingStatistic", 538);
        log("./log/Except", "CPacketTranslater::OnPvpPingStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnPvpPingStatistic", 544);
        log("./log/Except", "CPacketTranslater::OnPvpPingStatistic() Exception Break\n");
    }
}
void CPacketTranslater::OnFairPvpPingStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnFairPvpPingStatistic() : 0 == m_pclApp")
        Packet_Fair_Pvp_Ping_Statistic* pck = (Packet_Fair_Pvp_Ping_Statistic*)pkt;
        UdpCharacteristic* uc = m_pclApp->Get_UdpCharacteristic();
        uc->PushFairPvpPingData(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnFairPvpPingStatistic", 569);
        log("./log/Except", "CPacketTranslater::OnFairPvpPingStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnFairPvpPingStatistic", 575);
        log("./log/Except", "CPacketTranslater::OnFairPvpPingStatistic() Exception Break\n");
    }
}
void CPacketTranslater::OnAbnormalExitStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnAbnormalExitStatistic() : 0 == m_pclApp")
        Packet_Abnormal_Exit_Statistic* pck = (Packet_Abnormal_Exit_Statistic*)pkt;
        UdpCharacteristic* uc = m_pclApp->Get_UdpCharacteristic();
        uc->PushAbnormalExitData(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnAbnormalExitStatistic", 600);
        log("./log/Except", "CPacketTranslater::OnAbnormalExitStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnAbnormalExitStatistic", 606);
        log("./log/Except", "CPacketTranslater::OnAbnormalExitStatistic() Exception Break\n");
    }
}
void CPacketTranslater::OnDungeonStatisticParty(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnDungeonStatisticParty() : 0 == m_pclApp")
        Packet_Dungeon_Statistic_Party* pck = (Packet_Dungeon_Statistic_Party*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->WriteDungeonPartyStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnDungeonStatisticParty", 631);
        log("./log/Except", "CPacketTranslater::OnDungeonStatisticParty() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnDungeonStatisticParty", 637);
        log("./log/Except", "CPacketTranslater::OnDungeonStatisticParty() Exception Break\n");
    }
}
void CPacketTranslater::OnDungeonStatisticPartyJob(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnDungeonStatisticPartyJob() : 0 == m_pclApp")
        Packet_Dungeon_Statistic_Party_Job* pck = (Packet_Dungeon_Statistic_Party_Job*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->WriteDungeonPartyJobStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnDungeonStatisticPartyJob", 663);
        log("./log/Except", "CPacketTranslater::OnDungeonStatisticPartyJob() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnDungeonStatisticPartyJob", 669);
        log("./log/Except", "CPacketTranslater::OnDungeonStatisticPartyJob() Exception Break\n");
    }
}
void CPacketTranslater::OnDungeonStatisticPartyCharac(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnDungeonStatisticPartyCharac() : 0 == m_pclApp")
        Packet_Dungeon_Statistic_Party_Charac* pck = (Packet_Dungeon_Statistic_Party_Charac*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->WriteDungeonPartyCharacStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnDungeonStatisticPartyCharac", 695);
        log("./log/Except", "CPacketTranslater::OnDungeonStatisticPartyCharac() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnDungeonStatisticPartyCharac", 701);
        log("./log/Except", "CPacketTranslater::OnDungeonStatisticPartyCharac() Exception Break\n");
    }
}
void CPacketTranslater::OnDeathTowerStatisticValue(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnDeathTowerStatisticValue() : 0 == m_pclApp")
        Packet_DeathTower_Statistic_Value* pck = (Packet_DeathTower_Statistic_Value*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->WriteDeathTowerValueStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnDeathTowerStatisticValue", 728);
        log("./log/Except", "CPacketTranslater::OnDeathTowerStatisticValue() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnDeathTowerStatisticValue", 734);
        log("./log/Except", "CPacketTranslater::OnDeathTowerStatisticValue() Exception Break\n");
    }
}
void CPacketTranslater::OnDeathTowerStatisticPlayDataJob(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnDeathTowerStatisticPlayDataJob() : 0 == m_pclApp")
        Packet_DeathTower_Statistic_Playdata_Job* pck = (Packet_DeathTower_Statistic_Playdata_Job*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->WriteDeathTowerPlayDataJobStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnDeathTowerStatisticPlayDataJob", 760);
        log("./log/Except", "CPacketTranslater::OnDeathTowerStatisticPlayDataJob() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnDeathTowerStatisticPlayDataJob", 766);
        log("./log/Except", "CPacketTranslater::OnDeathTowerStatisticPlayDataJob() Exception Break\n");
    }
}
void CPacketTranslater::OnDeathTowerStatisticPlayDataParty(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnDeathTowerStatisticPlayDataJob() : 0 == m_pclApp")
        Packet_DeathTower_Statistic_Playdata_Party* pck = (Packet_DeathTower_Statistic_Playdata_Party*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->WriteDeathTowerPlayDataPartyStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnDeathTowerStatisticPlayDataParty", 792);
        log("./log/Except", "CPacketTranslater::OnDeathTowerStatisticPlayDataParty() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnDeathTowerStatisticPlayDataParty", 798);
        log("./log/Except", "CPacketTranslater::OnDeathTowerStatisticPlayDataParty() Exception Break\n");
    }
}
void CPacketTranslater::OnPacketOverflowStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnPacketOverflowStatistic() : 0 == m_pclApp")
        Packet_Overflow_Statistic_Add* pck = (Packet_Overflow_Statistic_Add*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->WritePacketOverflowStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnPacketOverflowStatistic", 822);
        log("./log/Except", "CPacketTranslater::OnPacketOverflowStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnPacketOverflowStatistic", 827);
        log("./log/Except", "CPacketTranslater::OnPacketOverflowStatistic() Exception Break\n");
    }
}
void CPacketTranslater::OnAssertManagerStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnAssertManagerStatistic() : 0 == m_pclApp")
        Packet_Assert_Manager_Info* pck = (Packet_Assert_Manager_Info*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->WriteAssertManagerStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnAssertManagerStatistic", 845);
        log("./log/Except", "CPacketTranslater::OnAssertManagerStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnAssertManagerStatistic", 850);
        log("./log/Except", "CPacketTranslater::OnAssertManagerStatistic() Exception Break\n");
    }
}
void CPacketTranslater::OnUserTingTimeCheck(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnUserTingTimeCheck() : 0 == m_pclApp")
        Packet_User_Ting_TimeCheck_Statistic_Add* pck = (Packet_User_Ting_TimeCheck_Statistic_Add*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->WriteUserTingTImeCheckStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnUserTingTimeCheck", 868);
        log("./log/Except", "CPacketTranslater::OnUserTingTimeCheck() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnUserTingTimeCheck", 873);
        log("./log/Except", "CPacketTranslater::OnUserTingTimeCheck() Exception Break\n");
    }
}
void CPacketTranslater::OnHellPartyStatisticItem(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnHellPartyStatisticItem() : 0 == m_pclApp")
        Packet_HellParty_Statistic_Item* pck = (Packet_HellParty_Statistic_Item*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->WriteHellPartyStatisticItem(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnHellPartyStatisticItem", 896);
        log("./log/Except", "CPacketTranslater::OnHellPartyStatisticItem() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnHellPartyStatisticItem", 902);
        log("./log/Except", "CPacketTranslater::OnHellPartyStatisticItem() Exception Break\n");
    }
}
void CPacketTranslater::OnLoadingTimeReportStatistics(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnLoadingTimeReportStatistics() : 0 == m_pclApp")
        Packet_Loading_Time_Report_Statistics* pck = (Packet_Loading_Time_Report_Statistics*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddLoadingTimeReportStatistics(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnLoadingTimeReportStatistics", 923);
        log("./log/Except", "CPacketTranslater::OnLoadingTimeReportStatistics() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnLoadingTimeReportStatistics", 928);
        log("./log/Except", "CPacketTranslater::OnLoadingTimeReportStatistics() Exception Break");
    }
}
void CPacketTranslater::OnFatigueBatteryMoneyStatistics(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnLoadingTimeReportStatistics() : 0 == m_pclApp")
        Packet_Fatigue_Battery_Money_Statistic* pck = (Packet_Fatigue_Battery_Money_Statistic*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddFatigueBatteryStatistics(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnFatigueBatteryMoneyStatistics", 946);
        log("./log/Except", "CPacketTranslater::OnFatigueBatteryMoneyStatistics() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnFatigueBatteryMoneyStatistics", 951);
        log("./log/Except", "CPacketTranslater::OnFatigueBatteryMoneyStatistics() Exception Break");
    }
}
void CPacketTranslater::OnBloodDungeonStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnBloodDungeonStatistic() : 0 == m_pclApp")
        Packet_Blood_dungeon_statistic* pck = (Packet_Blood_dungeon_statistic*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddBloodDungeonStatistics(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnBloodDungeonStatistic", 994);
        log("./log/Except", "CPacketTranslater::OnBloodDungeonStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnBloodDungeonStatistic", 999);
        log("./log/Except", "CPacketTranslater::OnBloodDungeonStatistic() Exception Break");
    }
}
void CPacketTranslater::OnCubeStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnCubeStatistic() : 0 == m_pclApp")
        Packet_Cube_Statistic* pck = (Packet_Cube_Statistic*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        ((CCubeStatistic*)sm->getCubeStatisticObject())->addStatisticData(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnCubeStatistic", 1017);
        log("./log/Except", "CPacketTranslater::OnCubeStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnCubeStatistic", 1022);
        log("./log/Except", "CPacketTranslater::OnCubeStatistic() Exception Break");
    }
}
void CPacketTranslater::OnReasonCrashDownData(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnReasonCrashDownData : 0 == m_pclApp")
        Packet_Reason_Crash_Down_Info* pck = (Packet_Reason_Crash_Down_Info*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddReasonCrashDownData(pck, m_pclApp->m_serverHandler);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnReasonCrashDownData", 1040);
        log("./log/Except", "CPacketTranslater::OnReasonCrashDownData() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnReasonCrashDownData", 1045);
        log("./log/Except", "CPacketTranslater::OnReasonCrashDownData() Exception Break");
    }
}
void CPacketTranslater::OnUpdateDisjointAvatarStatic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnUpdateDisjointAvatarStatic : 0 == m_pclApp")
        Packet_Avater_Disjoint_Statistic* pck = (Packet_Avater_Disjoint_Statistic*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddDisjointAvatarInfo(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnUpdateDisjointAvatarStatic", 1065);
        log("./log/Except", "CPacketTranslater::OnUpdateDisjointAvatarStatic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnUpdateDisjointAvatarStatic", 1070);
        log("./log/Except", "CPacketTranslater::OnUpdateDisjointAvatarStatic() Exception Break");
    }
}
void CPacketTranslater::OnUpdateCreateEmblemStatic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnUpdateCreateEmblemStatic : 0 == m_pclApp")
        Packet_Emblem_Create_Statistic* pck = (Packet_Emblem_Create_Statistic*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddCreateEmblemInfo(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnUpdateCreateEmblemStatic", 1091);
        log("./log/Except", "CPacketTranslater::OnUpdateDisjointAvatarStatic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnUpdateCreateEmblemStatic", 1096);
        log("./log/Except", "CPacketTranslater::OnUpdateDisjointAvatarStatic() Exception Break");
    }
}
void CPacketTranslater::OnUserCountStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnUserCountStatistic() : 0 == m_pclApp")
        Packet_User_Count_Statistic* pck = (Packet_User_Count_Statistic*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddUserCountStatistics(m_pclApp->Get_ServerHandler(), pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnUserCountStatistic", 1116);
        log("./log/Except", "CPacketTranslater::OnUserCountStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnUserCountStatistic", 1121);
        log("./log/Except", "CPacketTranslater::OnUserCountStatistic() Exception Break");
    }
}
void CPacketTranslater::OnRandomboxStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnRandomboxStatistic : 0 == m_pclApp")
        Packet_Randombox_statistic* pck = (Packet_Randombox_statistic*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddRandomboxStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnRandomboxStatistic", 1142);
        log("./log/Except", "CPacketTranslater::OnRandomboxStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnRandomboxStatistic", 1147);
        log("./log/Except", "CPacketTranslater::OnRandomboxStatistic() Exception Break");
    }
}
void CPacketTranslater::OnLagStatisticsAdd(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnLagStatisticsAdd : 0 == m_pclApp")
        Packet_Stat_Lag_Statistics* pck = (Packet_Stat_Lag_Statistics*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddLagStatistics(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnLagStatisticsAdd", 1166);
        log("./log/Except", "CPacketTranlater::OnLagStatisticsAdd() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnLagStatisticsAdd", 1171);
        log("./log/Except", "CPacketTranlater::OnLagStatisticsAdd() Exception Break");
    }
}
void CPacketTranslater::OnValueStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnValueStatistic() : 0 == m_pclApp")
        Packet_Value_Statistic* pck = (Packet_Value_Statistic*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddValueStatistics(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnValueStatistic", 1198);
        log("./log/Except", "CPacketTranslater::OnValueStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnValueStatistic", 1203);
        log("./log/Except", "CPacketTranslater::OnValueStatistic() Exception Break");
    }
}
void CPacketTranslater::OnCirculationStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnCirculationStatistic() : 0 == m_pclApp")
        Packet_Circulation_Statistic* pck = (Packet_Circulation_Statistic*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddCirculationStatistics(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnCirculationStatistic", 1230);
        log("./log/Except", "CPacketTranslater::OnCirculationStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnCirculationStatistic", 1235);
        log("./log/Except", "CPacketTranslater::OnCirculationStatistic() Exception Break");
    }
}
void CPacketTranslater::OnServerMatchData(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnServerMatchData() : 0 == m_pclApp")
        Packet_Server_Match_data* pck = (Packet_Server_Match_data*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddServerMatchData(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnServerMatchData", 1255);
        log("./log/Except", "CPacketTranslater::OnServerMatchData() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnServerMatchData", 1260);
        log("./log/Except", "CPacketTranslater::OnServerMatchData() Exception Break");
    }
}
void CPacketTranslater::OnSecretShopStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnSecretShopStatistic() : 0 == m_pclApp")
        Packet_Secret_Shop_Statistic* pck = (Packet_Secret_Shop_Statistic*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddSecretShopStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnSecretShopStatistic", 1279);
        log("./log/Except", "CPacketTranslater::OnSecretShopStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnSecretShopStatistic", 1284);
        log("./log/Except", "CPacketTranslater::OnSecretShopStatistic() Exception Break");
    }
}
void CPacketTranslater::OnGoldCardEventStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnGoldCardEventStatistic() : 0 == m_pclApp")
        Packet_Goldcard_Event_Statistic_GTS* pck = (Packet_Goldcard_Event_Statistic_GTS*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddGoldcardEventStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnGoldCardEventStatistic", 1327);
        log("./log/Except", "CPacketTranslater::OnGoldCardEventStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnGoldCardEventStatistic", 1332);
        log("./log/Except", "CPacketTranslater::OnGoldCardEventStatistic() Exception Break");
    }
}
void CPacketTranslater::OnTowerOfDespairStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnTowerOfDespairStatistic() : 0 == m_pclApp")
        Packet_TowerOfDespair_Statistic_GTS* pck = (Packet_TowerOfDespair_Statistic_GTS*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddTowerOfDespairStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnTowerOfDespairStatistic", 1351);
        log("./log/Except", "CPacketTranslater::OnTowerOfDespairStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnTowerOfDespairStatistic", 1356);
        log("./log/Except", "CPacketTranslater::OnTowerOfDespairStatistic() Exception Break");
    }
}
void CPacketTranslater::OnStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnStatistic() : 0 == m_pclApp")
        StatisticsPacket* pck = (StatisticsPacket*)pkt;
        statistc_proxy::addStatisticProxy(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnStatistic", 1400);
        log("./log/Except", "CPacketTranslater::OnStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnStatistic", 1405);
        log("./log/Except", "CPacketTranslater::OnStatistic() Exception Break");
    }
}
void CPacketTranslater::OnStatisticGmCmd(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnStatisticGmCmd() : 0 == m_pclApp")
        PacketHeader* pck = (PacketHeader*)pkt;
        statistc_proxy::sendDBStatisticProxy();
        statistc_proxy::resetStatisticProxy();
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnStatisticGmCmd", 1422);
        log("./log/Except", "CPacketTranslater::OnStatisticGmCmd() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnStatisticGmCmd", 1427);
        log("./log/Except", "CPacketTranslater::OnStatisticGmCmd() Exception Break");
    }
}
void CPacketTranslater::OnMoneyLog(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnMoneyLog() : 0 == m_pclApp")
        MoneyLogPacket* pck = (MoneyLogPacket*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddMoneyLog(pck, m_pclApp->Get_ServerHandler());
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnMoneyLog", 1447);
        log("./log/Except", "CPacketTranslater::OnMoneyLog() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnMoneyLog", 1452);
        log("./log/Except", "CPacketTranslater::OnMoneyLog() Exception Break");
    }
}
void CPacketTranslater::OnCompatibilityIndex(PacketHeader* pkt)
{
}
void CPacketTranslater::OnP2PStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnP2PStatistic() : 0 == m_pclApp")
        Packet_P2P_Statistics* pck = (Packet_P2P_Statistics*)pkt;
        StatisticManager* sm = m_pclApp->Get_StatisticManager();
        sm->AddP2PStatistic(pck);
    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnP2PStatistic", 1515);
        log("./log/Except", "CPacketTranslater::OnP2PStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnP2PStatistic", 1518);
        log("./log/Except", "CPacketTranslater::OnP2PStatistic() Exception Break");
    }
}
void CPacketTranslater::OnFileStatistic(PacketHeader* pkt)
{
    try
    {
        char* pb = (char*)pkt;
        std::string path = "./log/";
        if (pb[10] == 0)
        {
            path.append("filestatics");
        }
        else
        {
            path.append(pb + 10);
        }
        CMyRawFileLog raw;
        raw(path.c_str(), pb + 0x10a);

    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnFileStatistic", 1541);
        log("./log/Except", "CPacketTranslater::OnFileStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnFileStatistic", 1546);
        log("./log/Except", "CPacketTranslater::OnFileStatistic() Exception Break");
    }
}
void CPacketTranslater::OnHolePunchingSuccessRateStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("OnHolePunchingSuccessRateStatistic() : 0 == m_pclApp")
        char* pb = (char*)pkt;
        Packet_GameServer2Statisctics2DBServer out;
        *(unsigned short*)((char*)&out + 10) = *(unsigned short*)(pb + 10);
        *(char*)((char*)&out + 0xc) = pb[0xc];
        *(int*)((char*)&out + 0xd) = *(int*)(pb + 0xd);
        *(int*)((char*)&out + 0x11) = *(int*)(pb + 0x11);
        strncpy((char*)&out + 0x15, pb + 0x15, 0x10);
        strncpy((char*)&out + 0x25, pb + 0x25, 0x10);
        m_pclApp->Get_ServerHandler()->SendToDB((PacketHeader*)&out);

    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog log("OnHolePunchingSuccessRateStatistic", 1574);
        log("./log/Except", "OnHolePunchingSuccessRateStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog log("OnHolePunchingSuccessRateStatistic", 1579);
        log("./log/Except", "OnHolePunchingSuccessRateStatistic() Exception Break");
    }
}
Packet_GameServer2Statisctics2DBServer::Packet_GameServer2Statisctics2DBServer()
    : PacketHeader(0x27fd, 0x35)
{
    m_fieldA = 0;
    m_fieldB = 0xff;
    m_fieldC = 0;
    m_fieldD = 0;
    memset(m_restE, 0, sizeof(m_restE));
    memset(m_restF, 0, sizeof(m_restF));
}
CApplication* CPacketTranslater::m_pclApp = 0;
