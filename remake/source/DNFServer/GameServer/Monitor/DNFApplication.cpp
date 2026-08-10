// df_monitor_r — DNFApplication（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <algorithm>

#include "DNFApplication.h"
#include "DNFFileLog.h"
#include "DNFFunctionLib.h"
#include "DNFAppConfig.h"
#include "DNFMemberConfig.h"
#include "ServerXml.h"
#include "SystemTimeHandler.h"
#include "Packet_Monitor_Member_Secede.h"
#include "Packet_GM_Request_Mid.h"
#include "Packet_PvPChannelInfo.h"
#include "Packet_PvPChannelUserCount.h"
#include "Packet_Item_Limit_Edition_Sell_Start.h"
#include "Packet_DBMW_Change_Char_Name.h"
#include "Packet_Monitor_Reply_Charac_Info.h"
#include "Arad_MomijiEvent.h"
#include "DNFAppStartInit.h"
#include "DNFAppStopInit.h"
#include "DNFBuddy.h"
#include "DNFDBServer.h"
#include "DNFGameServer.h"
#include "DNFInnerMsgHandler.h"
#include "DNFKillUserConfig.h"
#include "DNFManagerServer.h"
#include "DNFMember.h"
#include "DNFNetworkThread.h"
#include "DNFPacketDecoder.h"
#include "DNFPacketTranslater.h"
#include "DNFServerHandler.h"
#include "DNFSignal.h"
#include "DNFSignalTranslator.h"
#include "DNFTableBase.h"
#include "DNFTowerRank.h"
#include "DNFUdpHandler.h"
#include "DNFUser.h"
#include "EventActionManager.h"
#include "GMAccounts.h"
#include "IPCounter.h"
#include "ItemLimitEditionManager.h"
#include "LimitNpcBuyItem.h"
#include "LoginLogoutStatistics.h"
#include "MemoryCashManager.h"
#include "OnTimeEventManager.h"
#include "TaskImpl.h"
#include "TowerOfDespairReloadAPC.h"

CPeriodicMessageMgr::CPeriodicMessageMgr() { init(); }

CPeriodicMessageMgr::~CPeriodicMessageMgr() {}

void CPeriodicMessageMgr::init()
{
    memset(m_msg, 0, 0x200);
    m_startHour = 0;
    m_endHour = 0;
}

void CPeriodicMessageMgr::OnProcess(CServerHandler* handler)
{
    time_t t = time(0);
    struct tm st = *localtime(&t);
    if (st.tm_min == 0 && m_startHour <= st.tm_hour && m_endHour >= st.tm_hour &&
        st.tm_hour != 0)
    {
        OnTimerSendData(handler);
    }
    if (st.tm_hour == 0 && st.tm_min == 10)
    {
        Packet_Load_Periodic_Message pkt;
        handler->SendToDB(&pkt);
    }
}

void CPeriodicMessageMgr::OnTimerSendData(CServerHandler* handler)
{
    if (m_msg[0] != 0)
    {
        Packet_Send_Periodic_Message pkt;
        size_t len = strlen(m_msg);
        strncpy((char*)&pkt + 0xa, m_msg, len);
        handler->SendAllTcpGameServer(&pkt);
    }
}

void CPeriodicMessageMgr::SetMessageData(char* msg, int startHour, int endHour)
{
    if (startHour < 0 || endHour < 0 || 0x17 < startHour || 0x17 < endHour)
    {
        DNF_LOG_SCOPE_LINE(0x18, "./log/PeriodicMessage", "SetData() Error : Invalid input time");
    }
    else if (*msg == 0)
    {
        memset(this, 0, 0x200);
        DNF_LOG_SCOPE_LINE(0x24, "./log/PeriodicMessage", "SetData() Error : No string");
    }
    else
    {
        strncpy(m_msg, msg, 0x1ff);
        m_startHour = startHour;
        m_endHour = endHour;
        DNF_LOG_SCOPE_LINE(0x32, "./log/PeriodicMessage", "TEST Periodic Message : Arrive Load Result");
    }
}

