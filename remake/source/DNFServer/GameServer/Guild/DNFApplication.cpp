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
    Process();
}

void CAppBase::Clear()
{
}

CApplication* CApplicationInstance()
{
    static CApplication instance;
    return &instance;
}

CApplication::CApplication()
{
    m_loaded = false;
    m_reserved8 = 0;
    m_appInit = 0;
    m_appConfig = 0;
    m_serverConfig = 0;
    m_killConfig = 0;
    m_serverHandler = 0;
    m_innerMsgHandler = 0;
    m_udpHandler = 0;
    m_udpThread = 0;
    m_memoryCash = 0;
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
    ShowLogo();
    g_ServerString_.StrLoading();
    CheckArgv(argc, argv);
    CSignalTranslator* st = CSignalTranslatorInstance();
    st->init(this);
    AttachAppInitor(argv);
    if (m_appInit != 0)
    {
        m_appInit->Init(this, argc, argv);
    }
    puts("Application Init() Success!");
}

void CApplication::Load(int argc, char** argv)
{
    m_memoryCash = new CMemoryCashManager;
    m_memoryCash->Init(this);
    m_guildManager.Init(this);
    m_userManager.Init(this);
    if (m_appConfig != 0)
    {
        m_appConfig->Load_Table(argv[1]);
    }
    m_powerManager.InitPowerManager((char*)"./script/power_war_event.tbl", this);
    if (m_appConfig != 0)
    {
        m_frameCount.InitFrameCountInfo(this, (unsigned int)this,
                                        m_appConfig->Get_FrameCountValue());
    }
    m_udpHandler = new CUdpHandler;
    if (m_appConfig != 0)
    {
        unsigned short port = m_appConfig->Get_ServerUdpPort();
        if (m_udpHandler->InitServerSocket(port) == -1)
        {
            throw CDNFException("CApplication::Load() Init UDP Server Socket Exception Break!");
        }
    }
    m_serverHandler = new CServerHandler;
    m_serverHandler->Attach(this);
    if (m_appConfig != 0)
    {
        m_serverHandler->Load(m_appConfig->GetServerInfoMap());
    }
    CPacketTranslater::attach(this);
    m_innerMsgHandler = new CInnerMsgHandler;
    CPacketDecoderInstance()->Attach(this);
    m_udpThread = new CUdpNetworkThread;
    m_udpThread->attach(this);
    if (!((CThreadInterface*)m_udpThread)->begin())
    {
        throw CDNFException("CApplication::Load() UdpThread begin Fail!");
    }
    if (m_appConfig != 0)
    {
        m_tcpNetSystem.Init(m_appConfig->Get_ServerTcpPort());
    }
    if (m_appConfig != 0)
    {
        const char* dbIp = m_appConfig->Get_DBMWTcpIP();
        unsigned short dbPort = m_appConfig->Get_DBMWTcpPort();
        CTcpDBServer* db = m_serverHandler->GetTcpDBServer();
        if (*dbIp == '\0' || dbPort == 0)
        {
            puts("Application TCP cfg empty!");
            DNF_LOG_SCOPE_LINE(0x180, "./log/TcpServer", "Application TCP cfg empty!");
        }
        else
        {
            db->Init(&m_tcpNetSystem, &m_guildManager);
            db->SetIP(std::string(dbIp));
            db->SetPort(dbPort);
            int sock = 0;
            if (m_tcpNetSystem.OpenTcpService(sock, dbIp, dbPort))
            {
                printf("Application OpenTcpService(fd:%d,ip:%s,port:%d) Success!\n",
                       db->GetSock(), dbIp, (unsigned int)dbPort);
                DNF_LOG_SCOPE_LINE(0x179,"./log/TcpServer",
                    "Application OpenTcpService(fd:%d,ip:%s,port:%d) Success!",
                    db->GetSock(), dbIp, (unsigned int)dbPort);
            }
            else
            {
                printf("Application OpenTcpService(%s, %d) Fail!\n", dbIp,
                       (unsigned int)dbPort);
                DNF_LOG_SCOPE_LINE(0x175,"./log/TcpServer", "Application OpenTcpService(%s, %d, %d) Fail!",
                    dbIp, (unsigned int)dbPort, db->GetSock());
            }
        }
    }
    Packet_DB_Query_On_Guild_Booting pkt;
    *(unsigned char*)((char*)&pkt + 0xa) = Get_ServerGroup();
    m_serverHandler->SendToDB(&pkt);
    m_powerManager.SetPowerDBFlag(2);
    typedef std::queue<CTcpRecvBuffer*> TcpRecvQueue;
    IQueue<TcpRecvQueue>::Get()->InitQueue(
        ((CSwapQueue<TcpRecvQueue, 2>*)m_tcpNetSystem.Get_TcpSwapQPacket())->GetRecvQ(),
        ((CSwapQueue<TcpRecvQueue, 2>*)m_tcpNetSystem.Get_TcpSwapQPacket())->GetParseQ());
    m_loaded = true;
    puts("Application Load() Success!");
}

