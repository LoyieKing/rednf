// df_monitor_r — DNFApplication（从 MonitorTypes/App/Table 拆分）
#include <stdio.h>
#include "RawAccess.h"
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
    int h;
    if (st.tm_min == 0)
    {
        h = st.tm_hour;
        if (m_startHour <= h && m_endHour >= h && h != 0)
        {
            OnTimerSendData(handler);
        }
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
        int len = strlen(m_msg);
        strncpy(pkt.m_msg, m_msg, len);
        handler->SendAllTcpGameServer(&pkt);
    }
    return;
}

void CPeriodicMessageMgr::SetMessageData(char* msg, int startHour, int endHour)
{
    if (startHour < 0 || endHour < 0 || 0x17 < startHour || 0x17 < endHour)
    {
        DNF_LOG_SCOPE_LINE(0x18, "./log/PeriodicMessage", "SetData() Error : Invalid input time");
    }
    else if (*msg == 0)
    {
        memset(m_msg, 0, 0x200);
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

// 自由函数 get_rand_int —— 原实现在此处定义，已迁出（ORIG 判词 2026-09-14）：
//   df_monitor_r 里该符号全 binary 只有一处定义，且不在本 TU 的 .o 内 ——
//   0x0809bf18 紧邻 comp_by_time(0x0809bf04)，其后是 CTask_ChristmasEvent 一族
//   （0x0809c00e 起），该 .o 的 GI 键为 _GLOBAL__I__Z12get_rand_inti(0x0809c4a3)
//   ⇒ 定义在 TaskImpl.o 且为该 TU 首强实体；而本 TU 的 .o 区在 0x0805exxx-
//   0x0806cxxx（CApplicationInstance 0x080627c4、GI 键 0x08066025），相隔 ~0x3a000。
//   ⇒ DNFApplication.o 既不定义也不调用它（唯一调用点在 0x0809c058，属 TaskImpl.o）。
//   故定义已落回 TaskImpl.cpp 顶部；返回类型 int 的依据见 DNFApplication.h:255 起。
//   此处不得再定义：会与 ServerCommon/DNFFunctionLib.cpp 的同名定义构成
//   _Z12get_rand_inti 重复符号（后者对 monitor 已由 DF_NO_FREE_GET_RAND_INT 关闭）。
CollectItms::CollectItms()
{
    m_field0 = 0;
    m_field4 = 0;
    m_field8 = 0;
    m_fieldC = 1;
}

CollectItms::~CollectItms() {}

namespace init_accusation
{
CInitAccusationListMgr::CInitAccusationListMgr(CApplication& app)
{
    m_app = &app;  // ORIG：mov 0xc(%ebp),%edx; mov %edx,(%eax)
}

CInitAccusationListMgr::~CInitAccusationListMgr() {}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-DIFF-0040 已消除）
bool CInitAccusationListMgr::setSchedule(bool const& flag)
{
    time_t t = GetNowTime();
    struct tm* lt = localtime(&t);
    lt->tm_hour = 6;
    lt->tm_min = 0;
    lt->tm_sec = 0;
    time_t next = mktime(lt);
    if (flag)
    {
        next = next + 86400u;  // ORIG: mov/add/mov(无符号常量阻止 add-mem 折叠)
    }
    m_app->GetTaskScheduler()->AddTask(new CInitAccusationList(next, 0, this));
    return true;
}

CApplication* CInitAccusationListMgr::getApp() const
{
    return m_app;
}

CInitAccusationList::CInitAccusationList(unsigned int time, unsigned int flag,
                                         CInitAccusationListMgr* mgr) throw()
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

// ORIG 中 CAppInit ctor 与 CAppStartInit/StopInit 不同 TU（派生 ctor 帧 0x18）
CAppInit::CAppInit() {}

// ORIG 中 CBaseEventAction ctor 与 CNullEventAction 不同 TU（派生 ctor 帧 0x18）
CBaseEventAction::CBaseEventAction() {}

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
    : m_loaded(false),
      m_reserved8(0),
      m_appInit(0),
      m_appConfig(0),
      m_field90(0),
      m_memberConfig(0),
      m_memberExpTbl(0),
      m_serverHandler(0),
      m_serverHandler2(0),
      m_innerMsgHandler(0),
      m_udpHandler(0),
      m_udpThread(0),
      m_taskScheduler(0),
      m_gmAccounts(0),
      m_memoryCash(0),
      m_towerRank(0),
      m_ipCounter(0),
      m_loginLogoutStats(0),
      m_periodicMsg(0),
      m_limitNpc(0),
      m_collectItms(0)
{
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
        m_eventActionMgr = 0;
        m_onTimeEventMgr = 0;
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

// [DNF-NONIDENTICAL] DNF-MON-NEAR-0006 | monitor | 与ORIG差异=NEAR | CApplication::Load | 详见 function_reports/monitor/_ZN12CApplication4LoadEiPPc.md
//   2026-09-15 AppLane 已消形态：三个 Packet_* 声明下移到各自用点 block（ORIG ctor 在 786/900/926 行）；
//   CInnerMsgHandler 与 TowerOfDespairReloadAPC_Task 的 ctor 加 throw()（消除我方多余的 3 个
//   _ZdlPv 清理块，16→13 与 ORIG 一致）；`(int)CTask_ChristmasEvent::getEventEndTime()`（setae→setge）；
//   TowerOfDespairReloadAPC_Task* 具名局部 reloadTask；DNFApplication.h 的 m_udpThread 改基类指针
//   CThreadInterface*（ORIG new 赋值处有 `mov %ebx,%eax; mov %eax,%edx` 指针调整）；
//   另去掉 Mgr/Db 两个 TCP 配置块的 `{}`（S1：块层级进槽分配，去掉后 -0x20..-0x38 区对齐，77→53→27）。
//   2026-09-15 MonitorA（机理 A「占位↔具名临时代偿对」命中 ⇒ ndiff 27→6）：
//   删去 try 首未引用的 `int reservedSlot;` 占位，**同时**把 `bool schedule` 内联为字面量临时
//   （`setSchedule(false)`，并删去其包裹块）—— 二者是代偿对，缺一不可。
//   实测：只删占位=66；只内联（保留占位）=89；保留占位且把 bool 提到 try 首=76、提到 mgr 块前=55（均劣化）。
//   本改动使 bool、now/pt、pkt1 三区**全部对齐**；余差 ndiff=6：仅 pkt2(-0xae→-0xb2) 与 pkt3(-0xb8→-0xbc)
//   两槽深 4（orig/ours 均 1110 条、帧同为 0x18c）。
//   归一化验证（槽偏移+跳转/调用目标+数据地址全掩码）：residual=0 ⇒ 纯槽编号差异。
//   三连分诊（2026-09-15）：ORIG/我方槽集 46/46 相同、长度 1110/1110 相同、帧同为 0x18c
//   ⇒ **排除「重建遗漏」族**，余差是单纯的整体槽偏移。
//   已试无效（勿重复）：now/pt 四种声明位置（block 版 139）；占位换位置（外层/try 首/pt 之后/末位，非 try 首均 77）；
//   第二个占位（try 首 142、mgr 块前 142、db 块内 118）；`char reservedSlot[4]`（S1 前 139 / S1 后 89）；去 pkt1 块花括号（27 不变）。
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
        m_frameCount.InitFrameCountInfo(this, ((CAppConfig*)m_appConfig)->Get_FrameCountValue(), 0x3e8);

        m_udpHandler = new CUdpHandler;
        if (m_udpHandler->InitServerSocket(((CAppConfig*)m_appConfig)->Get_ServerUdpPort()) == -1)
        {
            throw CDNFException("CApplication::Load() Init Server Udp Socket Exception Break!");
        }

        m_serverHandler2 = new CServerHandler;
        m_serverHandler2->Attach(this);
        m_serverHandler2->Load(((CAppConfig*)m_appConfig)->GetServerInfoMap());
        CPacketTranslater::attach(this);
        m_innerMsgHandler = new CInnerMsgHandler;
        CPacketDecoderInstance()->Attach(this);
        m_udpThread = new CUdpNetworkThread;
        ((CUdpNetworkThread*)m_udpThread)->attach(this);
        if (!m_udpThread->begin())
        {
            throw;
        }

        m_tcpNetSystem.Init(((CAppConfig*)m_appConfig)->Get_ServerTcpPort());
        {
            DNF_LOG_SCOPE_LINE(0x1ba, "./log/Tcp", "App Load : Network system (%x)", &m_tcpNetSystem);
        }

        const char* mgrIp = ((CAppConfig*)m_appConfig)->Get_ManagerTcpIP();
        unsigned short mgrPort = ((CAppConfig*)m_appConfig)->Get_ManagerTcpPort();
        CTcpManagerServer* mgr = m_serverHandler2->GetTcpManagerServer();
        if (*mgrIp != '\0' && mgrPort != 0)
        {
            mgr->Init(&m_tcpNetSystem);
            mgr->SetIP(mgrIp);
            mgr->SetPort(mgrPort);
            // [2026-09-14 修复] 极性反了：ORIG CTcpNetSystem::OpenTcpService
            //   0x08053afd `mov eax,1` = **成功返 1**、0x080539e9 `mov eax,0` = 失败返 0；
            //   而 ORIG monitor 连接成功时打印的是 **Success!**（A/B 两轮稳定实测）。
            //   旧转写判 `== 1 → Fail` ⇒ 成功报 Fail、失败报 Success，正好相反。
            if (m_tcpNetSystem.OpenTcpService(*mgr->GetSockRef(), mgrIp, mgrPort) == 0)
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
        const char* dbIp = ((CAppConfig*)m_appConfig)->Get_DBMWTcpIP();
        unsigned short dbPort = ((CAppConfig*)m_appConfig)->Get_DBMWTcpPort();
        CTcpDBServer* db = m_serverHandler2->GetTcpDBServer();
        if (*dbIp != '\0' && dbPort != 0)
        {
            db->Init(&m_tcpNetSystem);
            db->SetIP(dbIp);
            db->SetPort(dbPort);
            // 同上：OpenTcpService 成功返 1 ⇒ 失败分支判 == 0
            if (m_tcpNetSystem.OpenTcpService(*db->GetSockRef(), dbIp, dbPort) == 0)
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
        srand((unsigned int)(((CAppConfig*)m_appConfig)->Get_ServerUdpPort() + time(0)));

        m_taskScheduler = new CTaskScheduler;
        if ((int)CTask_ChristmasEvent::getEventEndTime() >= (int)time(0))
        {
            m_taskScheduler->AddTask(new CTask_ChristmasEvent(CTask_ChristmasEvent::MakeEventStartTick(0), 0));
        }
        TowerOfDespairReloadAPC_Task* reloadTask = new TowerOfDespairReloadAPC_Task(0, 0);
        m_taskScheduler->AddTask(reloadTask);

        m_itemLimitMgr = new CItemLimitEditionMgr;
        {
            Packet_Item_Limit_Edition_Load_Data_Req pktLoadItemLimit;
            pktLoadItemLimit.m_fullLoad = 1;
            pktLoadItemLimit.m_serverType = Get_ServerGroup();
            pktLoadItemLimit.m_loadTargetNum = 0;
            m_serverHandler2->SendToDB(&pktLoadItemLimit);
        }
        m_ipCounter = new CIPCounter;
        m_ipCounter->Init(m_serverHandler2);

        if (m_eventActionMgr == 0)
        {
            m_eventActionMgr = new CEventActionManager;
        }
        if (m_onTimeEventMgr == 0)
        {
            m_onTimeEventMgr = new COnTimeEventManager;
        }
        m_onTimeEventMgr->AttachApp(this);

        m_gmAccounts = new WongWork::CGMAccounts;
        m_periodicMsg = new CPeriodicMessageMgr;
        {
            Packet_Load_Periodic_Message pktLoadPeriodic;
            m_serverHandler2->SendToDB(&pktLoadPeriodic);
        }
        m_limitNpc = new LimitNpcBuyItemManager;
        {
            LimitNpcBuyItemRequestInfo pktLoadLimitNpc;
            m_serverHandler2->SendToDB(&pktLoadLimitNpc);
        }

        m_collectItms = new CollectItms;
        time_t now = time(0);
        tm pt = *localtime(&now);
        m_timeSyncHour = (short)pt.tm_hour;

        m_towerRank = new CTowerRank;
        m_towerRank->processReloadRanking(m_serverHandler2, true, 10000);
        m_towerRank->processReloadRanking(m_serverHandler2, true, 5);

        m_accusationMgr = new init_accusation::CInitAccusationListMgr(*this);
        ((init_accusation::CInitAccusationListMgr*)m_accusationMgr)->setSchedule(false);

        m_loginLogoutStats = new CLoginLogoutStatistics(*this);
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
        if (m_gmAccounts != 0)
        {
        delete m_gmAccounts;
            m_gmAccounts = 0;
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
        if (m_accusationMgr != 0)
        {
            ::operator delete(m_accusationMgr);
            m_accusationMgr = 0;
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
        if (m_collectItms != 0)
        {
            ::operator delete(m_collectItms);
            m_collectItms = 0;
        }
        if (m_loginLogoutStats != 0)
        {
        delete m_loginLogoutStats;
            m_loginLogoutStats = 0;
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
            if (frameInfo->m_state != 0 && 1 < frameInfo->m_state)
            {
                m_serverHandler2->Process();
                if (2 < frameInfo->m_state)
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
                    ((CLoginLogoutStatistics*)m_loginLogoutStats)->ProcessByMinute();
                    ProcessTimeSync();
                    UpdateCollectItems();
                    if (frameInfo->m_state == 4)
                    {
                        UpdateMiniCraneSeed();
                    }
                }
                // ORIG：processScheduledJob 位于外层 if 块末尾（内层 if 之后）
                getItemLimitEditionMgr()->processScheduledJob(this, false);
            }
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
            DNF_LOG_SCOPE_LINE(0x413, "./log/process", "CApplication::Process() Exception Break : %s\n", e.what());
        }
        catch (...)
        {
            puts("CApplication::Process() Exception Break");
            DNF_LOG_SCOPE_LINE(0x418, "./log/process", "CApplication::Process() Exception Break\n");
        }
    }
    puts("CApplication::Process() Exit");
    DNF_LOG_SCOPE_LINE(0x41c, "./log/process", "CApplication::Process() Exit\n");
}

void CApplication::SwitchQueueTCP()
{
    CGuard<CMutex> guard(m_tcpNetSystem.Get_TcpRecvQLock());
    // ORIG：xor $0x1,%eax + jne = `if (!SwitchQueue()) {} else {swap}`
    // （空 if + else 形态才会物化 xor $1）
    if (!IQueue<TcpRecvQueue>::Get()->SwitchQueue())
    {
    }
    else
    {
        CPacketDecoderInstance()->SetTCPQueue(IQueue<TcpRecvQueue>::Get()->GetParseQueue());
    }
}

void CApplication::SwitchQueueUDP()
{
    CGuard<CMutex> guard(&m_udpQLock);
    if (m_udpSwapQueue.GetRecvQ()->empty())
    {
    }
    else
    {
        m_udpSwapQueue.SwapQ();
        ((CUdpNetworkThread*)m_udpThread)->SetUDPQueue(m_udpSwapQueue.GetRecvQ());
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
    return m_onTimeEventMgr;
}

void* CApplication::GetGMAccounts()
{
    return m_gmAccounts;
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

CollectItms* CApplication::getCollectItems()
{
    return m_collectItms;
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
    endPkt.m_eventCode = 9;
    CPacketTranslater::OnEventEnd(&endPkt);
    Packet_Monitor_Event_Start startPkt;
    startPkt.m_eventCode = 9;
    startPkt.m_eventParam1 = 4;
    startPkt.m_eventParam2 = 0;
    CPacketTranslater::OnEventStart(&startPkt);
}

void* CApplication::Get_MemoryCashManager()
{
    return m_memoryCash;
}

void* CApplication::GetLoginLogoutStatistics()
{
    return m_loginLogoutStats;
}

void* CApplication::GetPeriodicMessageManager()
{
    return m_periodicMsg;
}

void CApplication::Remove_GM_id(unsigned int id)
{
    std::map<unsigned int, std::list<unsigned int> >::iterator it = m_map368.find(id);
    if (m_map368.end() != it)
    {
        it->second.clear();
        m_map368.erase(it);
    }
}

bool CApplication::isGM_regFromChannel(unsigned int channel)
{
    return m_map368.find(channel) != m_map368.end();
}

bool CApplication::isAbleUserChatWithGM(unsigned int channel, unsigned int charNo)
{
    std::map<unsigned int, std::list<unsigned int> >::iterator it = m_map368.find(channel);
    if (m_map368.end() != it)
    {
        std::list<unsigned int>::iterator found =
            std::find(it->second.begin(), it->second.end(), charNo);
        if (it->second.end() != found)
        {
            return 1;
        }
    }
    return 0;
}

void CApplication::AddChattableUserWithGM(unsigned int channel, unsigned int charNo)
{
    std::map<unsigned int, std::list<unsigned int> >::iterator it = m_map368.find(channel);
    if (m_map368.end() != it)
    {
        std::list<unsigned int>::iterator found =
            std::find(it->second.begin(), it->second.end(), charNo);
        if (it->second.end() == found)
        {
            it->second.push_back(charNo);
        }
    }
}

void CApplication::DisableChatUserWithGM(unsigned int channel, unsigned int charNo)
{
    std::map<unsigned int, std::list<unsigned int> >::iterator it = m_map368.find(channel);
    if (m_map368.end() != it)
    {
        std::list<unsigned int>::iterator found =
            std::find(it->second.begin(), it->second.end(), charNo);
        if (it->second.end() != found)
        {
            it->second.remove(charNo);
        }
    }
}

bool CApplication::Send_Term_Signal(const std::string& file)
{
    int pid;
    int result;
    std::string path = "./pid/" + file + ".pid";
    FILE* f = fopen(path.c_str(), "r");
    if (f == 0)
    {
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", path.c_str());
        return 0;
    }
    else
    {
        fscanf(f, "%d", &pid);
        if (pid < 1)
        {
            fclose(f);
            printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
            return 0;
        }
        else
        {
            result = kill(pid, 0xf);
            if (result < 0)
            {
                fclose(f);
                printf("%d\xb9\xf8 process\xb7\xce \xc1\xbe\xb7\xe1 signal \xbc\xdb\xbd\xc5 \xbd\xc7\xc6\xd0", pid);
                return 0;
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
    int pid;
    int result;
    std::string path = "./pid/" + file + ".pid";
    FILE* f = fopen(path.c_str(), "r");
    if (f == 0)
    {
        printf("%s process id file open \xbd\xc7\xc6\xd0\n", path.c_str());
    }
    else
    {
        fscanf(f, "%d", &pid);
        if (pid < 1)
        {
            fclose(f);
            printf("%d\xb9\xf8\xc0\xc7 \xc0\xdf\xb8\xf8\xb5\xc8 process id\n", pid);
        }
        else
        {
            result = kill(pid, 10);
            if (result < 0)
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

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原 DNF-MON-DIFF-0005 已消除）
void CApplication::TranslateSignal()
{
    m_serverHandler->Clear_Table();
    m_serverHandler->Load_Table("./script/kill_user_config.tbl");
    const std::vector<ST_KillUSRConfig*>* vec = m_serverHandler->GetInfo();
    if (vec->empty())
    {
        return;
    }
    for (std::vector<ST_KillUSRConfig*>::const_iterator it = vec->begin(); it != vec->end();
         ++it)
    {
        switch ((*it)->m_type)
        {
        case 2:
        {
            Packet_Monitor_Event_Start pkt;
            pkt.m_eventCode = (*it)->m_val;
            pkt.m_eventParam1 = (unsigned short)(*it)->m_b;
            pkt.m_eventParam2 = (unsigned short)(*it)->m_c;
            CPacketTranslater::OnEventStart(&pkt);
            break;
        }
        case 3:
        {
            Packet_Monitor_Event_End pkt;
            pkt.m_eventCode = (*it)->m_val;
            CPacketTranslater::OnEventEnd(&pkt);
            break;
        }
        case 4:
        {
            m_serverHandler2->Load(((CAppConfig*)m_appConfig)->GetServerInfoMap());
            m_memberConfig->Load_Table("./script/member_cnt_config.tbl");
            m_memberExpTbl->Load_Table("./script/member_exp.tbl");
            break;
        }
        case 7:
        {
            Packet_Monitor_Take_Screen_Shot pkt;
            pkt.m_channel = 0xff;
            pkt.m_time = (unsigned int)time(0);
            CPacketTranslater::OnTakeScreenShot(&pkt);
            break;
        }
        }
    }
}

// [DNF-IDENTICAL] 2026-09-15 verified ndiff=0（原残差件 AddAccusationCharac 已消除）
char CApplication::AddAccusationCharac(const std::string& a, const std::string& b, int c,
                                      char type)
{
    std::map<std::string, int>::iterator it = m_map350.find(a);
    if (m_map350.end() == it)
    {
#if decompile_verify
        m_map350.insert(std::make_pair<const std::string, int>(a, 0));
#else
        // 宿主 make_pair 是转发引用形态，显式 <const string,int> 会要求右值 ⇒ 直接构造 pair。
        m_map350.insert(std::pair<const std::string, int>(a, 0));
#endif
        it = m_map350.find(a);
    }
    else
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
    if (m_map350.size() % 100 == 0)
    {
        DNF_LOG_SCOPE_AT("AddAccusationCharac", 0x5c0, "./log/mannerlessUser", "user count : %u\n", m_map350.size());
    }
    if (m_set338.find(std::pair<const std::string, int>(b, c)) == m_set338.end())
    {
        m_set338.insert(std::pair<const std::string, int>(b, c));
        it->second++;
        return 0x6e;
    }
    return 0x6f;
}

void CApplication::ClearAccusationList()
{
    m_set338.clear();
    m_map350.clear();
}

void CApplication::Add_GM_id(unsigned int id)
{
    // ORIG：单表达式 find(id)==end()（右先求值：end() 在前），make_pair 走 rvalue 形态
    if (m_map368.find(id) == m_map368.end())
    {
        std::list<unsigned int> l;
        // ORIG：make_pair 显式模板实参 <uint,list>（直接取 id/l 地址，无 move 调用）
#if decompile_verify
        m_map368.insert(std::make_pair<unsigned int, std::list<unsigned int> >(id, l));
#else
        // 宿主 make_pair 显式模板实参 + 左值实参会绑定失败 ⇒ 直接构造 pair。
        m_map368.insert(std::pair<unsigned int, std::list<unsigned int> >(id, l));
#endif
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
    return &m_udpQLock;
}

void* CApplication::Get_UdpBLock()
{
    return &m_udpBLock;
}

void CApplication::CheckArgv(int argc, char** argv)
{
    if (argc < 3)
    {
        throw CDNFException("CApplication::CheckArgv() \xbd\xc7\xc7\xe0 \xbe\xc6\xb1\xd4\xb8\xd5\xc6\xae \xbf\xc0\xb7\xf9\n");
    }
}

void CApplication::AttachAppInitor(char** argv)
{
    const char* mode = argv[2];
    if (strcmp(mode, "start") == 0 || strcmp(mode, "nofork") == 0)
    {
        m_appInit = new CAppStartInit;
        return;
    }
    if (strcmp(mode, "stop") == 0)
    {
        m_appInit = new CAppStopInit;
        return;
    }
    throw CDNFException("CApplication::AttachAppInitor() \xbd\xc7\xc7\xe0 \xbe\xc6\xb1\xd4\xb8\xd5\xc6\xae \xbf\xc0\xb7\xf9\n");
}

unsigned char CApplication::Get_ServerGroup()
{
    return ((CAppConfig*)m_appConfig)->Get_ServerGroup();
}

void CApplication::SetMiniCraneRandomSeed()
{
    m_miniCraneSeed = rand() % 0x32;
}

void CApplication::SendMiniCraneRandomSeed()
{
    Packet_MiniCraneSeed packet;
    packet.m_seed = getMiniCraneSeed();
    m_serverHandler2->SendAllTcpGameServer(&packet);
}

void CApplication::UpdateMiniCraneSeed()
{
    time_t now = time(0);
    tm t = *localtime(&now);
    if (t.tm_hour == 6)
    {
        SetMiniCraneRandomSeed();
        SendMiniCraneRandomSeed();
    }
}

void CApplication::ProcessTimeSync()
{
    time_t now = time(0);
    tm t = *localtime(&now);
    // ORIG：无独立 hour/min 局部变量（栈槽复用 struct tm 字段），
    // 且各范围检查为 goto 形态（ORIG 尾部为 4×`nop; jmp` 汇合 stub）
    if (t.tm_hour != m_timeSyncHour)
    {
        if (t.tm_hour < 0) goto end;
        if (t.tm_hour > 0x17) goto end;
        if (t.tm_min < 0) goto end;
        if (t.tm_min > 0x3b) goto end;
        Packet_Send_Time_Sync pkt;
        pkt.m_hour = (unsigned short)t.tm_hour;
        pkt.m_min = (unsigned short)t.tm_min;
        m_serverHandler2->SendAllTcpGameServer(&pkt);
        m_timeSyncHour = (short)t.tm_hour;
    }
end:
    ;
}

void CApplication::UpdateCollectItems()
{
    time_t now = time(0);
    tm t = *localtime(&now);
    if ((t.tm_min & 1U) == 0)
    {
        // ORIG：不设局部指针，每次直接读成员 m_collectItms
        Packet_CollectItemsUpdate pkt;
        pkt.m_fieldA = ((CollectItms*)m_collectItms)->m_field4;
        pkt.m_fieldF = ((CollectItms*)m_collectItms)->m_field8;
        pkt.m_fieldE = Get_ServerGroup();
        pkt.m_field13 = ((CollectItms*)m_collectItms)->m_fieldC;
        m_serverHandler2->SendToDB(&pkt);
        Packet_CollectItemsResult pkt2;
        pkt2.m_fieldE = ((CollectItms*)m_collectItms)->m_field4;
        pkt2.m_fieldA = ((CollectItms*)m_collectItms)->m_field0;
        pkt2.m_field12 = ((CollectItms*)m_collectItms)->m_field8;
        m_serverHandler2->SendAllTcpGameServer(&pkt2);
    }
}


// ---- ShowLogo / CApplicationInstance ----
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
           (int)(char)t.m_year, (int)(char)t.m_month,
           (int)(char)t.m_day, (int)(char)t.m_hour,
           (int)(char)t.m_minute);
}

CAppBase* CApplicationInstance()
{
    static CApplication instance;
    return &instance;
}