unsigned int get_rand_int(int n)
{
    if (n < 0)
    {
        return 0;
    }
    if (n == 0)
    {
        return rand();
    }
    int r = rand();
    if (n < r)
    {
        r = rand();
        return (unsigned int)r % (unsigned int)n;
    }
    int a = r * 0x41c64e6d + 0x3039;
    int b = a * 0x41c64e6d + 0x3039;
    int c = b * 0x41c64e6d + 0x3039;
    unsigned int u = ((((int)(((unsigned int)(a >> 31) >> 16) + a) >> 16) & 0x7ff) << 10 ^
                      (((int)(((unsigned int)(b >> 31) >> 16) + b) >> 16) & 0x3ff)) << 10 ^
                     ((int)(((unsigned int)(c >> 31) >> 16) + c) >> 16) & 0x3ff;
    if ((unsigned int)n < u)
    {
        u = u % (unsigned int)n;
    }
    return u;
}

CollectItms::CollectItms() {}

CollectItms::~CollectItms() {}

namespace init_accusation
{
CInitAccusationListMgr::CInitAccusationListMgr(CApplication& app) {}

CInitAccusationListMgr::~CInitAccusationListMgr() {}

void CInitAccusationListMgr::setSchedule(bool const& flag)
{
    time_t t = GetNowTime();
    struct tm* lt = localtime(&t);
    lt->tm_hour = 6;
    lt->tm_min = 0;
    lt->tm_sec = 0;
    time_t next = mktime(lt);
    if (flag)
    {
        next += 0x15180;
    }
    CInitAccusationList* list = new CInitAccusationList(next, 0, this);
    getApp()->GetTaskScheduler()->AddTask(list);
}

CApplication* CInitAccusationListMgr::getApp() const
{
    return *(CApplication**)this;
}

CInitAccusationList::CInitAccusationList(unsigned int time, unsigned int flag,
                                         CInitAccusationListMgr* mgr)
    : CTask(time, flag)
{
    m_mgr = mgr;
}

CInitAccusationList::~CInitAccusationList() {}

void CInitAccusationList::_DoExecute()
{
    m_mgr->getApp()->ClearAccusationList();
    bool flag = true;
    m_mgr->setSchedule(flag);
}
}

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
    void (**vtab)(void*) = (void(**)(void*))(*(void**)this);
    vtab[5](this);
}

