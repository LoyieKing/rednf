// df_guild_r — DNFApplication (split from source/guild per ORIG layout)
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "DNFApplication.h"
#include "BlackUser.h"
#include "CashObject.h"
#include "DNFAppConfig.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFDBServer.h"
#include "DNFFloatingPointExceptSig.h"
#include "DNFGameServer.h"
#include "DNFGuild.h"
#include "DNFGuildManager.h"
#include "DNFGuildServerMain.h"
#include "DNFGuildWar.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMonitorServer.h"
#include "DNFNetworkThread.h"
#include "DNFPacketBuffer.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTracer.h"
#include "DNFPacketTranslater.h"
#include "DNFPowerWarConfig.h"
#include "DNFSegmentationFaultSig.h"
#include "DNFServerConfig.h"
#include "DNFServerHandler.h"
#include "DNFServerInterface.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFSystemFailSig.h"
#include "DNFTableBase.h"
#include "DNFTcpAcceptThread.h"
#include "DNFTcpHandler.h"
#include "DNFTcpNetworkThread.h"
#include "DNFTcpSocket.h"
#include "DNFTerminateSig.h"
#include "DNFThreadInterface.h"
#include "DNFTickHandler.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "DNFUser1Sig.h"
#include "DNFUserManager.h"
#include "DNFVersion.h"
#include "GMAccounts.h"
#include "GuildBoard.h"
#include "GuildCargo.h"
#include "MemPool.h"
#include "MemoryCashManager.h"
#include "PacketCounter.h"
#include "Peer.h"
#include "Power.h"
#include "PowerManager.h"
#include "PowerWar.h"
#include "PowerWarCharacInfo.h"
#include "PowerWarGuildInfo.h"
#include "Scheduler.h"
#include "ServerLoadChecker.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "TcpNetSystem.h"
#include "WebEvent.h"

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
    : m_loaded(false),
      m_reserved8(0),
      m_appInit(0),
      m_appConfig(0),
      m_serverConfig(0),
      m_killConfig(0),
      m_serverHandler(0),
      m_innerMsgHandler(0),
      m_udpHandler(0),
      m_udpThread(0)
{
}

CApplication::~CApplication()
{
    puts("Application Stop!");
}

void ShowLogo()
{
    putchar(10);
    puts("**********************************************************");
    puts("* +---   +-+ +  +----         DUNGEON & FIGHTER          *");
    puts("* |   |  | | |  |                                        *");
    puts("* |   |  | | |  +----                                    *");
    puts("* |   |  | | |  |      Open Beta Test on 2005.08.10~     *");
    puts("* +---   + +-+  |      Copyright(c) 2004,2005 Neople Co. *");
    puts("**********************************************************");
    CommonTime curTime;
    curTime.SetCurTime();
    printf("[!] Service Date (%02d-%02d-%02d/%02d:%02d)\n",
           curTime.m_time[0], curTime.m_time[1], curTime.m_time[2],
           curTime.m_time[3], curTime.m_time[4]);
}

