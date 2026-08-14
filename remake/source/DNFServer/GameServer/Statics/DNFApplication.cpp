// df_statics_r — DNFApplication（ORIG DNFApplication.o 拆分）
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "DNFApplication.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFAppConfig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFUdpHandler.h"
#include "DNFInnerMsgHandler.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTranslater.h"
#include "DNFSignalTranslator.h"
#include "DNFTableBase.h"
#include "Statistics.h"
#include "CubeStatistics.h"
#include "StatisticProxy.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "PacketHeader.h"
#include "Packet_Monitor_Event_Start.h"
#include "Packet_Monitor_Event_End.h"

void CAppBase::Create(int argc, char** argv)
{
    Init(argc, argv);
    Load(argc, argv);
}
void CAppBase::Clear()
{
    Free();
}
CApplication* CApplicationInstance()
{
    static CApplication instance;
    return &instance;
}
CApplication::CApplication()
    : m_loaded(0), m_reserved1(0), m_appInit(0), m_appConfig(0), m_serverConfig(0),
      m_serverHandler(0), m_innerMsg(0), m_udpHandler(0)
{
}
CApplication::~CApplication()
{
    puts("Application Stop!");
}
void ShowLogo()
{
    CommonTime t;
    putchar(10);
    puts("**********************************************************");
    puts("* +---   +-+ +  +----         DUNGEON & FIGHTER          *");
    puts("* |   |  | | |  |                                        *");
    puts("* |   |  | | |  +----                                    *");
    puts("* |   |  | | |  |      Open Beta Test on 2005.08.10~     *");
    puts("* +---   + +-+  |      Copyright(c) 2004,2005 Neople Co. *");
    puts("**********************************************************");
    t.SetCurTime();
    printf("[!] Service Date (%02d-%02d-%02d/%02d:%02d)\n", (int)(char)t.m_time[0],
           (int)t.m_time[1], (int)t.m_time[2], (int)t.m_time[3], (int)t.m_time[4]);
}
void CApplication::Init(int argc, char** argv)
{
    try
    {
        ShowLogo();
        CheckArgv(argc, argv);
        CSignalTranslatorInstance()->init(this);
        AttachAppInitor(argv);
        m_appInit->Init(this, argc, argv);
        puts("Application Init() Success!");
    }
    catch (CDNFException& e)
    {
        printf("CApplication::Init() Exception Break : %s\n", e.what());
        throw;
    }
    catch (...)
    {
        puts("CApplication::Init() Exception Break");
        throw;
    }
}
void CApplication::Load(int argc, char** argv)
{
    try
    {
        statistc_proxy::initialize();
        statistc_proxy::StatisticProxy::sendPacketFunctionPointer = global_function::SendPacketToDbmw;
        m_appConfig->Load_Table(argv[1]);
        m_serverConfig->Load_Table("./table/server_config.tbl");
        m_frame.InitFrameCountInfo(this, ((CAppConfig*)m_appConfig)->Get_FrameCountValue(), 0x3e8);
        m_udpHandler = new CUdpHandler;
        if (m_udpHandler->InitServerSocket(((CAppConfig*)m_appConfig)->Get_ServerUdpPort() & 0xffff) == -1)
        {
            throw CDNFException(
                std::string("CApplication::Load() Init Server Socket Exception Break!"));
        }
        m_serverHandler = new CServerHandler;
        m_serverHandler->Attach(this);
        m_serverHandler->Load(((CServerConfig*)m_serverConfig)->GetServerInfo());
        CPacketTranslater::attach(this);
        m_innerMsg = new CInnerMsgHandler;
        CPacketDecoderInstance()->Attach(this);
        puts("Application Load() Success!");
        m_loaded = true;
    }
    catch (CDNFException& e)
    {
        printf("CApplication::Load() Exception Break : %s\n", e.what());
        throw;
    }
    catch (...)
    {
        puts("CApplication::Load() Exception Break");
        throw;
    }
}
void CApplication::Free()
{
    try
    {
        puts("Application Free Start!");
        if (m_innerMsg != 0)
        {
            delete m_innerMsg;
            m_innerMsg = 0;
        }
        puts("UDP Handler Free Success!");
        if (m_serverHandler != 0)
        {
            delete m_serverHandler;
            m_serverHandler = 0;
        }
        puts("Game Server Handler Free Success!");
        if (m_udpHandler != 0)
        {
            ::operator delete(m_udpHandler);
            m_udpHandler = 0;
        }
        puts("UDP Handler Free Success!");
        CSignalTranslatorInstance()->clear();
        puts("Signal Translater Free Success!");
        if (m_appConfig != 0)
        {
            delete m_appConfig;
            m_appConfig = 0;
        }
        puts("Application Config Free Success!");
        if (m_serverConfig != 0)
        {
            delete m_serverConfig;
            m_serverConfig = 0;
        }
        puts("Application Server Config Free Success!");
        if (m_appInit != 0)
        {
            delete m_appInit;
            m_appInit = 0;
        }
        puts("Application Initor Free Success!");
        puts("application \xc1\xbe\xb7\xe1!");
    }
    catch (CDNFException& e)
    {
        printf("capplication::free() exception break : %s\n", e.what());
        throw;
    }
    catch (...)
    {
        puts("capplication::free() exception break");
        throw;
    }
}
void CApplication::Process()
{
    char buf[0x1800] = {0};
    PacketHeader* pkt = (PacketHeader*)buf;
    while (m_loaded)
    {
        try
        {
            DNFFLib::Sleep_Ext(0, 1);
            CFrameCountHandler* f = m_frame.GetFrameCountInfo();
            if (f->m_state != 0 && 1 < (unsigned char)f->m_state)
            {
                m_serverHandler->Process();
                m_frameLag.LoadSpec(m_serverHandler);
                if (f->m_state == 3)
                {
                    m_frame.SaveProcess();
                    m_hwSpec.DBSaveProcess(m_serverHandler);
                    m_statistic.DBSaveProcess(m_serverHandler);
                    m_frameLag.ReLoadSpec(m_serverHandler);
                    m_frameLag.SaveFrameLagData(m_serverHandler);
                    m_frameLag.RenewToday();
                    m_frameLag.SaveCollectedDirectxVersion(m_serverHandler);
                    m_frameLag.SaveUsedMemory(m_serverHandler);
                    m_udpChar.SaveUdpCharacteristicData(m_serverHandler,
                                                        m_frameLag.GetCollectInterval());
                }
            }
            memset(buf, 0, 0x1800);
            int len = 0x1800;
            unsigned short port;
            unsigned int ip;
            if (m_udpHandler->RecvFromClient(buf, &len, &ip, &port) != 1)
            {
                continue;
            }
            {
                if (pkt->packetSize != len)
                {
                    DNF_LOG_SCOPE_LINE(0x20c,"./log/recvErr",
                        "Packet Size is Incorrect! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                        pkt->packetSize, len, pkt->packetId);
                    continue;
                }
                if (0x17ff < pkt->packetSize)
                {
                    DNF_LOG_SCOPE_LINE(0x212,"./log/recvErr",
                        "Packet Size is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                        pkt->packetSize, len, pkt->packetId);
                    continue;
                }
                if (0x1800u < len)
                {
                    DNF_LOG_SCOPE_LINE(0x219,"./log/recvErr",
                        "Recv Byte is Over! Packet Size( %d ), Recv Byte( %d ) Code( %d )\n",
                        pkt->packetSize, len,
                        pkt->packetId);
                    continue;
                }
                pkt->m_connNo = ip;
                pkt->reversed1 = port;
                if (CPacketDecoderInstance()->MsgDecode(pkt) != 1)
                {
                    DNF_LOG_SCOPE_LINE(0x22a, "./log/recv",
                        "CPacketDecoder::MsgDecode() Undefined Packet Arrived Exception Break!");
                }
            }
        }
        catch (CDNFException& e)
        {
            printf("CApplication::Process() Exception Break : %s\n", e.what());
            DNF_LOG_SCOPE_LINE(0x234, "./log/process", "CApplication::Process() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            puts("CApplication::Process() Exception Break");
            DNF_LOG_SCOPE_LINE(0x239, "./log/process", "CApplication::Process() Exception Break\n");
        }
    }
}
void CApplication::AttachAppInitor(char** argv)
{
    const char* cmd = argv[2];
    if (strcmp(cmd, "start") == 0 || strcmp(cmd, "nofork") == 0)
    {
        m_appInit = new CAppStartInit;
        return;
    }
    if (strcmp(cmd, "stop") == 0)
    {
        m_appInit = new CAppStopInit;
        return;
    }
    throw CDNFException("CApplication::AttachAppInitor() \xbd\xc7\xc7\xe0 "
                        "\xbe\xc6\xb1\xd4\xb8\xd5\xc6\xae \xbf\xc0\xb7\xf9\n");
}
void CApplication::CheckArgv(int argc, char** argv)
{
    if (argc <= 2)
    {
        throw CDNFException("CApplication::CheckArgv() \xbd\xc7\xc7\xe0 "
                            "\xbe\xc6\xb1\xd4\xb8\xd5\xc6\xae \xbf\xc0\xb7\xf9\n");
    }
}
bool CApplication::Send_Term_Signal(const std::string& name)
{
    std::string path = "./pid/" + name + ".pid";
    FILE* f = fopen(path.c_str(), "r");
    if (f == 0)
    {
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", path.c_str());
    }
    else
    {
        int pid;
        fscanf(f, "%d", &pid);
        if (pid < 1)
        {
            fclose(f);
            printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        }
        else
        {
            if (kill(pid, 0xf) < 0)
            {
                fclose(f);
                printf("%d\xb9\xf8 process\xb7\xce \xc1\xbe\xb7\xe1 signal \xbc\xdb\xbd\xc3 "
                       "\xbd\xc7\xc6\xd0", pid);
            }
            else
            {
                fclose(f);
                if (remove(path.c_str()) == -1)
                {
                    puts("FAIL TO DELETE PID FILE ERROR");
                }
            }
        }
    }
    return 0;
}
void CApplication::Send_Suspend_Signal(const std::string& name)
{
    std::string path = "./pid/" + name + ".pid";
    FILE* f = fopen(path.c_str(), "r");
    if (f == 0)
    {
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", path.c_str());
    }
    else
    {
        int pid;
        fscanf(f, "%d", &pid);
        if (pid < 1)
        {
            fclose(f);
            printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        }
        else if (kill(pid, 10) < 0)
        {
            fclose(f);
            printf("%d\xb9\xf8 process\xb7\xce \xc1\xbe\xb7\xe1 signal \xbc\xdb\xbd\xc3 "
                   "\xbd\xc7\xc6\xd0", pid);
        }
        else
        {
            printf("SEND SUSPEND SIGNAL TO %d\n", pid);
            fclose(f);
        }
    }
}
void CApplication::App_Stop()
{
    StatisticManager* sm = Get_StatisticManager();
    sm->SendDBHellPartyStatisticItem(m_serverHandler);
    sm->ResetHellPartyStatisticItemMap();
    sm->SendDBPartyStatistic(m_serverHandler);
    sm->ResetPartyMap();
    sm->SendDBPartyJobStatistic(m_serverHandler);
    sm->ResetPartyJobMap();
    sm->SendDBPartyCharacStatistic(m_serverHandler);
    sm->ResetPartyCharacMap();
    sm->SendDBDeathTowerValueStatistic(m_serverHandler);
    sm->ResetDeathTowerValueMap();
    sm->SendDBDeathTowerPlayDataJobStatistic(m_serverHandler);
    sm->ResetDeathTowerPlayDataJobMap();
    sm->SendDBDeathTowerPlayDataPartyStatistic(m_serverHandler);
    sm->ResetDeathTowerPlayDataPartyMap();
    sm->SendDBFatigueBattery(m_serverHandler);
    sm->ResetFatigueBattery();
    sm->SendDBBloodDungeonStatistic(m_serverHandler);
    sm->ResetBloodDungeon();
    ((CCubeStatistic*)sm->getCubeStatisticObject())->sendStatisticData(m_serverHandler);
    ((CCubeStatistic*)sm->getCubeStatisticObject())->resetStatisticData();
    sm->SendDBValueStatistic(m_serverHandler);
    sm->ResetValueStatistic();
    sm->SendDBCirculationStatistic(m_serverHandler);
    sm->ResetCirculationStatistic();
    statistc_proxy::sendDBStatisticProxy();
    statistc_proxy::resetStatisticProxy();
    sm->SendDBSecretShopStatistic(m_serverHandler);
    sm->ResetSecretShopStatistic();
    sm->SendDBGoldcardEventStatistic(m_serverHandler);
    sm->ResetGoldcardEventStatistic();
    sm->SendDBTowerOfDespairStatistic(m_serverHandler);
    sm->ResetTowerOfDespair();
    sm->SendDBDisjointAvatarInfoTotal(m_serverHandler);
    sm->ResetDisjointAvatarInfoTotal();
    sm->SendDBP2PStatistic(m_serverHandler);
    sm->ResetP2PStatistic();
    m_loaded = 0;
}
unsigned char CApplication::Get_ServerGroup()
{
    return ((CAppConfig*)m_appConfig)->Get_ServerGroup();
}
void CApplication::SendTestPacket_2()
{
}
void CApplication::SendTestPacket_1()
{
    Packet_Monitor_Event_End end;
    end.m_fieldA = 9;
    CPacketTranslater::OnEventEnd((PacketHeader*)&end);
    Packet_Monitor_Event_Start start;
    start.m_fieldA = 9;
    start.m_fieldE = 4;
    start.m_field10 = 0;
    CPacketTranslater::OnEventStart((PacketHeader*)&start);
}
void CApplication::TranslateSignal()
{
}
CAppBase::~CAppBase()
{
}
void CAppBase::Process()
{
}
CAppBase::CAppBase()
{
}
StatisticManager* CApplication::Get_StatisticManager()
{
    return &m_statistic;
}
std::queue<CUdpRecvBuffer*>* CApplication::Get_QPacket()
{
    return &m_queue;
}
CMutex* CApplication::Get_QLock()
{
    return &m_lock1;
}
CMutex* CApplication::Get_BLock()
{
    return &m_lock2;
}
CServerHandler* CApplication::Get_ServerHandler()
{
    return m_serverHandler;
}
CHWSpecResearcher* CApplication::Get_HWspecResearch()
{
    return &m_hwSpec;
}
FrameLagCollector* CApplication::Get_FrameLagCollector()
{
    return &m_frameLag;
}
UdpCharacteristic* CApplication::Get_UdpCharacteristic()
{
    return &m_udpChar;
}
CUdpHandler* CApplication::Get_UdpHandler()
{
    return m_udpHandler;
}