CApplication::CApplication()
{
    m_loaded = false;
    m_reserved8 = 0;
    m_appInit = 0;
    m_appConfig = 0;
    m_field90 = 0;
    m_memberConfig = 0;
    m_memberExpTbl = 0;
    m_serverHandler = 0;
    m_serverHandler2 = 0;
    m_innerMsgHandler = 0;
    m_udpHandler = 0;
    m_udpThread = 0;
    m_taskScheduler = 0;
    m_field2cc = 0;
    m_memoryCash = 0;
    m_towerRank = 0;
    m_itemLimitMgr = 0;
    m_ipCounter = 0;
    m_field330 = 0;
    m_periodicMsg = 0;
    m_limitNpc = 0;
    m_field388 = 0;
    SetMiniCraneRandomSeed();
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
        g_ServerString_.StrLoading();
        CheckArgv(argc, argv);
        CSignalTranslatorInstance()->init(this);
        AttachAppInitor(argv);
        m_appInit->Init(this, argc, argv);
        m_field31c = 0;
        m_field320 = 0;
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
        m_memoryCash = new CMemoryCashManager;
        m_memoryCash->Init(this);
        m_userManager.Init(this);

        m_memberConfig->Load_Table("./script/member_cnt_config.tbl");
        m_memberExpTbl->Load_Table("./script/member_exp.tbl");
        m_memberManager.Init(this, &m_userManager, m_memberConfig, m_memberExpTbl);

        m_appConfig->Load_Table(argv[1]);
        m_frameCount.InitFrameCountInfo(this, m_appConfig->Get_FrameCountValue(), 0x3e8);

        m_udpHandler = new CUdpHandler;
        if (m_udpHandler->InitServerSocket(m_appConfig->Get_ServerUdpPort()) == -1)
        {
            throw CDNFException("CApplication::Load() Init Server Udp Socket Exception Break!");
        }

        m_serverHandler2 = new CServerHandler;
        m_serverHandler2->Attach(this);
        m_serverHandler2->Load(m_appConfig->GetServerInfoMap());
        CPacketTranslater::attach(this);
        m_innerMsgHandler = new CInnerMsgHandler;
        CPacketDecoderInstance()->Attach(this);
        m_udpThread = new CUdpNetworkThread;
        m_udpThread->attach(this);
        if (!m_udpThread->begin())
        {
            throw;
        }

        m_tcpNetSystem.Init(m_appConfig->Get_ServerTcpPort());
        {
            DNF_LOG_SCOPE_LINE(0x1ba, "./log/Tcp", "App Load : Network system (%x)", &m_tcpNetSystem);
        }

        {
            const char* mgrIp = m_appConfig->Get_ManagerTcpIP();
            unsigned short mgrPort = m_appConfig->Get_ManagerTcpPort();
            CTcpManagerServer* mgr = (CTcpManagerServer*)m_serverHandler2->GetTcpManagerServer();
            if (*mgrIp != '\0' && mgrPort != 0)
            {
                mgr->Init(&m_tcpNetSystem);
                mgr->SetIP(mgrIp);
                mgr->SetPort(mgrPort);
                if (m_tcpNetSystem.OpenTcpService(*mgr->GetSockRef(), mgrIp, mgrPort) == 1)
                {
                    printf("Application OpenTcpService(%s, %d) Fail!\n", mgrIp, mgrPort);
                    DNF_LOG_SCOPE_LINE(0x1d3,"./log/TcpServer", "Application OpenTcpService(%s, %d, %d) Fail!",
                        mgrIp, mgrPort, mgr->GetSock());
                }
                else
                {
                    DNF_LOG_SCOPE_LINE(0x1dc,"./log/TcpServer", "Application OpenTcpService(fd:%d,ip:%s,port:%d) Success!",
                        mgr->GetSock(), mgrIp, mgrPort);
                    printf("Application OpenTcpService(fd:%d,ip:%s,port:%d) Success!\n",
                        mgr->GetSock(), mgrIp, mgrPort);
                }
            }
            else
            {
                puts("Application TCP cfg empty!");
                DNF_LOG_SCOPE_LINE(0x1e4, "./log/TcpServer", "Application TCP cfg empty!");
            }
        }

        {
            const char* dbIp = m_appConfig->Get_DBMWTcpIP();
            unsigned short dbPort = m_appConfig->Get_DBMWTcpPort();
            CTcpDBServer* db = (CTcpDBServer*)m_serverHandler2->GetTcpDBServer();
            if (*dbIp != '\0' && dbPort != 0)
            {
                db->Init(&m_tcpNetSystem);
                db->SetIP(dbIp);
                db->SetPort(dbPort);
                if (m_tcpNetSystem.OpenTcpService(*db->GetSockRef(), dbIp, dbPort) == 1)
                {
                    printf("Application OpenTcpService(%s, %d) Fail!\n", dbIp, dbPort);
                    DNF_LOG_SCOPE_LINE(0x200,"./log/TcpServer", "Application OpenTcpService(%s, %d, %d) Fail!",
                        dbIp, dbPort, db->GetSock());
                }
                else
                {
                    DNF_LOG_SCOPE_LINE(0x209,"./log/TcpServer", "Application OpenTcpService(fd:%d,ip:%s,port:%d) Success!",
                        db->GetSock(), dbIp, dbPort);
                    printf("Application OpenTcpService(fd:%d,ip:%s,port:%d) Success!\n",
                        db->GetSock(), dbIp, dbPort);
                }
            }
            else
            {
                puts("Application TCP cfg empty!");
                DNF_LOG_SCOPE_LINE(0x211, "./log/TcpServer", "Application TCP cfg empty!");
            }
        }

        srand((unsigned int)(m_appConfig->Get_ServerUdpPort() + time(0)));

        m_taskScheduler = new CTaskScheduler;
        if (CTask_ChristmasEvent::getEventEndTime() >= (int)time(0))
        {
            m_taskScheduler->AddTask(new CTask_ChristmasEvent(CTask_ChristmasEvent::MakeEventStartTick(0), 0));
        }
        m_taskScheduler->AddTask(new TowerOfDespairReloadAPC_Task(0, 0));

        m_itemLimitMgr = new CItemLimitEditionMgr;
        {
            Packet_Item_Limit_Edition_Load_Data_Req pkt;
            pkt.m_fieldA = 1;
            pkt.m_fieldB = Get_ServerGroup();
            pkt.m_fieldC = 0;
            m_serverHandler2->SendToDB(&pkt);
        }

        m_ipCounter = new CIPCounter;
        m_ipCounter->Init(m_serverHandler2);

        if (m_field31c == 0)
        {
            m_field31c = new CEventActionManager;
        }
        if (m_field320 == 0)
        {
            m_field320 = new COnTimeEventManager;
        }
        ((COnTimeEventManager*)m_field320)->AttachApp(this);

        m_field2cc = new WongWork::CGMAccounts;
        m_periodicMsg = new CPeriodicMessageMgr;
        {
            Packet_Load_Periodic_Message pkt;
            m_serverHandler2->SendToDB(&pkt);
        }
        m_limitNpc = new LimitNpcBuyItemManager;
        {
            LimitNpcBuyItemRequestInfo pkt;
            m_serverHandler2->SendToDB(&pkt);
        }

        m_field388 = new CollectItms;
        {
            time_t now = time(0);
            tm pt = *localtime(&now);
            m_timeSyncHour = (short)pt.tm_hour;
        }

        m_towerRank = new CTowerRank;
        m_towerRank->processReloadRanking(m_serverHandler2, true, 10000);
        m_towerRank->processReloadRanking(m_serverHandler2, true, 5);

        m_field334 = new init_accusation::CInitAccusationListMgr(*this);
        {
            bool schedule = false;
            ((init_accusation::CInitAccusationListMgr*)m_field334)->setSchedule(schedule);
        }

        m_field330 = new CLoginLogoutStatistics(*this);
        {
            typedef std::queue<CTcpRecvBuffer*, std::deque<CTcpRecvBuffer*, std::allocator<CTcpRecvBuffer*> > > TcpRecvQueue;
            IQueue<TcpRecvQueue>::Get()->InitQueue(
                m_tcpNetSystem.Get_TcpSwapQPacket()->GetRecvQ(),
                m_tcpNetSystem.Get_TcpSwapQPacket()->GetParseQ());
        }

        puts("Application Load() Success!");
        m_loaded = true;
    }
    catch (CDNFException& e)
    {
        printf("Load Exception : %s\n", e.what());
        throw;
    }
    catch (...)
    {
        puts("Load Exception !");
        throw;
    }
}