void CApplication::Init(int argc, char** argv)
{
    try
    {
    ShowLogo();
    g_ServerString_.StrLoading();
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

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0010 | guild | 与ORIG差异=DIFF | CApplication::Load | 详见 function_reports/guild/_ZN12CApplication4LoadEiPPc.md
void CApplication::Load(int argc, char** argv)
{
    try
    {
    m_memoryCash = new CMemoryCashManager;
    m_memoryCash->Init(this);
    m_guildManager.Init(this);
    m_userManager.Init(this);
    m_appConfig->Load_Table(argv[1]);
    m_powerManager.InitPowerManager((char*)"./script/power_war_event.tbl", this);
    m_frameCount.InitFrameCountInfo(this, m_appConfig->Get_FrameCountValue(), 1000);
    m_udpHandler = new CUdpHandler;
    if (m_udpHandler->InitServerSocket(m_appConfig->Get_ServerUdpPort()) == -1)
    {
        throw CDNFException("CApplication::Load() Init UDP Server Socket Exception Break!");
    }
    m_serverHandler = new CServerHandler;
    m_serverHandler->Attach(this);
    m_serverHandler->Load(m_appConfig->GetServerInfoMap());
    CPacketTranslater::attach(this);
    m_innerMsgHandler = new CInnerMsgHandler;
    CPacketDecoderInstance()->Attach(this);
    m_udpThread = new CUdpNetworkThread;
    m_udpThread->attach(this);
    if (!((CThreadInterface*)m_udpThread)->begin())
    {
        throw;
    }
    m_tcpNetSystem.Init(m_appConfig->Get_ServerTcpPort());
    const char* dbIp = m_appConfig->Get_DBMWTcpIP();
    unsigned short dbPort = m_appConfig->Get_DBMWTcpPort();
    CTcpDBServer* db = m_serverHandler->GetTcpDBServer();
    if (*dbIp != '\0' && dbPort != 0)
    {
        db->Init(&m_tcpNetSystem, &m_guildManager);
        db->SetIP(std::string(dbIp));
        db->SetPort(dbPort);
        if (!m_tcpNetSystem.OpenTcpService(db->GetSockRef(), dbIp, dbPort))
        {
            printf("Application OpenTcpService(%s, %d) Fail!\n", dbIp,
                   (unsigned int)dbPort);
            DNF_LOG_SCOPE_LINE(0x175,"./log/TcpServer", "Application OpenTcpService(%s, %d, %d) Fail!",
                dbIp, (unsigned int)dbPort, db->GetSock());
        }
        else
        {
            DNF_LOG_SCOPE_LINE(0x179,"./log/TcpServer",
                "Application OpenTcpService(fd:%d,ip:%s,port:%d) Success!",
                db->GetSock(), dbIp, (unsigned int)dbPort);
            printf("Application OpenTcpService(fd:%d,ip:%s,port:%d) Success!\n",
                db->GetSock(), dbIp, (unsigned int)dbPort);
        }
    }
    else
    {
        puts("Application TCP cfg empty!");
        DNF_LOG_SCOPE_LINE(0x180, "./log/TcpServer", "Application TCP cfg empty!");
    }
    Packet_DB_Query_On_Guild_Booting pkt;
    pkt.m_group = Get_ServerGroup();
    m_serverHandler->SendToDB(&pkt);
    m_powerManager.SetPowerDBFlag(2);
    typedef std::queue<CTcpRecvBuffer*> TcpRecvQueue;
    IQueue<TcpRecvQueue>::Get()->InitQueue(
        ((CSwapQueue<TcpRecvQueue, 2>*)m_tcpNetSystem.Get_TcpSwapQPacket())->GetRecvQ(),
        ((CSwapQueue<TcpRecvQueue, 2>*)m_tcpNetSystem.Get_TcpSwapQPacket())->GetParseQ());
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
    m_guildManager.DBGuildAndGuildMemberSave(Get_ServerHandler());
    puts("Guild And Guild Member DB Save Success!");
    if (m_udpThread != 0)
    {
        m_udpThread->stop();
        delete m_udpThread;
        m_udpThread = 0;
    }
    puts("Udp Thread Free Success!");
    if (m_innerMsgHandler != 0)
    {
        delete m_innerMsgHandler;
        m_innerMsgHandler = 0;
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
            CFrameCountHandler* f = m_frameCount.GetFrameCountInfo();
            if (f->m_state != 0 && 1 < f->m_state)
            {
                m_serverHandler->Process();
                if (f->m_state == 3)
                {
                    m_frameCount.SaveProcess();
                    m_guildManager.DBGuildProcess(m_serverHandler, false);
                    m_guildManager.ProcessByMinute();
                    m_userManager.ProcessByMinute();
                    m_memoryCash->ProcessLifeTimeOut();
                }
                m_guildManager.ProcessBySecond();
            }
            SwitchQueueTCP();
            SwitchQueueUDP();
            CPacketDecoderInstance()->Process();
            DNFFLib::Sleep_Ext(0, 1);
        }
        catch (CDNFException& e)
        {
            printf("CApplication::Process() Exception Break : %s\n", e.what());
            DNF_LOG_SCOPE_LINE(0x248, "./log/process",
                "CApplication::Process() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            puts("CApplication::Process() Exception Break");
            DNF_LOG_SCOPE_LINE(0x24d, "./log/process",
                "CApplication::Process() Exception Break\n");
        }
    }
    puts("CApplication::Process() Exit");
    DNF_LOG_SCOPE_LINE(0x251, "./log/process", "CApplication::Process() Exit\n");
}

void CApplication::AttachAppInitor(char** argv)
{
    char* argv2 = argv[2];
    if (strcmp(argv2, "start") == 0 || strcmp(argv2, "nofork") == 0)
    {
        m_appInit = new CAppStartInit;
        return;
    }
    if (strcmp(argv2, "stop") == 0)
    {
        m_appInit = new CAppStopInit;
        return;
    }
    throw CDNFException("CApplication::AttachAppInitor() \xbd\xc7\xc7\xe0 \xbe\xc6\xb1\xd4\xb8\xd5\xc6\xae \xbf\xc0\xb7\xf9\n");
}

void CApplication::CheckArgv(int argc, char** argv)
{
    if (argc < 3)
    {
        throw CDNFException(
            "CApplication::CheckArgv() \xbd\xc7\xc7\xe0 \xbe\xc6\xb1\xd4\xb8\xd5\xc6\xae \xbf\xc0\xb7\xf9\n");
    }
}

bool CApplication::Send_Term_Signal(const std::string& name)
{
    return 0;
}

void CApplication::Send_Suspend_Signal(const std::string& name)
{
    std::string path = "./pid/" + name;
    FILE* fp = fopen(path.c_str(), "r");
    if (fp == 0)
    {
        printf("%s process id file open fail\n", path.c_str());
    }
    else
    {
        int pid = 0;
        fscanf(fp, "%d", &pid);
        if (pid < 1)
        {
            fclose(fp);
            printf("\x25\x64\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        }
        else
        {
            if (kill(pid, 10) < 0)
            {
                fclose(fp);
                printf("\x25\x64\xb9\xf8 process id\n", pid);
            }
            else
            {
                printf("SEND SUSPEND SIGNAL TO %d\n", pid);
                fclose(fp);
            }
        }
    }
}

CGameServer* CApplication::FindGameServer(int group)
{
    return m_serverHandler->GetGameServer((unsigned int)group);
}

CTcpGameServer* CApplication::FindTcpGameServer(unsigned int group)
{
    return m_serverHandler->GetTcpGameServer(group);
}

void CApplication::App_Stop()
{
    m_loaded = false;
    m_guildManager.DBGuildAndGuildMemberSave(Get_ServerHandler());
    puts("Guild And Guild Member DB Save Success!");
}

void CApplication::OnTcpGameServerDown(CTcpGameServer* server)
{
    m_userManager.DeleteUsersOnTcpGameServerDown(server);
}

void CApplication::OnGameServerDown(CGameServer* server)
{
    m_userManager.DeleteUsersOnGameServerDown(server);
}

void CApplication::Call_DeleteGuildMember(unsigned int group, CUser* user)
{
    m_guildManager.GuildMemLogout(group, user);
}

unsigned char CApplication::Get_ServerGroup()
{
    return m_appConfig->Get_ServerGroup();
}

void CApplication::SendTestPacket_2()
{
}

void CApplication::SendTestPacket_1()
{
}

// [DNF-NONIDENTICAL] DNF-GLD-DIFF-0009 | guild | 与ORIG差异=DIFF | CApplication::TranslateSignal | 详见 function_reports/guild/_ZN12CApplication15TranslateSignalEv.md
void CApplication::TranslateSignal()
{
    m_killConfig->Clear_Table();
    m_killConfig->Load_Table("./script/kill_user_config.tbl");
    const std::vector<ST_KillUSRConfig*>* vec = m_killConfig->GetInfo();
    if (vec->empty())
    {
        return;
    }
    for (std::vector<ST_KillUSRConfig*>::const_iterator it = vec->begin(); it != vec->end(); ++it)
    {
        switch ((*it)->m_type)
        {
        case 1:
            Get_GuildManager()->DBGuildProcess(Get_ServerHandler(), true);
            break;
            case 3:
            {
                Packet_Monitor_Send_Guild_Mail mail;
                mail.m_charNo = (unsigned int)(*it)->m_param1;
                mail.m_guildId = (unsigned int)(*it)->m_param2;
                memcpy(mail.m_msg,
                       "\xc5\xc2\xbd\xba\xc6\xae \xb1\xe6\xb5\xe5\xb8\xde\xc0\xcf\xc0\xd4\xb4\xcf\xb4\xd9.",
                       0x17);
                CPacketTranslater::OnMonitorSendGuildLetter(&mail);
                break;
            }
            case 4:
                m_serverHandler->Load(m_appConfig->GetServerInfoMap());
                break;
            case 7:
            {
                Packet_Monitor_Notice_Guild_Enter enter;
                enter.m_info.m_guildKey = (unsigned int)(*it)->m_param1;
                enter.m_info.m_dbid = (unsigned int)(*it)->m_param2;
                enter.m_info.m_charNo = (unsigned int)(*it)->m_param3;
                memcpy(enter.m_info.m_guildName,
                       "\xb4\xab\xbb\xe7\xb6\xf7\x00\xbe\xdf\xb8\xde\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
                       0x16);
                CPacketTranslater::OnNoticeGuildEnter(&enter);
                break;
            }
            case 8:
            {
                Packet_Monitor_Set_GuildMember_Grade_FromWeb grade;
                grade.m_masterCharNo = (unsigned int)(*it)->m_param1;
                grade.m_guildKey = 2;  // 与 ORIG 一致：guildKey 恒为 2
                grade.m_newGrade = (unsigned char)(*it)->m_param3;
                grade.m_targetCharNo = (unsigned int)(*it)->m_param2;
                CPacketTranslater::OnSetGuildMemberGradeFromWeb(&grade);
                break;
            }
            case 9:
            {
                Packet_Guild_Master_Delegate_FromWeb delegate;
                delegate.m_guildKey = (unsigned int)(*it)->m_param1;
                delegate.m_requesterCharNo = (unsigned int)(*it)->m_param2;
                delegate.m_delegateeCharNo = (unsigned int)(*it)->m_param3;
                memcpy(delegate.m_msg, "\xbe\xdf\xb8\xde", 4);
                CPacketTranslater::OnGuildMasterDelegateFromWeb(&delegate);
                break;
            }
            case 0xb:
                m_powerManager.LoadPowerWarCfg("./script/power_war_event.tbl");
                break;
            case 0xc:
                m_powerManager.StartPowerWarEvent();
                {
                    Packet_Monitor_Event_Start start;
                    start.m_eventType = 0x1e;
                    m_serverHandler->SendAllTcpGameServer(&start);
                }
                break;
            case 0xd:
                m_powerManager.SendPowerWarScore();
                m_powerManager.EndPowerWarEvent();
                {
                    Packet_Monitor_Event_End end;
                    end.m_eventType = 0x1e;
                    m_serverHandler->SendAllTcpGameServer(&end);
                }
                break;
            case 0xe:
                Get_TcpNetSystem()->CleanTcpSendPacketQ();
                break;
            case 0xf:
                m_guildManager.CargoLock();
                break;
            case 0x10:
                m_guildManager.CargoUnlock();
                break;
            default:
                break;
            }
        }
    }

void CApplication::SwitchQueueTCP()
{
    CGuard<CMutex> guard(m_tcpNetSystem.Get_TcpRecvQLock());
    typedef std::queue<CTcpRecvBuffer*> TcpRecvQueue;
    if (!IQueue<TcpRecvQueue>::Get()->SwitchQueue())
        return;
    CPacketDecoderInstance()->SetTCPQueue(
        IQueue<TcpRecvQueue>::Get()->GetParseQueue());
}

void CApplication::SwitchQueueUDP()
{
    CGuard<CMutex> guard(&m_udpQLock);
    typedef std::queue<CUdpRecvBuffer*> UdpRecvQueue;
    if (m_swapQueue.GetRecvQ()->empty())
        return;
    m_swapQueue.SwapQ();
    m_udpThread->SetUDPQueue(
        m_swapQueue.GetRecvQ());
    CPacketDecoderInstance()->SetUdpQueue(
        m_swapQueue.GetParseQ());
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

Packet_Monitor_Notice_Guild_Enter::Packet_Monitor_Notice_Guild_Enter()
    : PacketHeader(0x3fd, 0x50)
{
}

Packet_Monitor_Set_GuildMember_Grade_FromWeb::Packet_Monitor_Set_GuildMember_Grade_FromWeb()
    : PacketHeader(0x441, 0x17)
{
    (void)0;
}

Packet_Monitor_Event_Start::Packet_Monitor_Event_Start()
    : PacketHeader(0x44f, 0x12)
{
    (void)0;
}

Packet_Monitor_Event_End::Packet_Monitor_Event_End()
    : PacketHeader(0x450, 0xe)
{
    (void)0;
}

Packet_DB_Query_On_Guild_Booting::Packet_DB_Query_On_Guild_Booting()
    : PacketHeader(0x677, 0xb)
{
    (void)0;
}

Packet_Monitor_Send_Guild_Mail::Packet_Monitor_Send_Guild_Mail()
    : PacketHeader(0x432, 0x112)
{
    memset(m_msg, 0, 0x100);
}

Packet_Guild_Master_Delegate_FromWeb::Packet_Guild_Master_Delegate_FromWeb()
    : PacketHeader(0x442, 0x34)
{
    memset(m_msg, 0, 0x1e);
}

CTcpNetSystem* CApplication::Get_TcpNetSystem()
{
    return &m_tcpNetSystem;
}

CServerHandler* CApplication::Get_ServerHandler()
{
    return m_serverHandler;
}

CGuildManager* CApplication::Get_GuildManager()
{
    return &m_guildManager;
}

void* CApplication::Get_UdpPacketParseQ()
{
    return m_swapQueue.GetParseQ();
}

CMutex* CApplication::Get_UdpQLock()
{
    return &m_udpQLock;
}

CMutex* CApplication::Get_UdpBLock()
{
    return &m_udpBLock;
}

CUdpHandler* CApplication::Get_UdpHandler()
{
    return m_udpHandler;
}

CUserManager* CApplication::Get_UserManager()
{
    return &m_userManager;
}

CMemoryCashManager* CApplication::Get_MemoryCashManager()
{
    return m_memoryCash;
}

WongWork::CGMAccounts* CApplication::GetGMAccounts()
{
    return m_gmAccounts;
}

CPowerManager* CApplication::GetPowerManager()
{
    return &m_powerManager;
}

void* CApplication::Get_UdpPacketRecvQ()
{
    return m_swapQueue.GetRecvQ();
}
