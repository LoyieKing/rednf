#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();

void ShowLogo();
CQueryCounter* CQueryCounterInstance();
CPacketTracer* CPacketTracerInstance();
CPacketDecoder* CPacketDecoderInstance();
CSignalTranslator* CSignalTranslatorInstance();
int get_awardItem_using_interval();
void get_awardItem_using_interval(int monthInterval);
// df_dbmw_r - DNFApplication (ORIG DNFApplication.cpp)

#include <signal.h>
#include <cstdio>
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

CAppBase::CAppBase() {}
CAppBase::~CAppBase() {}
void CAppBase::Process() {}
void CAppBase::Create(int argc, char** argv)
{
    Init(argc, argv);
    if (strcmp(argv[2], "stop") != 0)
        Load(argc, argv);
}
void CAppBase::Clear()
{
    Free();
}
CApplication::CApplication()
    : m_loaded(0), m_field8(0), m_appInitor(0), m_appConfig(0),
      m_serverConfig(0), m_serverHandler(0), m_killUsrConfig(0),
      m_udpHandler(0), m_networkThread(0), m_guildManager(0)
{
}
CApplication::~CApplication()
{
    puts("Application Stop!");
}
void CApplication::App_Stop() { m_loaded = 0; }
void CApplication::SendTestPacket_1() {}
void CApplication::SendTestPacket_2() {}
void* CApplication::GetGMAccounts() { return m_gmAccounts; }
CGameServer* CApplication::FindGameServer(int idx)
{
    return m_serverHandler->GetGameServer(idx);
}
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
                    CQueryCounter* counter = CQueryCounterInstance();
                    counter->WriteDBLog(m_dbManager);
                    CPacketTracerInstance()->WritePacketProcessLog();
                }
            }
            SwitchQueueTCP();
            SwitchQueueUDP();
            CPacketDecoderInstance()->Process();
            DNFFLib::Sleep_Ext(0, 1);
        }
        DNF_CATCH_LOG_PRINTF("./log/process.log", "CApplication::Process() Exception Break", 0x315, 0x31a);
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
        g_ServerString_.StrLoading();
    }
    DNF_CATCH_RETHROW("CApplication::Init() Exception Break");
}
// [DNF-NONIDENTICAL] DNF-DBM-DIFF-0069 | dbmw | 与ORIG差异=DIFF | CApplication::Load | 详见 function_reports/dbmw/_ZN12CApplication4LoadEiPPc.md
void CApplication::Load(int argc, char** argv)
{
    try
    {
        m_appConfig->Load_Table(std::string(argv[1]));
        puts("Application App Config Load_Table() Success!");
        m_serverConfig->Load_Table(std::string("./table/server_config.tbl"));
        puts("Application Server Config Load_Table() Success!");
        m_frameCount.InitFrameCountInfo(this,
                                        ((CAppConfig*)m_appConfig)->Get_FrameCountValue(),
                                        0x3e8);
        puts("Application Init Frame Count() Success!");
        m_udpHandler = new CUdpHandler;
        if (((CUdpHandler*)m_udpHandler)->InitServerSocket(
                (unsigned short)((CAppConfig*)m_appConfig)->Get_ServerUdpPort()) == -1)
            throw CDNFException("CApplication::Load() Init Server Socket Exception Break!");
        puts("Application UDP Handler Create() Success!");
        m_serverHandler = new CServerHandler;
        m_serverHandler->Attach(this);
        m_serverHandler->Load((ST_ServerInfo*)((CServerConfig*)m_serverConfig)->GetServerInfo());
        puts("Application Server Handler Create() Success!");
        CPacketTranslater::attach(this);
        puts("Application Packet Translater Attach() Success!");
        CPacketDecoderInstance()->Attach(this);
        puts("Application Packet Decoder Attach() Success!");
        // TODO(dbmw): InitDB() 尚未按原版 0x0806d25c 还原
        if (InitDB() != 1)
        {
            puts("DB Open Fail");
            throw;
        }
        m_guildManager = new CGuildManager;
        m_networkThread = new CNetworkThread;
        m_networkThread->attach(this);
        if (m_networkThread->begin() != 1)
            throw;
        puts("Application Network Thread Begin() Success!");
        unsigned short port = ((CAppConfig*)m_appConfig)->Get_ServerTcpPort();
        if (port != 0)
        {
            m_tcpNetSystem.Init(port);
        }
        else
        {
            puts("TCP Server Unused");
            DNF_LOG_SCOPE_LINE(0x251, "./log/TcpServer", "TCP Server Unused");
        }
        m_gmAccounts = new WongWork::CGMAccounts;
        IQueue<TcpRecvQueue>::Get().InitQueue(
            m_tcpNetSystem.Get_TcpSwapQPacket()->GetRecvQ(),
            m_tcpNetSystem.Get_TcpSwapQPacket()->GetParseQ());
        puts("Application Load() Success!");
        m_loaded = 1;
    }
    DNF_CATCH_RETHROW("CApplication::Load() Exception Break");
}
void CApplication::CheckArgv(int argc, char** argv)
{
    if (argc <= 2)
        throw CDNFException("CApplication::CheckArgv() start argument error\n");
}
bool CApplication::InitDB()
{
    m_dbManager.Init((ENUM_DB_KIND)1, this);
    puts("Application DB Manager Init() Success!");
    if (OpenDB(DB_HANDLE_0, "E_MASTER_DB") != 1)
        return 0;
    std::map<ENUM_DB_HANDLE_IDX, std::string> dbMap;
    dbMap.insert(std::make_pair(DB_HANDLE_1, "E_ACCOUNT_DB"));
    dbMap.insert(std::make_pair(DB_HANDLE_2, "E_GAME_DB"));
    dbMap.insert(std::make_pair(DB_HANDLE_3, "E_GAME_DB_2ND"));
    dbMap.insert(std::make_pair(DB_HANDLE_4, "E_LOG_DB"));
    dbMap.insert(std::make_pair(DB_HANDLE_6, "E_SSO_DB"));
    dbMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)8, "E_GUILD_DB"));
    dbMap.insert(std::make_pair(DB_HANDLE_5, "E_WEB_DB"));
    dbMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)0xf, "E_FRAME_LAG_INDEX_DB"));
    dbMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)0x9, "E_EVENT_DB"));
    dbMap.insert(std::make_pair((ENUM_DB_HANDLE_IDX)0xd, "E_SE_EVENT_DB"));
    for (std::map<ENUM_DB_HANDLE_IDX, std::string>::iterator it = dbMap.begin();
         it != dbMap.end(); ++it)
    {
        register STDBConnInfo* dbConnInfo =
            ((CAppConfig*)m_appConfig)->GetDBConnInfo(it->first);
        register ENUM_SERVER_GROUP serverGroup =
            (ENUM_SERVER_GROUP)((CAppConfig*)m_appConfig)->GetServerGroup();
        if (QueryConnInfo(it->first, serverGroup, *dbConnInfo) != 1)
            return 0;
        if (OpenDB(it->first, it->second) != 1)
            return 0;
    }
    puts("DBMW_ALL_DB Open Success");
    return 1;
}
bool CApplication::OpenDB(ENUM_DB_HANDLE_IDX idx, std::string name)
{
    STDBConnInfo* connInfo = ((CAppConfig*)m_appConfig)->GetDBConnInfo(idx);
    if (strncmp(connInfo->m_host, "unused", strlen(connInfo->m_host)) == 0 ||
        strncmp(connInfo->m_host, "", strlen(connInfo->m_host)) == 0)
        return 1;
    // 原版传参顺序：Open(idx, host, port, db, user, pass)——
    // 与 CDBManager::Open 的 (user, pass, db) 及 open6 的 mysql_real_connect
    // 错位形成双重补偿，最终连接参数恰好正确；逐字节复刻原版。
    if (!m_dbManager.Open(idx, connInfo->m_host, connInfo->m_port,
                          connInfo->m_db, connInfo->m_user, connInfo->m_pass))
    {
        printf("%s Open Fail!\n", name.c_str());
        return 0;
    }
    printf("%s Open Success!\n", name.c_str());
    return 1;
}
bool CApplication::QueryConnInfo(ENUM_DB_HANDLE_IDX idx, ENUM_SERVER_GROUP serverGroup,
                                 STDBConnInfo& connInfo)
{
    CDBHandle* h = m_dbManager.GetDBHandle(DB_HANDLE_0);
    if (!h->set_query(0,
                      "seLect db_name,db_ip,db_port,db_userid,db_passwd from db_connect where db_server_group = %d and db_type = %d",
                      serverGroup, idx))
        return 0;
    if (!h->exec(0))
        return 0;
    if (!h->fetch())
        return 0;
    if (h->get_n_rows() != 1)
        return 0;
    int i = 0;
    if (!h->get_str(i++, connInfo.m_db, 0x1f))
        return 0;
    if (!h->get_str(i++, connInfo.m_host, 0x11))
        return 0;
    if (!h->get_int(i++, connInfo.m_port))
        return 0;
    if (!h->get_str(i++, connInfo.m_user, 0x15))
        return 0;
    char buf[0x3c] = {0};
    if (!h->get_str(i++, buf, 0x3c))
        return 0;
    if (!((CAppConfig*)m_appConfig)->DecryptValue(buf, connInfo.m_pass))
        return 0;
    connInfo.m_tail = serverGroup;
    return 1;
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
CServerHandler* CApplication::Get_ServerHandler() { return m_serverHandler; }
void* CApplication::Get_UdpHandler() { return m_udpHandler; }
CTcpNetSystem* CApplication::Get_TcpNetSystem() { return &m_tcpNetSystem; }
CMutex* CApplication::Get_UdpQLock() { return &m_mutexF8; }
CMutex* CApplication::Get_UdpBLock() { return &m_mutex110; }
CMutex* CApplication::Get_QLock() { return &m_mutexF8; }
CMutex* CApplication::Get_BLock() { return &m_mutex110; }
UdpRecvQueue* CApplication::Get_UdpPacketRecvQ() { return m_udpSwapQueue.GetRecvQ(); }
UdpRecvQueue* CApplication::Get_UdpPacketParseQ() { return m_udpSwapQueue.GetParseQ(); }
bool CApplication::Send_Term_Signal(const std::string& msg)
{
    std::string filename = std::string("./pid/") + msg + std::string(".pid");
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
int CApplication::Send_Suspend_Signal(const std::string& msg)
{
    std::string filename = std::string("./pid/") + msg + std::string(".pid");
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
    (void)idx;
    return m_serverHandler->GetMonitorServer();
}
void CApplication::SwitchQueueTCP()
{
    CGuard<CMutex> guard(m_tcpNetSystem.Get_TcpRecvQLock());
    if (!IQueue<TcpRecvQueue>::Get().SwitchQueue())
        return;
    CPacketDecoderInstance()->SetTCPQueue(
        IQueue<TcpRecvQueue>::Get().GetParseQueue());
}
void CApplication::SwitchQueueUDP()
{
    CGuard<CMutex> guard(&m_mutexF8);
    if (m_udpSwapQueue.GetRecvQ()->empty())
        return;
    m_udpSwapQueue.SwapQ();
    m_networkThread->SetUDPQueue(m_udpSwapQueue.GetRecvQ());
    CPacketDecoderInstance()->SetUdpQueue(m_udpSwapQueue.GetParseQ());
}
void CApplication::Free()
{
    try
    {
        puts("Application Free Start!");
        if (m_networkThread)
        {
            (*(void(***)(void*))m_networkThread)[0](m_networkThread);
            if (m_networkThread != 0)
                (*(void(***)(void*))m_networkThread)[3](m_networkThread);
            m_networkThread = 0;
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
        if (m_guildManager)
        {
            delete m_guildManager;
            m_guildManager = 0;
        }
        puts("Guild Manager Free Success!");
        puts("Application End!");
    }
    DNF_CATCH_RETHROW("CApplication::Free() Exception Break");
}
// [DNF-NONIDENTICAL] DNF-DBM-DIFF-0068 | dbmw | 与ORIG差异=DIFF | CApplication::TranslateSignal | 详见 function_reports/dbmw/_ZN12CApplication15TranslateSignalEv.md
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
        case 1:
            m_serverConfig->Load_Table(std::string("./table/server_config.tbl"));
            m_serverHandler->Load((ST_ServerInfo*)((CServerConfig*)m_serverConfig)->GetServerInfo());
            break;
        case 2:
        {
            int guildWarPoint = (*it)->m_field4;
            m_guildManager->clearGuildWar();
            if (!m_dbManager.QueryGuildWarPointList(guildWarPoint, m_guildManager))
            {
                DNF_LOG_SCOPE_LINE(0x454,"./log/GuildWar",
                    "m_clDBManager::QueryGuildWarPointList Err : return false");
                return;
            }
            std::vector<std::pair<unsigned int, STGuildWarRankInfo*> >* ranks =
                m_guildManager->GetVtGuildWarRankInfo();
            if (ranks->empty())
                return;
            int item = get_awardItem_using_interval();
            if (item < 0)
            {
                DNF_LOG_SCOPE_LINE(0x45f,"./log/GuildWar",
                    "m_clDBManager.OnEndGuildWar Err : get_day_interval() Func return minus value");
                return;
            }
            int guildId = (*it)->m_field8;
            if (guildId != 0)
            {
                if (!m_dbManager.AwardGuildTitleByMail(
                        guildWarPoint, guildId, 0x65b2,
                        "\xB8\xB6\xC0\xCC\xBE\xEE\xC0\xFC\xC0\xEF\xBF\xB5\xBF\xF5", item))
                {
                    DNF_LOG_SCOPE_LINE(0x46a,"./log/GuildWar",
                        "CApplication.AwardGuildTitleByMail Err : return false");
                    return;
                }
            }
            Packet_Notice_Guild_Mail_Arrived pkt;
            pkt.m_guildId = guildId;
            pkt.m_flag = 1;
            CMonitorServer* ms = m_serverHandler->GetMonitorServer();
            ms->SendToServer((char*)&pkt, pkt.packetSize);
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