void CApplication::Free()
{
    try
    {
        puts("Application Free Start!");
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
        if (m_serverHandler2 != 0)
        {
            delete m_serverHandler2;
            m_serverHandler2 = 0;
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
        if (m_taskScheduler != 0)
        {
            delete m_taskScheduler;
            m_taskScheduler = 0;
        }
        if (m_field2cc != 0)
        {
        delete m_field2cc;
            m_field2cc = 0;
        }
        if (m_memoryCash != 0)
        {
            delete m_memoryCash;
            m_memoryCash = 0;
        }
        if (m_towerRank != 0)
        {
            delete m_towerRank;
            m_towerRank = 0;
        }
        if (m_itemLimitMgr != 0)
        {
            delete m_itemLimitMgr;
            m_itemLimitMgr = 0;
        }
        if (m_ipCounter != 0)
        {
            delete m_ipCounter;
            m_ipCounter = 0;
        }
        if (m_field334 != 0)
        {
            ::operator delete(m_field334);
            m_field334 = 0;
        }
        if (m_periodicMsg != 0)
        {
            delete m_periodicMsg;
            m_periodicMsg = 0;
        }
        if (m_limitNpc != 0)
        {
            delete m_limitNpc;
            m_limitNpc = 0;
        }
        if (m_field388 != 0)
        {
            ::operator delete(m_field388);
            m_field388 = 0;
        }
        if (m_field330 != 0)
        {
        delete m_field330;
            m_field330 = 0;
        }
        puts("Application Exit!");
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
            CFrameCountHandler* frameInfo = m_frameCount.GetFrameCountInfo();
            if (frameInfo->m_field24 != 0 && 1 < frameInfo->m_field24)
            {
                m_serverHandler2->Process();
                if (2 < frameInfo->m_field24)
                {
                    m_memberManager.MemberRegisterFlagProcess();
                    m_userManager.MemberEnterProcess();
                    m_frameCount.SaveProcess();
                    m_userManager.ProcessByMinute();
                    m_memoryCash->ProcessLifeTimeOut();
                    m_memoryCash->ProcessCashDataPrint();
                    m_towerRank->processReloadRanking(m_serverHandler2, false, 10000);
                    m_towerRank->processReloadRanking(m_serverHandler2, false, 5);
                    m_periodicMsg->OnProcess(m_serverHandler2);
                    ((CLoginLogoutStatistics*)m_field330)->ProcessByMinute();
                    ProcessTimeSync();
                    UpdateCollectItems();
                    if (frameInfo->m_field24 == 4)
                    {
                        UpdateMiniCraneSeed();
                    }
                }
            }
            getItemLimitEditionMgr()->processScheduledJob(this, false);
            SwitchQueueTCP();
            SwitchQueueUDP();
            CPacketDecoderInstance()->Process();
            m_ipCounter->Proc((unsigned int)time(0));
            m_taskScheduler->ProcessTask((unsigned int)time(0));
            DNFFLib::Sleep_Ext(0, 1);
        }
        catch (CDNFException& e)
        {
            printf("CApplication::Process() Exception Break : %s\n", e.what());
            DNF_LOG_SCOPE_LINE(0x413, "./log/process", "CApplication::Process() Exception Break : %s", e.what());
            throw;
        }
        catch (...)
        {
            puts("CApplication::Process() Exception Break");
            DNF_LOG_SCOPE_LINE(0x418, "./log/process", "CApplication::Process() Exception Break\n");
            throw;
        }
    }
    puts("CApplication::Process() Exit");
    DNF_LOG_SCOPE_LINE(0x41c, "./log/process", "CApplication::Process() Exit\n");
}