void CApplication::Free()
{
    try
    {
    puts("Application Free Start!");
    CServerHandler* handler = Get_ServerHandler();
    m_guildManager.DBGuildAndGuildMemberSave(handler);
    puts("Guild And Guild Member DB Save Success!");
    if (m_udpThread != 0)
    {
        m_udpThread->stop();
        if (m_udpThread != 0)
        {
            delete m_udpThread;
        }
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
    CSignalTranslator* st = CSignalTranslatorInstance();
    st->clear();
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
            if (f->m_field24 != 0 && 1 < (unsigned char)f->m_field24)
            {
                m_serverHandler->Process();
                if ((unsigned char)f->m_field24 == 3)
                {
                    f->SaveProcess();
                    m_guildManager.DBGuildProcess(m_serverHandler, false);
                    m_guildManager.ProcessByMinute();
                    m_userManager.ProcessByMinute();
                    m_memoryCash->ProcessLifeTimeOut();
                }
                m_guildManager.ProcessBySecond();
            }
            SwitchQueueTCP();
            SwitchQueueUDP();
            CPacketDecoder* dec = CPacketDecoderInstance();
            dec->Process();
            DNFFLib::Sleep_Ext(0, 1);
        }
        catch (CDNFException& e)
        {
            printf("%s\n", e.what());
            DNF_LOG_SCOPE_LINE(0x248, "./log/process", "%s\n", e.what());
        }
        catch (...)
        {
            puts("CApplication::Process() Exception Break");
            DNF_LOG_SCOPE_LINE(0x24d, "./log/process", "CApplication::Process() Exception Break");
        }
    }
    puts("CApplication::Process() Exit");
    DNF_LOG_SCOPE_LINE(0x251, "./log/process", "CApplication::Process() Exit\n");
}

void CApplication::AttachAppInitor(char** argv)
{
    if (argv[2] == 0)
    {
        throw CDNFException("CApplication::AttachAppInitor() invalid argv[2]!");
    }
    if (strcmp(argv[2], "start") == 0 || strcmp(argv[2], "nofork") == 0)
    {
        m_appInit = new CAppStartInit;
        return;
    }
    if (strcmp(argv[2], "stop") == 0)
    {
        m_appInit = new CAppStopInit;
        return;
    }
    throw CDNFException("CApplication::AttachAppInitor() invalid mode!");
}

void CApplication::CheckArgv(int argc, char** argv)
{
    if (argc < 3)
    {
        throw CDNFException(
            "CApplication::CheckArgv() \xbd\xc7\xc7\xe0 \xbe\xc6\xb1\xd4\xb8\xd5\xc6\xae \xbf\xc0\xb7\xf9\n");
    }
}

int CApplication::Send_Term_Signal(const std::string& name)
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
    return m_serverHandler ? m_serverHandler->GetTcpGameServer(group) : 0;
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
    return m_appConfig ? m_appConfig->Get_ServerGroup() : 0;
}

void CApplication::SendTestPacket_2()
{
}

void CApplication::SendTestPacket_1()
{
}

