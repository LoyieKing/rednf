// df_coserver_r — CAppBase/CApplication/main/ShowLogo（Ghidra 反汇编还原）
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "DNFApplication.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFKillUserConfig.h"
#include "DNFNetworkThread.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTranslater.h"
#include "DNFGameServer.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFSignalTranslator.h"
#include "DNFTableBase.h"
#include "DNFUdpHandler.h"
#include "SystemTimeHandler.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

CAppBase::CAppBase()
{
}

CAppBase::~CAppBase()
{
}

void CAppBase::Process()
{
}

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

int main(int argc, char** argv)
{
    CApplication* app = CApplicationInstance();
    try
    {
        app->Create(argc, argv);
        app->Process();
        app->Clear();
    }
    catch (...)
    {
        app->Clear();
    }
    return 1;
}

CApplication::CApplication()
    : m_loaded(0), m_reserved1(0), m_appInit(0), m_appConfig(0), m_serverConfig(0),
      m_killUsrConfig(0), m_serverHandler(0), m_innerMsg(0)
{
    for (int i = 0; i < 0x65; i++)
    {
        m_udpHandlers[i] = 0;
        m_netThreads[i] = 0;
    }
}

CApplication::~CApplication()
{
    puts("Application Stop!");
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
        m_userManager.Init(this);
        m_appConfig->Load_Table(argv[1]);
        m_serverConfig->Load_Table("./table/server_config.tbl");
        m_frame.InitFrameCountInfo(this, m_appConfig->Get_FrameCountValue(), 1000);
        m_serverHandler = new CServerHandler;
        m_serverHandler->Attach(this);
        m_serverHandler->Load(m_serverConfig->GetServerInfo());
        CPacketTranslater::attach(this);
        m_innerMsg = new CInnerMsgHandler;
        CPacketDecoderInstance()->Attach(this);
        for (int i = 0; i < 10; i++)
        {
            m_appThreads[i] = new CAppThread;
            m_appThreads[i]->attach(this, i);
            if (!m_appThreads[i]->begin())
            {
                throw CDNFException(
                    std::string("CApplication::Load() Init App Thread Exception Break!"));
            }
        }
        for (int i = 0; i <= 100; i++)
        {
            unsigned short port = m_appConfig->Get_ServerUdpPort((unsigned char)i);
            if (port != 0)
            {
                m_udpHandlers[i] = new CUdpHandler;
                if (m_udpHandlers[i]->InitServerSocket((unsigned int)port) == -1)
                {
                    throw CDNFException(
                        std::string("CApplication::Load() Init Server Socket Exception Break!"));
                }
                m_netThreads[i] = new CNetworkThread;
                m_netThreads[i]->attach(this, i);
                if (!m_netThreads[i]->begin())
                {
                    throw CDNFException(
                        std::string("CApplication::Load() Init Network Thread Exception Break!"));
                }
            }
        }
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
        for (int i = 0; i < 0x65; i++)
        {
            if (m_netThreads[i] != 0)
            {
                m_netThreads[i]->stop();
                delete m_netThreads[i];
                m_netThreads[i] = 0;
            }
            if (m_udpHandlers[i] != 0)
            {
                ::operator delete(m_udpHandlers[i]);
                m_udpHandlers[i] = 0;
            }
        }
        puts("UDP Handler Free Success!");
        puts("Thread Free Success!");
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
        puts("Application \xc1\xbe\xb7\xe1!");
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

void CApplication::Process()
{
    while (m_loaded)
    {
        try
        {
            CFrameCountHandler* f = m_frame.GetFrameCountInfo();
            if (f->m_state != 0 && 1 < (unsigned char)f->m_state)
            {
                m_serverHandler->Process();
                if (f->m_state == 3)
                {
                    m_frame.SaveProcess(9999);
                }
            }
            DNFFLib::Sleep_Ext(0, 1);
        }
        catch (CDNFException& e)
        {
            printf("CApplication::Process() Exception Break : %s\n", e.what());
            DNF_LOG_SCOPE_LINE(0x1ec, "./log/process", "CApplication::Process() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            puts("CApplication::Process() Exception Break");
            DNF_LOG_SCOPE_LINE(0x1f1, "./log/process", "CApplication::Process() Exception Break\n");
        }
    }
}

void CApplication::CheckArgv(int argc, char** argv)
{
    if (argc <= 2)
    {
        throw CDNFException("CApplication::CheckArgv() \xbd\xc7\xc7\xe0 "
                            "\xbe\xc6\xb1\xd4\xb8\xd5\xc6\xae \xbf\xc0\xb7\xf9\n");
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

CGameServer* CApplication::FindGameServer(int group, int channel)
{
    if (group > 0x64 || channel > 0xfe)
    {
        return 0;
    }
    return m_serverHandler->GetGameServer(group * 0xff + channel);
}

void CApplication::App_Stop()
{
    m_loaded = 0;
}

void CApplication::SendTestPacket_1()
{
    Packet_Monitor_Event_End end;
    int x = 9;
    (void)x;
    Packet_Monitor_Event_Start start;
    start.m_field1 = 9;
    start.m_field2 = 4;
    start.m_field3 = 0;
}

void CApplication::SendTestPacket_2()
{
}

void CApplication::TranslateSignal()
{
    m_killUsrConfig->Clear_Table();
    m_killUsrConfig->Load_Table("./script/kill_user_config.tbl");
    std::vector<ST_KillUSRConfig*>* v = m_killUsrConfig->GetInfo();
    if (v->empty())
        return;
}

std::queue<CUdpRecvBuffer*>* CApplication::Get_QPacket()
{
    return m_udpQueues;
}

CMutex* CApplication::Get_QLock()
{
    return m_udpLocks;
}

CMutex* CApplication::Get_BLock()
{
    return &m_lock1;
}

CUdpHandler* CApplication::Get_UdpHandler(int idx)
{
    if (idx < 0x65)
    {
        return m_udpHandlers[idx];
    }
    return 0;
}

CUserManager* CApplication::Get_UserManager()
{
    return &m_userManager;
}
