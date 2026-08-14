// df_manager_r — CAppBase / CApplication / ShowLogo / CVersionMgr / CSourceVersionMgr
//               （ORIG DNFApplication.cpp）
#include "DNFApplication.h"

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFKillUserConfig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFTableBase.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "DNFNetworkThread.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTranslater.h"
#include "DNFSignalTranslator.h"
#include "DNFTcpServerInterface.h"
#include "DNFUdpHandler.h"
#include "Packet_Monitor_Event_End.h"
#include "Packet_Monitor_Event_Start.h"
#include "Packet_Web_Prohibit_User_Connect.h"
#include "SystemTimeHandler.h"

CAppBase::CAppBase() {}
CAppBase::~CAppBase() {}
void CAppBase::Process() {}
void CAppBase::Create(int argc, char** argv)
{
    Init(argc, argv);
    Load(argc, argv);
}
void CAppBase::Clear()
{
    Free();
}

CApplication::CApplication()
    : m_loaded(0), m_field8(0), m_appInitor(0), m_appConfig(0),
      m_serverConfig(0), m_serverHandler(0), m_frameCount(),
      m_userManager(), m_killUsrConfig(0), m_udpSwapQueue(),
      m_udpHandler(0), m_udpNetworkThread(0), m_tcpNetSystem(),
      m_mutex22c(), m_mutex244()
{
}
CApplication::~CApplication()
{
    puts("Application Stop!");
}
void CApplication::App_Stop() { m_loaded = 0; }
void CApplication::SendTestPacket_1() {}
void CApplication::SendTestPacket_2() {}
void CApplication::Process()
{
    while (m_loaded)
    {
        try
        {
            CFrameCountHandler* info = (CFrameCountHandler*)m_frameCount.GetFrameCountInfo();
            if (info->m_field24 != 0 && info->m_field24 > 1)
            {
                m_serverHandler->Process();
                if (info->m_field24 == 3)
                {
                    m_frameCount.SaveProcess();
                    m_userManager.ProcessByMinute();
                }
            }
            SwitchQueueTCP();
            SwitchQueueUDP();
            CPacketDecoderInstance()->Process();
            DNFFLib::Sleep_Ext(0, 1);
        }
        catch (CDNFException& e)
        {
            printf("CApplication::Process() Exception Break : %s\n", e.what());
            DNF_LOG_SCOPE_LINE(0x22a, "./log/process.log", "CApplication::Process() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            puts("CApplication::Process() Exception Break");
            DNF_LOG_SCOPE_LINE(0x22f, "./log/process.log", "CApplication::Process() Exception Break\n");
        }
    }
}

void CApplication::Init(int argc, char** argv)
{
    try
    {
        ShowLogo();
        CheckArgv(argc, argv);
        CSignalTranslatorInstance()->init(this);
        AttachAppInitor(argv);
        m_appInitor->Init(this, argc, argv);
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
        m_appConfig->Load_Table(std::string(argv[1]));
        puts("Application App Config Load_Table() Success!");
        m_serverConfig->Load_Table(std::string("./table/server_config.tbl"));
        puts("Application Server Config Load_Table() Success!");
        m_frameCount.InitFrameCountInfo(this, (unsigned short)((CAppConfig*)m_appConfig)->Get_FrameCountValue(), 0x3e8);
        puts("Application Init Frame Count() Success!");
        m_udpHandler = new CUdpHandler;
        if (((CUdpHandler*)m_udpHandler)->InitServerSocket((unsigned short)((CAppConfig*)m_appConfig)->Get_ServerUdpPort()) == -1)
            throw CDNFException("CApplication::Load() Init Server Socket Exception Break!");
        puts("Application UDP Handler Create() Success!");
        m_serverHandler = new CServerHandler;
        m_serverHandler->Attach(this);
        m_serverHandler->Load(((CServerConfig*)m_serverConfig)->GetServerInfo());
        puts("Application Server Handler Create() Success!");
        CPacketTranslater::attach(this);
        puts("Application Packet Translater Attach() Success!");
        CPacketDecoderInstance()->Attach(this);
        puts("Application Packet Decoder Attach() Success!");
        m_udpNetworkThread = new CUdpNetworkThread;
        ((CUdpNetworkThread*)m_udpNetworkThread)->attach(this);
        if (!m_udpNetworkThread->CThreadInterface::begin())
            throw;
        puts("Application Network Thread Begin() Success!");
        IQueue<TcpRecvQueue>::Get().InitQueue(
            m_tcpNetSystem.Get_TcpSwapQPacket()->GetRecvQ(),
            m_tcpNetSystem.Get_TcpSwapQPacket()->GetParseQ());
        m_tcpNetSystem.Init(((CAppConfig*)m_appConfig)->Get_ServerTcpPort());
        m_userManager.Init(this);
        puts("Application Load() Success!");
        m_loaded = 1;
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
void CApplication::CheckArgv(int argc, char** argv)
{
    if (argc <= 2)
        throw CDNFException("CApplication::CheckArgv() start argument error\n");
}
void CApplication::AttachAppInitor(char** argv)
{
    char* arg = argv[2];
    if (strcmp(arg, "start") == 0 || strcmp(arg, "nofork") == 0)
        m_appInitor = new CAppStartInit;
    else if (strcmp(arg, "stop") == 0)
        m_appInitor = new CAppStopInit;
    else
        throw CDNFException("CApplication::AttachAppInitor() start argument error\n");
}
CUserManager* CApplication::Get_UserManager() { return &m_userManager; }
CServerHandler* CApplication::Get_ServerHandler() { return m_serverHandler; }
void* CApplication::Get_UdpHandler() { return m_udpHandler; }
CTcpNetSystem* CApplication::Get_TcpNetSystem() { return &m_tcpNetSystem; }
CMutex* CApplication::Get_UdpQLock() { return &m_mutex22c; }
CMutex* CApplication::Get_UdpBLock() { return &m_mutex244; }
UdpRecvQueue* CApplication::Get_UdpPacketRecvQ() { return m_udpSwapQueue.GetRecvQ(); }
UdpRecvQueue* CApplication::Get_UdpPacketParseQ() { return m_udpSwapQueue.GetParseQ(); }
bool CApplication::Send_Term_Signal(const std::string& msg)
{
    // ORIG 形态："./pid/" + msg + ".pid"（operator+(const char*, string) +
    // operator+(string, const char*)；包 std::string() 会改重载序列）。
    std::string filename = "./pid/" + msg + ".pid";
    FILE* f = fopen(filename.c_str(), "r");
    if (!f)
    {
        printf("%s process id file open Fail\n", filename.c_str());
        return 0;
    }
    int pid;
    fscanf(f, "%d", &pid);
    if (pid <= 0)
    {
        fclose(f);
        printf("%d no's wrong process id\n", pid);
        return 0;
    }
    int ret = kill(pid, 0xf);
    if (ret < 0)
    {
        fclose(f);
        printf("%d no's process end, signal send fail", pid);
        return 0;
    }
    fclose(f);
    if (remove(filename.c_str()) == -1)
        puts("FAIL TO DELETE PID FILE ERROR");
    return 0;
}

bool CApplication::Send_Suspend_Signal(const std::string& msg)
{
    std::string filename = "./pid/" + msg + ".pid";
    FILE* f = fopen(filename.c_str(), "r");
    if (!f)
    {
        printf("%s process id file open Fail\n", filename.c_str());
        return 0;
    }
    int pid;
    fscanf(f, "%d", &pid);
    if (pid <= 0)
    {
        fclose(f);
        printf("%d no's wrong process id\n", pid);
        return 0;
    }
    int ret = kill(pid, 0xa);
    if (ret < 0)
    {
        fclose(f);
        printf("%d no's process end, signal send fail", pid);
        return 0;
    }
    printf("SEND SUSPEND SIGNAL TO %d\n", pid);
    fclose(f);
    return 0;
}
CTcpServer* CApplication::FindTcpServer(unsigned int idx)
{
    return m_serverHandler->GetTcpServer(idx);
}
CMonitorServer* CApplication::FindMonitorServer(int idx)
{
    return m_serverHandler->GetMonitorServer(idx);
}

void CApplication::SwitchQueueTCP()
{
    CGuard<CMutex> guard(m_tcpNetSystem.Get_TcpRecvQLock());
    if (IQueue<TcpRecvQueue>::Get().SwitchQueue() == false)
    {
    }
    else
    {
        CPacketDecoderInstance()->SetTCPQueue(IQueue<TcpRecvQueue>::Get().GetParseQueue());
    }
}

void CApplication::SwitchQueueUDP()
{
    CGuard<CMutex> guard(&m_mutex22c);
    if (m_udpSwapQueue.GetRecvQ()->empty())
    {
    }
    else
    {
        m_udpSwapQueue.SwapQ();
        ((CUdpNetworkThread*)m_udpNetworkThread)->SetUDPQueue(m_udpSwapQueue.GetRecvQ());
        CPacketDecoderInstance()->SetUdpQueue(m_udpSwapQueue.GetParseQ());
    }
}

void CApplication::Free()
{
    try
    {
        puts("Application Free Start!");
        if (m_udpNetworkThread)
        {
            m_udpNetworkThread->stop();  // ORIG：先虚调 stop（vptr[0]），再 delete（D0）
            delete m_udpNetworkThread;
            m_udpNetworkThread = 0;
        }
        puts("Thread Free Success!");
        if (m_serverHandler)
        {
            delete m_serverHandler;
            m_serverHandler = 0;
        }
        puts("Game Server Handler Free Success!");
        if (m_udpHandler)
        {
            ::operator delete(m_udpHandler);
            m_udpHandler = 0;
        }
        puts("UDP Handler Free Success!");
        CSignalTranslatorInstance()->clear();
        puts("Signal Translater Free Success!");
        if (m_appConfig)
        {
            delete m_appConfig;
            m_appConfig = 0;
        }
        puts("Application Config Free Success!");
        if (m_serverConfig)
        {
            delete m_serverConfig;
            m_serverConfig = 0;
        }
        puts("Application Server Config Free Success!");
        if (m_appInitor)
        {
            delete m_appInitor;
            m_appInitor = 0;
        }
        puts("Application Initor Free Success!");
        puts("Application End!");
    }
    catch (CDNFException& e)
    {
        printf("CApplication::Free() Exception Break : %s\n", e.what());
        throw;
    }
    catch (...)
    {
        puts("CApplication::Free() Exception Break");
        throw;
    }
}

void CApplication::TranslateSignal()
{
    m_killUsrConfig->Clear_Table();
    m_killUsrConfig->Load_Table(std::string("./script/kill_user_config.tbl"));
    const std::vector<ST_KillUSRConfig*>* list =
        (const std::vector<ST_KillUSRConfig*>*)m_killUsrConfig->GetInfo();
    if (list->empty())
        return;
    for (std::vector<ST_KillUSRConfig*>::const_iterator it = list->begin();
         it != list->end(); ++it)
    {
        switch ((*it)->m_type)
        {
        case 4:
            m_serverConfig->Load_Table(std::string("./table/server_config.tbl"));
            m_serverHandler->Load(((CServerConfig*)m_serverConfig)->GetServerInfo());
            break;
        case 2:
        {
            Packet_Monitor_Event_Start pkt;
            pkt.m_eventCode = (*it)->m_field4;
            pkt.m_eventParam1 = (unsigned short)(*it)->m_field8;
            pkt.m_eventParam2 = (unsigned short)(*it)->m_fieldC;
            CPacketTranslater::OnEventStart(&pkt);
            break;
        }
        case 3:
        {
            Packet_Monitor_Event_End pkt;
            pkt.m_eventCode = (*it)->m_field4;
            CPacketTranslater::OnEventEnd((PacketHeader*)&pkt);
            break;
        }
        case 5:
        {
            Packet_Web_Prohibit_User_Connect pkt;
            pkt.m_fieldA = (*it)->m_field4;
            pkt.m_fieldE = (char)(*it)->m_field8;
            pkt.m_fieldF = (unsigned short)(*it)->m_fieldC;
            CPacketTranslater::OnWebNoticeProhibitConnectUser(&pkt);
            break;
        }
        }
    }
}

void ShowLogo()
{
    putchar('\n');
    puts("**********************************************************");
    puts("* +---   +-+ +  +----         DUNGEON & FIGHTER          *");
    puts("* |   |  | | |  |                                        *");
    puts("* |   |  | | |  +----                                    *");
    puts("* |   |  | | |  |      Open Beta Test on 2005.08.10~     *");
    puts("* +---   + +-+  |      Copyright(c) 2004,2005 Neople Co. *");
    puts("**********************************************************");
    CommonTime t;
    t.SetCurTime();
    printf("[!] Service Date (%02d-%02d-%02d/%02d:%02d)\n",
           t.m_year, t.m_mon, t.m_mday, t.m_hour, t.m_min);
}

// ---- CVersionMgr / CSourceVersionMgr（无独立 ORIG .o，随 DNFApplication.cpp）----
CVersionMgr::CVersionMgr(int a, int b, int c, int d)
{
    m_versions[0] = a;
    m_versions[1] = b;
    m_versions[2] = c;
    m_versions[3] = d;
}

CSourceVersionMgr::SourceVersion::SourceVersion(const SourceVersion& other)
    : m_name(other.m_name), m_version(other.m_version) {}

CSourceVersionMgr::SourceVersion& CSourceVersionMgr::SourceVersion::operator=(const SourceVersion& other)
{
    m_name = other.m_name;
    m_version = other.m_version;
    return *this;
}

CSourceVersionMgr::SourceVersion::~SourceVersion() {}

CSourceVersionMgr::CSourceVersionMgr()
{
    InsertSourceVersion(".svn/all-wcprops", 0x368f);
    InsertSourceVersion("DNFServerCommon/.svn/all-wcprops", 0x36c9);
}
CSourceVersionMgr::~CSourceVersionMgr() {}

void CSourceVersionMgr::InsertSourceVersion(char* name, int version)
{
    m_versions.push_back(SourceVersion(name, version));
}

CSourceVersionMgr::SourceVersion::SourceVersion(char* name, int version)
{
    m_name = name;
    m_version = version;
}