void CApplication::TranslateSignal()
{
    m_killConfig->Clear_Table();
    m_killConfig->Load_Table("./script/kill_user_config.tbl");
    std::vector<ST_KillUSRConfig*>* vec = m_killConfig->GetInfo();
    if (!vec->empty())
    {
        for (std::vector<ST_KillUSRConfig*>::iterator it = vec->begin(); it != vec->end(); ++it)
        {
            ST_KillUSRConfig* cfg = *it;
            switch (cfg->m_field0)
            {
            case 1:
                m_guildManager.DBGuildProcess(Get_ServerHandler(), (bool)Get_ServerHandler());
                break;
            case 3:
            {
                Packet_Monitor_Send_Guild_Mail mail;
                *(unsigned int*)((char*)&mail + 0xa) = (unsigned int)cfg->m_field1;
                *(unsigned int*)((char*)&mail + 0xe) = (unsigned int)cfg->m_field2;
                memcpy((char*)&mail + 0x12,
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
                *(unsigned int*)((char*)&enter + 0xa) = (unsigned int)cfg->m_field1;
                *(unsigned int*)((char*)&enter + 0xe) = (unsigned int)cfg->m_field2;
                *(unsigned int*)((char*)&enter + 0x12) = (unsigned int)cfg->m_field3;
                memcpy((char*)&enter + 0x16,
                       "\xb4\xab\xbb\xe7\xb6\xf7\x00\xb0\xde\xdf\xb8\xde\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",
                       0x16);
                CPacketTranslater::OnNoticeGuildEnter(&enter);
                break;
            }
            case 8:
            {
                Packet_Monitor_Set_GuildMember_Grade_FromWeb grade;
                *(unsigned int*)((char*)&grade + 0x12) = 2;
                *(unsigned int*)((char*)&grade + 0xa) = (unsigned int)cfg->m_field1;
                *(unsigned int*)((char*)&grade + 0xe) = (unsigned int)cfg->m_field2;
                *(unsigned char*)((char*)&grade + 0x16) = (unsigned char)cfg->m_field3;
                CPacketTranslater::OnSetGuildMemberGradeFromWeb(&grade);
                break;
            }
            case 9:
            {
                Packet_Guild_Master_Delegate_FromWeb delegate;
                *(unsigned int*)((char*)&delegate + 0xa) = (unsigned int)cfg->m_field1;
                *(unsigned int*)((char*)&delegate + 0xe) = (unsigned int)cfg->m_field2;
                *(unsigned int*)((char*)&delegate + 0x12) = (unsigned int)cfg->m_field3;
                memcpy((char*)&delegate + 0x16, "\xb0\xde\xdf\xb8", 4);
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
                    *(unsigned int*)((char*)&start + 0xa) = 0x1e;
                    m_serverHandler->SendAllTcpGameServer(&start);
                }
                break;
            case 0xd:
                m_powerManager.SendPowerWarScore();
                m_powerManager.EndPowerWarEvent();
                {
                    Packet_Monitor_Event_End end;
                    *(unsigned int*)((char*)&end + 0xa) = 0x1e;
                    m_serverHandler->SendAllTcpGameServer(&end);
                }
                break;
            case 0xe:
                m_tcpNetSystem.CleanTcpSendPacketQ();
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
}

void CApplication::SwitchQueueTCP()
{
    CGuard<CMutex> guard(m_tcpNetSystem.Get_TcpRecvQLock());
    typedef std::queue<CTcpRecvBuffer*> TcpRecvQueue;
    IQueue<TcpRecvQueue>* q = IQueue<TcpRecvQueue>::Get();
    if (q->SwitchQueue())
    {
        CPacketDecoder* dec = CPacketDecoderInstance();
        dec->SetTCPQueue(q->GetParseQueue());
    }
}

void CApplication::SwitchQueueUDP()
{
    CGuard<CMutex> guard(&m_udpQLock);
    typedef std::queue<CUdpRecvBuffer*> UdpRecvQueue;
    CSwapQueue<UdpRecvQueue, 2>* sq = (CSwapQueue<UdpRecvQueue, 2>*)((char*)this + 0xa0);
    UdpRecvQueue* recvQueue = sq->GetRecvQ();
    if (!recvQueue->empty())
    {
        sq->SwapQ();
        m_udpThread->SetUDPQueue(sq->GetRecvQ());
        CPacketDecoder* dec = CPacketDecoderInstance();
        dec->SetUdpQueue(sq->GetParseQ());
    }
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
    new ((char*)this + 0x10) ST_Notice_Guild_Enter;
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
    memset((char*)this + 0x12, 0, 0x100);
}

Packet_Guild_Master_Delegate_FromWeb::Packet_Guild_Master_Delegate_FromWeb()
    : PacketHeader(0x442, 0x34)
{
    memset((char*)this + 0x16, 0, 0x1e);
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