void CApplication::SwitchQueueTCP()
{
    CGuard<CMutex> guard(m_tcpNetSystem.Get_TcpRecvQLock());
    if (!IQueue<TcpRecvQueue>::Get()->SwitchQueue())
    {
        CPacketDecoderInstance()->SetTCPQueue(IQueue<TcpRecvQueue>::Get()->GetParseQueue());
    }
}

void CApplication::SwitchQueueUDP()
{
    CGuard<CMutex> guard(&m_udpQLock);
    if (!m_udpSwapQueue.GetRecvQ()->empty())
    {
        m_udpSwapQueue.SwapQ();
        m_udpThread->SetUDPQueue(m_udpSwapQueue.GetRecvQ());
        CPacketDecoderInstance()->SetUdpQueue(m_udpSwapQueue.GetParseQ());
    }
}

void CApplication::SendTestPacket_2()
{
}

UdpRecvQueue* CApplication::Get_UdpPacketParseQ()
{
    return m_udpSwapQueue.GetParseQ();
}

CItemLimitEditionMgr* CApplication::getItemLimitEditionMgr()
{
    return m_itemLimitMgr;
}

LimitNpcBuyItemManager* CApplication::getLimitNpcBuyItemManager()
{
    return m_limitNpc;
}

CTcpNetSystem* CApplication::Get_TcpNetSystem()
{
    return &m_tcpNetSystem;
}

void CApplication::OnGameServerDown(CGameServer* server)
{
    m_userManager.DeleteUsersOnGameServerDown(server);
}

CServerHandler* CApplication::Get_ServerHandler()
{
    return m_serverHandler2;
}

CUserManager* CApplication::Get_UserManager()
{
    return &m_userManager;
}

CMemberManager* CApplication::Get_MemberManager()
{
    return &m_memberManager;
}

CBuddyRegisterManager* CApplication::Get_BuddyRegisterManager()
{
    return &m_buddyMgr;
}

CTaskScheduler* CApplication::GetTaskScheduler()
{
    return m_taskScheduler;
}

COnTimeEventManager* CApplication::GetOnTimeEventManager()
{
    return *(COnTimeEventManager**)((char*)this + 0x320);
}

void* CApplication::GetGMAccounts()
{
    return m_field2cc;
}

void* CApplication::getIPCounter()
{
    return m_ipCounter;
}

