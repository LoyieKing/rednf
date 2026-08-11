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

#pragma pack(push, 1)
struct Packet_ClientSpecLocal
{
    char m_pad[0xa];
    unsigned char m_type;       // +0xa
    unsigned char m_count;      // +0xb
    int m_errorCode;            // +0xc
    unsigned short m_errorLine; // +0x10
    HWSpec m_spec[1];           // +0x12
};
#pragma pack(pop)
#pragma pack(push, 1)
struct FrameLagFpsEntry
{
    char m_pad[4];                    // +0
    short m_windowFps;                // +4
    short m_fullFps;                  // +6
    short m_fullWindowFps;            // +8
    short m_fullWindowNosyncFps;      // +0xa
    char m_rest[0x2c];                // +0xc
};
struct FrameLagRec
{
    int m_frame;                      // +0
    float m_time;                     // +4
};
union Packet_FrameLagAddLocal
{
    struct
    {
        unsigned short m_packetId;          // +0
        unsigned short m_packetSize;        // +2
        unsigned short m_reversed1;         // +4
        unsigned int m_reversed2;           // +6
        int m_cpuClock;                     // +0xa
        char m_numOfProcessor;              // +0xe
        char m_cpuVendor;                   // +0xf
        char m_pad1[2];                     // +0x10
        unsigned short m_videoCardVendorId; // +0x12
        unsigned short m_videoCardDeviceId; // +0x14
        short m_availableTextureMemory;     // +0x16
        short m_ramMemory;                  // +0x18
        char m_osVersion;                   // +0x1a
        int m_directxVersion;               // +0x1b
        char m_crash;                       // +0x1f
        FrameLagFpsEntry m_fps[6];          // +0x20
    } m_main;
    struct
    {
        char m_pad2[0xc];                   // +0
        FrameLagRec m_frameLag[52];         // +0xc
    } m_lag;
};
#pragma pack(pop)
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
        Packet_ClientSpecLocal* pck = (Packet_ClientSpecLocal*)pkt;
        CHWSpecResearcher* hw = m_pclApp->Get_HWspecResearch();
        for (int i = 0; i < (int)(unsigned char)pck->m_count; i++)
        {
            hw->WriteSpecStatics(pck->m_type, pck->m_spec[i]);
        }
        if (pck->m_type == 1)
        {
            hw->WriteErrorLineStatics(pck->m_errorLine, pck->m_errorCode);
        }

    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog(__FUNCTION__, 254)("./log/Except", "CPacketTranslater::OnClientSpecStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 259)("./log/Except", "CPacketTranslater::OnClientSpecStatistic() Exception Break\n");
    }
}
void CPacketTranslater::OnFrameLagStatisticsAdd(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("CPacketTranslater::OnFrameLagStatisticsAdd() : 0 == m_pclApp")
        Packet_FrameLagAddLocal* pck = (Packet_FrameLagAddLocal*)pkt;
        static int chk_ting[8];
        int nRet;
        FrameLagCollector* flc = m_pclApp->Get_FrameLagCollector();
        const char* pPath = "./SHOW_PACKET";
        nRet = access(pPath, 0);
        if (nRet == 0)
        {
            CMyFileLog(__FUNCTION__, 0x124)("./log/FrameLag.log", "packet->m_wSize\t\t: %hu", pck->m_main.m_packetSize);
            CMyFileLog(__FUNCTION__, 0x125)("./log/FrameLag.log", "crashCount\t\t\t\t: %hhd", (int)(char)pck->m_main.m_crash);
            CMyFileLog(__FUNCTION__, 0x126)("./log/FrameLag.log", "cpuInfo.cpuClock       : %d", pck->m_main.m_cpuClock);
            CMyFileLog(__FUNCTION__, 0x127)("./log/FrameLag.log", "cpuInfo.numOfProcessor : %hhd", (int)(char)pck->m_main.m_numOfProcessor);
            CMyFileLog(__FUNCTION__, 0x128)("./log/FrameLag.log", "cpuInfo.cpuVendor      : %hhd", (int)(char)pck->m_main.m_cpuVendor);
            CMyFileLog(__FUNCTION__, 0x129)("./log/FrameLag.log", "videoCardVendorId      : %hu", pck->m_main.m_videoCardVendorId);
            CMyFileLog(__FUNCTION__, 0x12a)("./log/FrameLag.log", "videoCardDeviceId      : %hu", pck->m_main.m_videoCardDeviceId);
            CMyFileLog(__FUNCTION__, 299)("./log/FrameLag.log", "availableTextureMemory : %hd", (int)pck->m_main.m_availableTextureMemory);
            CMyFileLog(__FUNCTION__, 300)("./log/FrameLag.log", "ramMemory              : %hd", (int)pck->m_main.m_ramMemory);
            CMyFileLog(__FUNCTION__, 0x12d)("./log/FrameLag.log", "osVersion              : %hhd", (int)(char)pck->m_main.m_osVersion);
            CMyFileLog(__FUNCTION__, 0x12e)("./log/FrameLag.log", "directxVersion         : %x", pck->m_main.m_directxVersion);
            CMyFileLog(__FUNCTION__, 0x130)("./log/FrameLag.log", "crash\t\t\t\t\t: %hhd", (int)(char)pck->m_main.m_crash);
            if (-1 < pck->m_main.m_crash && pck->m_main.m_crash < 8)
            {
                chk_ting[pck->m_main.m_crash] = chk_ting[pck->m_main.m_crash] + 1;
            }
            for (int i = 0; i < 6; i++)
            {
                CMyFileLog(__FUNCTION__, 0x138)("./log/FrameLag.log", "window_fps fps[%d]             : %hd", i,
                      (int)pck->m_main.m_fps[i].m_windowFps);
                CMyFileLog(__FUNCTION__, 0x139)("./log/FrameLag.log", "full_fps fps[%d]               : %hd", i,
                      (int)pck->m_main.m_fps[i].m_fullFps);
                CMyFileLog(__FUNCTION__, 0x13a)("./log/FrameLag.log", "full_window_fps fps[%d]        : %hd", i,
                      (int)pck->m_main.m_fps[i].m_fullWindowFps);
                CMyFileLog(__FUNCTION__, 0x13b)("./log/FrameLag.log", "full_window_nosync_fps fps[%d] : %hd", i,
                      (int)pck->m_main.m_fps[i].m_fullWindowNosyncFps);
                for (int j = 0; j < 6; j++)
                {
                    CMyFileLog(__FUNCTION__, 0x13e)("./log/FrameLag.log",
                          "m_frameLagArray[%d].framelag[%d].frame : %d", i, j,
                          pck->m_lag.m_frameLag[i * 7 + (4 + j)].m_frame);
                    CMyFileLog(__FUNCTION__, 0x13f)("./log/FrameLag.log",
                          "m_frameLagArray[%d].framelag[%d].time : %.3f", i, j,
                          (double)pck->m_lag.m_frameLag[i * 7 + (4 + j)].m_time);
                }
            }
            register unsigned int t5 = chk_ting[5];
            register unsigned int t4 = chk_ting[4];
            register unsigned int t3 = chk_ting[3];
            register unsigned int t2 = chk_ting[2];
            register unsigned int t1 = chk_ting[1];
            register unsigned int t0 = chk_ting[0];
            CMyFileLog(__FUNCTION__, 0x142)("./log/FrameLag.log", "TOTAL TING : %u, %u, %u, %u, %u, %u",
                  t0, t1, t2, t3, t4, t5);
            CMyFileLog(__FUNCTION__, 0x144)("./log/FrameLag.log", "packet validation : %d",
                  flc->is_valid_statistic_packet((Packet_Frame_Lag_Statistic_Add*)pck) & 0xff);
            CMyFileLog(__FUNCTION__, 0x146)("./log/FrameLag.log", "\n");
        }
        else
        {
            for (int i = 0; i < 8; i++)
            {
                chk_ting[i] = 0;
            }
        }
        flc->PushOneFrameLagData((Packet_Frame_Lag_Statistic_Add*)pck);

    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog(__FUNCTION__, 340)("./log/Except", "CPacketTranslater::OnFrameLagStatisticsAdd() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 345)("./log/Except", "CPacketTranslater::OnFrameLagStatisticsAdd() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 370)("./log/Except", "CPacketTranslater::OnFrameLagStatisticsResultLoadSpec() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 375)("./log/Except", "CPacketTranslater::OnFrameLagStatisticsResultLoadSpec() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 396)("./log/Except", "CPacketTranslater::OnFrameLagStatisticsResultReloadSpec() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 401)("./log/Except", "CPacketTranslater::OnFrameLagStatisticsResultReloadSpec() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 422)("./log/Except", "CPacketTranslater::OnFrameLagStatisticsCollectIntervalCheck() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 427)("./log/Except", "CPacketTranslater::OnFrameLagStatisticsCollectIntervalCheck() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 450)("./log/Except", "CPacketTranslater::OnFrameLagStatisticsSpecDeleteNotify() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 455)("./log/Except", "CPacketTranslater::OnFrameLagStatisticsSpecDeleteNotify() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 478)("./log/Except", "CPacketTranslater::OnPartyResultStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 484)("./log/Except", "CPacketTranslater::OnPartyResultStatistic() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 508)("./log/Except", "CPacketTranslater::OnPartyPingStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 514)("./log/Except", "CPacketTranslater::OnPartyPingStatistic() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 538)("./log/Except", "CPacketTranslater::OnPvpPingStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 544)("./log/Except", "CPacketTranslater::OnPvpPingStatistic() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 569)("./log/Except", "CPacketTranslater::OnFairPvpPingStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 575)("./log/Except", "CPacketTranslater::OnFairPvpPingStatistic() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 600)("./log/Except", "CPacketTranslater::OnAbnormalExitStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 606)("./log/Except", "CPacketTranslater::OnAbnormalExitStatistic() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 631)("./log/Except", "CPacketTranslater::OnDungeonStatisticParty() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 637)("./log/Except", "CPacketTranslater::OnDungeonStatisticParty() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 663)("./log/Except", "CPacketTranslater::OnDungeonStatisticPartyJob() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 669)("./log/Except", "CPacketTranslater::OnDungeonStatisticPartyJob() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 695)("./log/Except", "CPacketTranslater::OnDungeonStatisticPartyCharac() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 701)("./log/Except", "CPacketTranslater::OnDungeonStatisticPartyCharac() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 728)("./log/Except", "CPacketTranslater::OnDeathTowerStatisticValue() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 734)("./log/Except", "CPacketTranslater::OnDeathTowerStatisticValue() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 760)("./log/Except", "CPacketTranslater::OnDeathTowerStatisticPlayDataJob() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 766)("./log/Except", "CPacketTranslater::OnDeathTowerStatisticPlayDataJob() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 792)("./log/Except", "CPacketTranslater::OnDeathTowerStatisticPlayDataParty() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 798)("./log/Except", "CPacketTranslater::OnDeathTowerStatisticPlayDataParty() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 822)("./log/Except", "CPacketTranslater::OnPacketOverflowStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 827)("./log/Except", "CPacketTranslater::OnPacketOverflowStatistic() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 845)("./log/Except", "CPacketTranslater::OnAssertManagerStatistic() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 850)("./log/Except", "CPacketTranslater::OnAssertManagerStatistic() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 868)("./log/Except", "CPacketTranslater::OnUserTingTimeCheck() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 873)("./log/Except", "CPacketTranslater::OnUserTingTimeCheck() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 896)("./log/Except", "CPacketTranslater::OnHellPartyStatisticItem() Exception Break : %s\n", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 902)("./log/Except", "CPacketTranslater::OnHellPartyStatisticItem() Exception Break\n");
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
        CMyFileLog(__FUNCTION__, 923)("./log/Except", "CPacketTranslater::OnLoadingTimeReportStatistics() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 928)("./log/Except", "CPacketTranslater::OnLoadingTimeReportStatistics() Exception Break");
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
        CMyFileLog(__FUNCTION__, 946)("./log/Except", "CPacketTranslater::OnFatigueBatteryMoneyStatistics() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 951)("./log/Except", "CPacketTranslater::OnFatigueBatteryMoneyStatistics() Exception Break");
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
        CMyFileLog(__FUNCTION__, 994)("./log/Except", "CPacketTranslater::OnBloodDungeonStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 999)("./log/Except", "CPacketTranslater::OnBloodDungeonStatistic() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1017)("./log/Except", "CPacketTranslater::OnCubeStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1022)("./log/Except", "CPacketTranslater::OnCubeStatistic() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1040)("./log/Except", "CPacketTranslater::OnReasonCrashDownData() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1045)("./log/Except", "CPacketTranslater::OnReasonCrashDownData() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1065)("./log/Except", "CPacketTranslater::OnUpdateDisjointAvatarStatic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1070)("./log/Except", "CPacketTranslater::OnUpdateDisjointAvatarStatic() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1091)("./log/Except", "CPacketTranslater::OnUpdateDisjointAvatarStatic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1096)("./log/Except", "CPacketTranslater::OnUpdateDisjointAvatarStatic() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1116)("./log/Except", "CPacketTranslater::OnUserCountStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1121)("./log/Except", "CPacketTranslater::OnUserCountStatistic() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1142)("./log/Except", "CPacketTranslater::OnRandomboxStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1147)("./log/Except", "CPacketTranslater::OnRandomboxStatistic() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1166)("./log/Except", "CPacketTranlater::OnLagStatisticsAdd() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1171)("./log/Except", "CPacketTranlater::OnLagStatisticsAdd() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1198)("./log/Except", "CPacketTranslater::OnValueStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1203)("./log/Except", "CPacketTranslater::OnValueStatistic() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1230)("./log/Except", "CPacketTranslater::OnCirculationStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1235)("./log/Except", "CPacketTranslater::OnCirculationStatistic() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1255)("./log/Except", "CPacketTranslater::OnServerMatchData() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1260)("./log/Except", "CPacketTranslater::OnServerMatchData() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1279)("./log/Except", "CPacketTranslater::OnSecretShopStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1284)("./log/Except", "CPacketTranslater::OnSecretShopStatistic() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1327)("./log/Except", "CPacketTranslater::OnGoldCardEventStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1332)("./log/Except", "CPacketTranslater::OnGoldCardEventStatistic() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1351)("./log/Except", "CPacketTranslater::OnTowerOfDespairStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1356)("./log/Except", "CPacketTranslater::OnTowerOfDespairStatistic() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1400)("./log/Except", "CPacketTranslater::OnStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1405)("./log/Except", "CPacketTranslater::OnStatistic() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1422)("./log/Except", "CPacketTranslater::OnStatisticGmCmd() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1427)("./log/Except", "CPacketTranslater::OnStatisticGmCmd() Exception Break");
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
        CMyFileLog(__FUNCTION__, 1447)("./log/Except", "CPacketTranslater::OnMoneyLog() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1452)("./log/Except", "CPacketTranslater::OnMoneyLog() Exception Break");
    }
}
void CPacketTranslater::OnCompatibilityIndex(PacketHeader* pkt)
{
    char dummy[16];
    (void)dummy;
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
        CMyFileLog(__FUNCTION__, 1515)("./log/Except", "CPacketTranslater::OnP2PStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1518)("./log/Except", "CPacketTranslater::OnP2PStatistic() Exception Break");
    }
}
void CPacketTranslater::OnFileStatistic(PacketHeader* pkt)
{
    try
    {
        char* pb = (char*)pkt;
        std::string path("./log/");
        if (pb[10] == 0)
        {
            path.append("filestatics");
        }
        else
        {
            path.append(pb + 10);
        }
        CMyRawFileLog()(path.c_str(), pb + 0x10a);

    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog(__FUNCTION__, 1541)("./log/Except", "CPacketTranslater::OnFileStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1546)("./log/Except", "CPacketTranslater::OnFileStatistic() Exception Break");
    }
}
void CPacketTranslater::OnHolePunchingSuccessRateStatistic(PacketHeader* pkt)
{
    try
    {
        THROW_IF_NO_APP("OnHolePunchingSuccessRateStatistic() : 0 == m_pclApp")
        Packet_GameServer2Statisctics2DBServer* pck = (Packet_GameServer2Statisctics2DBServer*)pkt;
        Packet_GameServer2Statisctics2DBServer out;
        out.m_fieldA = pck->m_fieldA;
        out.m_fieldB = pck->m_fieldB;
        out.m_fieldC = pck->m_fieldC;
        out.m_fieldD = pck->m_fieldD;
        strncpy(out.m_restE, pck->m_restE, 0x10);
        strncpy(out.m_restF, pck->m_restF, 0x10);
        m_pclApp->Get_ServerHandler()->SendToDB((PacketHeader*)&out);

    }
    catch (CDNFException& e)
    {
        register const char* reason = e.what();
        CMyFileLog(__FUNCTION__, 1574)("./log/Except", "OnHolePunchingSuccessRateStatistic() Exception Break : %s", reason);
    }
    catch (...)
    {
        CMyFileLog(__FUNCTION__, 1579)("./log/Except", "OnHolePunchingSuccessRateStatistic() Exception Break");
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