void* CApplication::getTowerRank()
{
    return m_towerRank;
}

int CApplication::getMiniCraneSeed() const
{
    return m_miniCraneSeed;
}

void* CApplication::getCollectItems()
{
    return m_field388;
}

void* CApplication::FindGameServer(int id)
{
    return m_serverHandler2->GetGameServer((unsigned int)id);
}

void* CApplication::FindTcpGameServer(unsigned int id)
{
    return m_serverHandler2->GetTcpGameServer(id);
}

void CApplication::OnTcpGameServerDown(CTcpGameServer* tcpGameServer)
{
    m_userManager.DeleteUsersOnTcpGameServerDown(tcpGameServer);
}

void CApplication::SendTestPacket_1()
{
    Packet_Monitor_Event_End endPkt;
    *(unsigned int*)((char*)&endPkt + 0xa) = 9;
    CPacketTranslater::OnEventEnd(&endPkt);
    Packet_Monitor_Event_Start startPkt;
    *(unsigned int*)((char*)&startPkt + 0xa) = 9;
    *(unsigned short*)((char*)&startPkt + 0xe) = 4;
    *(unsigned short*)((char*)&startPkt + 0x12) = 0;
    CPacketTranslater::OnEventStart(&startPkt);
}

void* CApplication::Get_MemoryCashManager()
{
    return m_memoryCash;
}

void* CApplication::GetLoginLogoutStatistics()
{
    return m_field330;
}

void* CApplication::GetPeriodicMessageManager()
{
    return m_periodicMsg;
}

void CApplication::Remove_GM_id(unsigned int id)
{
    std::map<unsigned int, std::list<unsigned int> >::iterator it = m_map368.find(id);
    if (it != m_map368.end())
    {
        it->second.clear();
        m_map368.erase(it);
    }
}

char CApplication::isGM_regFromChannel(unsigned int channel)
{
    std::map<unsigned int, std::list<unsigned int> >::iterator it = m_map368.find(channel);
    return it != m_map368.end();
}

char CApplication::isAbleUserChatWithGM(unsigned int channel, unsigned int charNo)
{
    std::map<unsigned int, std::list<unsigned int> >::iterator it = m_map368.find(channel);
    if (it != m_map368.end())
    {
        for (std::list<unsigned int>::iterator li = it->second.begin(); li != it->second.end();
             ++li)
        {
            if (*li == charNo)
            {
                return 1;
            }
        }
    }
    return 0;
}

void CApplication::AddChattableUserWithGM(unsigned int channel, unsigned int charNo)
{
    std::map<unsigned int, std::list<unsigned int> >::iterator it = m_map368.find(channel);
    if (it != m_map368.end())
    {
        for (std::list<unsigned int>::iterator li = it->second.begin(); li != it->second.end();
             ++li)
        {
            if (*li == charNo)
            {
                return;
            }
        }
        it->second.push_back(charNo);
    }
}

void CApplication::DisableChatUserWithGM(unsigned int channel, unsigned int charNo)
{
    std::map<unsigned int, std::list<unsigned int> >::iterator it = m_map368.find(channel);
    if (it != m_map368.end())
    {
        for (std::list<unsigned int>::iterator li = it->second.begin(); li != it->second.end();
             ++li)
        {
            if (*li == charNo)
            {
                it->second.erase(li);
                return;
            }
        }
    }
}

char CApplication::Send_Term_Signal(const std::string& file)
{
    std::string path = "./pid/" + file;
    FILE* f = fopen(path.c_str(), "r");
    if (f == 0)
    {
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", path.c_str());
    }
    else
    {
        int pid = 0;
        fscanf(f, "%d", &pid);
        if (pid < 1)
        {
            fclose(f);
            printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        }
        else
        {
            int r = kill(pid, 0xf);
            if (r < 0)
            {
                fclose(f);
                printf("%d\xb9\xf8 process\xb7\xce \xc1\xbe\xb7\xe1 signal \xbc\xdb\xbd\xc5 \xbd\xc7\xc6\xd0", pid);
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

void CApplication::Send_Suspend_Signal(const std::string& file)
{
    std::string path = "./pid/" + file;
    FILE* f = fopen(path.c_str(), "r");
    if (f == 0)
    {
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", path.c_str());
    }
    else
    {
        int pid = 0;
        fscanf(f, "%d", &pid);
        if (pid < 1)
        {
            fclose(f);
            printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        }
        else
        {
            int r = kill(pid, 10);
            if (r < 0)
            {
                fclose(f);
                printf("%d\xb9\xf8 process\xb7\xce \xc1\xbe\xb7\xe1 signal \xbc\xdb\xbd\xc5 \xbd\xc7\xc6\xd0", pid);
            }
            else
            {
                printf("SEND SUSPEND SIGNAL TO %d\n", pid);
                fclose(f);
            }
        }
    }
}

void CApplication::App_Stop()
{
    m_loaded = false;
}

void CApplication::TranslateSignal()
{
    m_serverHandler->Clear_Table();
    m_serverHandler->Load_Table("./script/kill_user_config.tbl");
    const std::vector<ST_KillUSRConfig*>* vec = m_serverHandler->GetInfo();
    if (!vec->empty())
    {
        for (std::vector<ST_KillUSRConfig*>::const_iterator it = vec->begin(); it != vec->end();
             ++it)
        {
            ST_KillUSRConfig* cfg = *it;
            if (cfg->m_type == 3)
            {
                Packet_Monitor_Event_End pkt;
                pkt.m_fieldA = cfg->m_val;
                CPacketTranslater::OnEventEnd(&pkt);
            }
            else if (cfg->m_type == 2)
            {
                Packet_Monitor_Event_Start pkt;
                pkt.m_fieldA = cfg->m_val;
                pkt.m_fieldB = (unsigned short)cfg->m_b;
                pkt.m_fieldC = (unsigned short)cfg->m_c;
                CPacketTranslater::OnEventStart(&pkt);
            }
            else if (cfg->m_type == 4)
            {
                m_serverHandler2->Load(m_appConfig->GetServerInfoMap());
                m_memberConfig->Load_Table("./script/member_cnt_config.tbl");
                m_memberExpTbl->Load_Table("./script/member_exp.tbl");
            }
            else if (cfg->m_type == 7)
            {
                Packet_Monitor_Take_Screen_Shot pkt;
                pkt.m_fieldA = 0xff;
                pkt.m_fieldB = (unsigned int)time(0);
                CPacketTranslater::OnTakeScreenShot(&pkt);
            }
        }
    }
}

int CApplication::AddAccusationCharac(const std::string& a, const std::string& b, int c,
                                      char type)
{
    std::map<std::string, int>::iterator it = m_map350.find(a);
    if (it != m_map350.end())
    {
        if (type == 'p')
        {
            if (9 < it->second)
            {
                return 99;
            }
        }
        else
        {
            if (1 < it->second)
            {
                return 99;
            }
        }
    }
    else
    {
        m_map350.insert(std::pair<const std::string, int>(a, 0));
        it = m_map350.find(a);
    }
    if (m_map350.size() == (m_map350.size() / 100) * 100)
    {
        DNF_LOG_SCOPE_AT("AddAccusationCharac", 0x5c0, "./log/mannerlessUser", "user count : %u\n", m_map350.size());
    }
    std::pair<const std::string, int> key(b, c);
    if (m_set338.find(key) != m_set338.end())
    {
        return 0x6f;
    }
    m_set338.insert(key);
    it->second++;
    return 0x6e;
}

void CApplication::ClearAccusationList()
{
    m_set338.clear();
    m_map350.clear();
}

void CApplication::Add_GM_id(unsigned int id)
{
    std::map<unsigned int, std::list<unsigned int> >::iterator it = m_map368.find(id);
    if (it == m_map368.end())
    {
        std::list<unsigned int> l;
        m_map368.insert(std::pair<const unsigned int, std::list<unsigned int> >(id, l));
    }
}

void CApplication::Call_DeleteMember(unsigned int key, CUser* user)
{
    m_memberManager.MemberMemLogout(key, user, true);
}

void CApplication::Call_ResetBlackList(unsigned int charNo)
{
    m_userManager.ResetBlackList(charNo);
}

void CApplication::Call_ResetBuddyList(unsigned int charNo)
{
    m_userManager.ResetBuddyList(charNo);
}

void CApplication::Call_ResetUserMemberInfo(unsigned int key)
{
    m_userManager.ResetMemberInfo(key);
}

void* CApplication::Get_UdpPacketRecvQ()
{
    return m_udpSwapQueue.GetRecvQ();
}

CUdpHandler* CApplication::Get_UdpHandler()
{
    return m_udpHandler;
}

void* CApplication::Get_UdpQLock()
{
    return 0;
}

void* CApplication::Get_UdpBLock()
{
    return 0;
}

void CApplication::CheckArgv(int argc, char** argv)
{
    if (argc < 3)
    {
        throw CDNFException("CApplication::CheckArgv() Argv Count Exception Break!");
    }
}

void CApplication::AttachAppInitor(char** argv)
{
    const char* mode = argv[2];
    if (strcmp(mode, "start") != 0 && strcmp(mode, "nofork") != 0)
    {
        if (strcmp(mode, "stop") == 0)
        {
            m_appInit = new CAppStopInit;
            return;
        }
        throw CDNFException("CApplication::AttachAppInitor() unknown argv[2]");
    }
    m_appInit = new CAppStartInit;
}

unsigned char CApplication::Get_ServerGroup()
{
    return m_appConfig->Get_ServerGroup();
}

void CApplication::SetMiniCraneRandomSeed()
{
    m_miniCraneSeed = rand() % 0x32;
}

void CApplication::SendMiniCraneRandomSeed()
{
    Packet_MiniCraneSeed packet;
    packet.m_fieldA = getMiniCraneSeed();
    m_serverHandler2->SendAllTcpGameServer(&packet);
}

void CApplication::UpdateMiniCraneSeed()
{
    time_t now = time(0);
    tm* pt = localtime(&now);
    if (pt->tm_hour == 6)
    {
        SetMiniCraneRandomSeed();
        SendMiniCraneRandomSeed();
    }
}

void CApplication::ProcessTimeSync()
{
    time_t now = time(0);
    tm t = *localtime(&now);
    int hour = t.tm_hour;
    int min = t.tm_min;
    if (hour != m_timeSyncHour && hour >= 0 && hour < 0x18 && min >= 0 && min < 0x3c)
    {
        Packet_Send_Time_Sync pkt;
        pkt.m_fieldA = (unsigned short)hour;
        pkt.m_fieldC = (unsigned short)min;
        m_serverHandler2->SendAllTcpGameServer(&pkt);
        m_timeSyncHour = (short)hour;
    }
}

void CApplication::UpdateCollectItems()
{
    time_t now = time(0);
    tm t = *localtime(&now);
    if ((t.tm_min & 1U) == 0)
    {
        CollectItms* items = (CollectItms*)m_field388;
        Packet_CollectItemsUpdate pkt;
        pkt.m_fieldA = *(unsigned int*)((char*)items + 4);
        pkt.m_fieldF = *(unsigned int*)((char*)items + 8);
        pkt.m_fieldE = Get_ServerGroup();
        pkt.m_field13 = *(unsigned char*)((char*)items + 0xc);
        m_serverHandler2->SendToDB(&pkt);
        Packet_CollectItemsResult pkt2;
        pkt2.m_fieldE = *(unsigned int*)((char*)items + 4);
        pkt2.m_fieldA = *(unsigned int*)((char*)items + 0);
        pkt2.m_field12 = *(unsigned int*)((char*)items + 8);
        m_serverHandler2->SendAllTcpGameServer(&pkt2);
    }
}


// ---- ShowLogo / CApplicationInstance ----
void ShowLogo()
{
    putchar('\n');
    puts("**********************************************************");
    puts("* DUNGEON & FIGHTER Monitor Server                        *");
    puts("**********************************************************");
    puts("**********************************************************");
    puts("* DUNGEON & FIGHTER Monitor Server                        *");
    puts("**********************************************************");
    puts("**********************************************************");
    CommonTime t;
    t.SetCurTime();
    printf("%c%c%c%c%c", t.m_field0, t.m_field1, t.m_field2, t.m_field3, t.m_field4);
}

CAppBase* CApplicationInstance()
{
    static CApplication instance;
    return &instance;
}
