// df_game_r 入口域 —— main / Init / MainLoop / ShowLogo /
// Neof_startupGlobalInstances / Neof_shutdownGlobalInstances 及
// g_npcScript* 全局变量定义。
//
// ORIG 权威符号（nm -S df_game_r）：
//   main                            T 0x0829f839  0x12d
//   ShowLogo()                      T 0x0829f966  0x0c5
//   Neof_startupGlobalInstances()   T 0x0829f7c5  0x3a
//   Neof_shutdownGlobalInstances()  T 0x0829f7ff  0x3a
//   Init(int, char**)               T 0x082a08f4  0x181e
//   MainLoop()                      T 0x0829fa2b  0x305
//   g_npcScriptBaseDirectory        B 0x094fe680  0x100
//   g_npcScriptFileList             B 0x094fe780  0x18
//
// Init/MainLoop 体量大（0x181e/0x305），此处为忠实骨架：按 objdump
// 提取的 call 序列（ORIG 地址逐条核对）还原主体流程与顺序；未还原的
// 细节分支以「推断」标注。

#include "CEnvironment.h"
#include "StreamDeps.h"
#include "CLog.h"
#include "CShutdowManager.h"
#include "GameTypes.h"
#include "GlobalData.h"
#include "ThreadInterface.h"
#include "CMonitorServerProxy.h"
#include "CGuildServerProxy.h"
#include "CStatisticServerProxy.h"
#include "CServerProxyMgr.h"
#include "NpcScript.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "DBMgr.h"
#include "CDataManager.h"
#include "ServerParameterScript.h"  // stLightServerNotice_t
#include "CAuctionAveragePrice.h"
#include "CGameManager.h"
#include "GameWorld.h"
#include "HistoryLog.h"
#include "CDeathTowerRanking.h"
#include "CDailyScheduleManager.h"
#include "Arad_DataManager.h"  // ARAD::ScriptManager / Singleton（权威声明）
#include "CEventTable.h"  // CEventTable 权威声明（唯一；定义在 CEventManager.cpp）
#include <cstdio>
#include <cerrno>
#include <sys/resource.h>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include "EpollHandler.h"     // GlobalInstance<EpollHandler> 完整类型
#include "CInventory.h"       // Store（GlobalInstance<Store>）
#include <pthread.h>
#include <csignal>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <map>
#include <algorithm>

// 定时器注册实现依赖（ORIG G_TimerQueue 0x80f647c / CSystemTime::getCurSec
// 0x80cbc9e / get_rand_int 0x86b1b87 均为 ORIG 真实符号）
#include "TimerQueue.h"

// 三大分发器权威类型（本 TU 仅用其生命周期 CREATE_/DESTROY_ 与定时器静态成员）
#include "InterDispatcher.h"
#include "PacketDispatcher.h"
#include "TimerDispatcher.h"
int get_rand_int(int range);  // 定义于 CItemList/工具 TU（GameRand.h 同签名）
void setCodePageByLocal();
using GlobalData::s_systemTime_;  // ORIG 0x0941f714（CSystemTime 全局实例）

// 全局单例创建/销毁（ORIG T 0x82a7029..0x82a7136；定义属各自翻译域 TU）
void CREATE_CEnvironment();        void DESTROY_CEnvironment();
void CREATE_CGameManager();       void DESTROY_CGameManager();
void CREATE_CDataManager();       void DESTROY_CDataManager();
void CREATE_InterDispatcher();    void DESTROY_InterDispatcher();
void CREATE_EpollHandler();       void DESTROY_EpollHandler();
void CREATE_PacketDispatcher();   void DESTROY_PacketDispatcher();
void CREATE_Store();              void DESTROY_Store();
void CREATE_TimerQueue();         void DESTROY_TimerQueue();
void CREATE_TimerDispatcher();    void DESTROY_TimerDispatcher();
// ---------------------------------------------------------------------------
// 外部自由函数 / 类（均为 ORIG 真实符号；声明集中于此，定义在各自 TU）
// ---------------------------------------------------------------------------
void regist_signals();                       // _Z14regist_signalsv          0x0829f4c5
int check_argv(int argc, char** argv);       // _Z10check_argviPPc           0x0829ea5a
bool IsLightServer();                        // _Z13IsLightServerv           0x0822ad44 W
int create_thread();                         // _Z13create_threadv           0x0829fd30（返回 bool）
void InitCipherKeyString();                  // _Z19InitCipherKeyStringv     0x082a025a
void RegistServerSchedule();                 // _Z20RegistServerSchedulev    0x082a0459
void sleep_ext(int sec, int usec);           // _Z9sleep_extii
void ProcessAntiBotInputWrapper();           // _Z26ProcessAntiBotInputWrapperv
void ProcessServerStatusLog();               // _Z22ProcessServerStatusLogv

// GetConfigFileString(map<string,string>&, const char*)  _Z19GetConfigFileStringRSt3mapISsSsSt4lessISsESaISt4pairIKSsSsEEEPKc
std::string GetConfigFileString(
    std::map<std::string, std::string>& conf, const char* key);

class CodePage
{
public:
    static void initCodePage();              // _ZN8CodePage12initCodePageEv
};

namespace Taiwan
{
class AuthGarenaToken
{
public:
    static void init(char const* path);      // _ZN6Taiwan15AuthGarenaToken4initEPKc
};
}  // namespace Taiwan


// np_server_xml::CServerXml::StrLoading()（_ZN13np_server_xml10CServerXml10StrLoadingEv）
namespace np_server_xml
{

class CServerXml
{
public:
    void StrLoading();
};
}  // namespace np_server_xml


// CInGameAdvertisementManager（ORIG 无既有头；最小声明，字段未建模 —「推断」）
class CInGameAdvertisementManager
{
public:
    void LoadADExposeInfoAtApp();            // _ZN27CInGameAdvertisementManager21LoadADExposeInfoAtAppEv
};
CInGameAdvertisementManager* InGameADManagerInstance();  // _Z23InGameADManagerInstancev 0x080fc9e9

// CmmChannelProxy（ORIG 真实类；头未建，最小声明 —「推断」）
class CmmChannelProxy
{
public:
    void SendChannelInfo();                  // _ZN15CmmChannelProxy15SendChannelInfoEv
};

// InGameADManagerInstance —— ORIG 0x80fc9e9（__cxa_guard 静态单例，0x93fead0）。
CInGameAdvertisementManager* InGameADManagerInstance()
{
    static CInGameAdvertisementManager s_instance;
    return &s_instance;
}

// CInGameAdvertisementManager::LoadADExposeInfoAtApp —— ORIG 0x80fd83c：
// memset 0x67b60 条件缓冲 → queryADExposeInfo(0x80fd19e) → 对 200 项
// （每项 0x84c 字节）首字段 != -1 者调 InsertADExposeInfo(0x80fd8e4)。
// query/Insert 及条件表结构未建模 —「推断」骨架：TODO(G2)。
void CInGameAdvertisementManager::LoadADExposeInfoAtApp()
{
}

// Taiwan::AuthGarenaToken::init(char const*) —— ORIG 0x81730d6（0x11b 字节）：
// param 与空串比较，空则 COpensslRSA::Init()（默认公钥），否则 COpensslRSA::Init(param)。
// COpensslRSA（全局 0x9404754）未建模 —「推断」骨架：TODO(G2)。
void Taiwan::AuthGarenaToken::init(char const*)
{
}

// 定时器包装类：类型声明已迁至权威头 TimerDispatcher.h（Timer_* 分发器类，
// 静态 RegistNextTimer/registNextTimer 成员与 ORIG nm 一致）；本 TU 仅保留
// 其静态成员定义（下方，逐指令转写）。

// ---------------------------------------------------------------------------
// 定时器注册实现（ORIG 汇编逐条转写；均为 G_TimerQueue()->InsertTimer 包装，
// OBJ_TYPE=2 表示全局对象定时器，param1/key 均为 0）
// ---------------------------------------------------------------------------
void Timer_SecretShopStatistic::RegistNextTimer()
{
    // ORIG 0x86397ee：msg=0x90，间隔 60s
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x90, 0x3c, 0, 0);
}

void Timer_DailySchedule::RegistNextTimer()
{
    // ORIG 0x812c940：msg=0x91，间隔 60s
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x91, 0x3c, 0, 0);
}

void advancealtar::Timer_MainTick::RegistNextTimerGage()
{
    // ORIG 0x812f7b8：毫秒级 InsertTimerInMilisecond，msg=0xa6，1000ms
    G_TimerQueue()->InsertTimerInMilisecond((TimerEntry::OBJ_TYPE)8, 0,
                                            (TIMER_MESSAGE)0xa6, 0x3e8, 0, 0);
}

void Timer_OneADayItemShop::RegistNextTimer()
{
    // ORIG 0x863a93c：msg=0x99，间隔 60s
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x99, 0x3c, 0, 0);
}

void Timer_EventManagement::RegistNextTimer(int intervalMs)
{
    // ORIG 0x863abd4：msg=0x9c，间隔=参数
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x9c, intervalMs, 0, 0);
}

void Timer_DungeonStatisticEntrance_Hour::RegistNextTimer()
{
    // ORIG 0x863b9fa：msg=0xab，间隔 3600s
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0xab, 0xe10, 0, 0);
}

void TimerDeathTowerTopRankerReload::registNextTimer(long ms)
{
    // ORIG 0x8637854：
    //   ms==0 → 首次注册：now + rand(0..299)（秒）；
    //   否则 localtime 后强制 tm_hour=4、tm_min=30+rand(0..299)，mktime 归一
    //   （每日 04:30~08:29 随机重载点）；最终以 max(delay,1) 秒注册 msg=0x73。
    if (ms == 0)
        ms = static_cast<long>(s_systemTime_.getCurSec()) + get_rand_int(0x12c);
    else
    {
        std::tm t;
        localtime_r(&ms, &t);
        t.tm_hour = 4;
        t.tm_min = get_rand_int(0x12c) + 0x1e;
        ms = mktime(&t);
    }
    int delay = static_cast<int>(ms) - s_systemTime_.getCurSec();
    delay = std::max(delay, 1);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)6, 0,
                                (TIMER_MESSAGE)0x73, delay, 0, 0);
}

// ---------------------------------------------------------------------------
// TimerStatisticLevel / TimerScheduleTenSecond / Timer_DungeonInoutOpenTime
// ---------------------------------------------------------------------------
// ORIG m_nTimeTable 0x93ace24 D：{1,10,14,17,21}（统计小时点表）。
int TimerStatisticLevel::m_nTimeTable[5] = { 1, 10, 0xe, 0x11, 0x15 };

// ORIG 0x863591a（0x112 字节）逐分支转写：
// i 从 3 递减找首个 m_nTimeTable[i] <= 当前小时的 i，目标小时 = 表[i+1]；
// 全表未命中（found=false）或 mktime 结果早于当前时刻 → 小时=表[0]、
// tm_mday+1（明天首档）再 mktime；最终返回目标时刻 + get_rand_int(300) 秒。
int TimerStatisticLevel::CalcNextTime()
{
    std::time_t now = static_cast<std::time_t>(s_systemTime_.getCurSec());
    std::tm cur;
    localtime_r(&now, &cur);
    std::tm target = cur;
    target.tm_hour = cur.tm_hour;
    target.tm_min = 0;
    target.tm_sec = 0;
    bool found = false;
    for (int i = 3; i >= 0; --i)
    {
        if (m_nTimeTable[i] <= cur.tm_hour)
        {
            target.tm_hour = m_nTimeTable[i + 1];
            found = true;
            break;
        }
    }
    std::time_t next = mktime(&target);
    if (!found || next < now)
    {
        target.tm_hour = m_nTimeTable[0];
        target.tm_mday += 1;
        next = mktime(&target);
    }
    return static_cast<int>(next) + get_rand_int(300);
}

// ORIG 0x8637a62：msg=0x57，间隔=参数秒。
void TimerScheduleTenSecond::registNextTimer(unsigned int sec)
{
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                (TIMER_MESSAGE)0x57, (int)sec, 0, 0);
}

// ORIG 0x8639b20（0x2ea 字节）逐分支转写：遍历
// m_serverParameter.m_dungeonInoutc8（CDataManager+0x130），对每个副本进出
// 开放窗口注册下一次 msg=0x93 定时器（OBJ_TYPE=2，param2=0，param3=副本 idx）：
//   当前处于窗口内（startH<=H<=endH && startM<=M<=endM）→ 定到今天 00:00:00；
//   未进窗（开始前，或同小时内未到/已过起止分）→ 定到今天 start
//   （start 分已过则下一小时的 start 分）；
//   其余（已开始但窗口判定未命中的边缘）→ mktime(start)+0x15180（明天 start）。
//   delay = 目标 - now，负值钳 0。ORIG 末尾恒返回 eax=1（本模型 void，mangling 同）。
void Timer_DungeonInoutOpenTime::RegistNextTimer()
{
    std::time_t now = static_cast<std::time_t>(s_systemTime_.getCurSec());
    std::tm t;
    localtime_r(&now, &t);
    const int curHour = t.tm_hour;
    const int curMin = t.tm_min;
    // 非 const 迭代器：与 ORIG 一致地实例化 _Rb_tree_iterator 弱符号组。
    std::map<int, STDungeonInoutTime>& table =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8;
    for (std::map<int, STDungeonInoutTime>::iterator it = table.begin();
         it != table.end(); ++it)
    {
        const STDungeonInoutTime& win = it->second;
        std::time_t next;
        if (curHour >= win.m_startHour && curHour <= win.m_endHour &&
            curMin >= win.m_startMin && curMin <= win.m_endMin)
        {
            // 窗口内：定到今天零点。
            t.tm_hour = 0;
            t.tm_min = 0;
            t.tm_sec = 0;
            next = mktime(&t);
        }
        else
        {
            const bool beforeStart =
                win.m_startHour >= curHour ||
                (win.m_endHour >= curHour &&
                 (win.m_startMin > curMin ||
                  (win.m_startMin <= curMin && win.m_endMin < curMin)));
            if (beforeStart)
            {
                if (curHour < win.m_startHour)
                    t.tm_hour = win.m_startHour;
                else if (curMin < win.m_startMin)
                    t.tm_hour = curHour;
                else
                    t.tm_hour = curHour + 1;
                t.tm_min = win.m_startMin;
                t.tm_sec = 0;
                next = mktime(&t);
            }
            else
            {
                t.tm_hour = win.m_startHour;
                t.tm_min = win.m_startMin;
                t.tm_sec = 0;
                next = mktime(&t) + 0x15180;  // +86400：明天同一开放点
            }
        }
        int delay = static_cast<int>(next) - static_cast<int>(now);
        if (delay < 0)
        {
            delay = 0;
        }
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                    (TIMER_MESSAGE)0x93, delay, 0,
                                    (unsigned int)it->first);
    }
}

// ---------------------------------------------------------------------------
// RegistServerSchedule() —— ORIG 0x82a0459（0x49b 字节）逐条转写。
// 全部为 G_TimerQueue()->InsertTimer(OBJ_TYPE=2, 0, msg, delaySec, 0, 0)。
// ---------------------------------------------------------------------------
void RegistServerSchedule()
{
    // 0x50 / 0x52：固定 0 秒
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x50, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x52, 0, 0, 0);

    // 0x54：对齐到整半点（ORIG：1740 - 60*(tm_mon - 30*(tm_mon/15)) + (60 - tm_sec)）
    {
        std::time_t now = std::time(0);
        std::tm t;
        localtime_r(&now, &t);
        int q = t.tm_mon / 15;
        int v = t.tm_mon - 30 * q;
        int delay = 1740 - 60 * v + (60 - t.tm_sec);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                    (TIMER_MESSAGE)0x54, delay, 0, 0);
    }

    // 0x55：TimerStatisticLevel 下一统计点距当前秒数
    {
        int delay = TimerStatisticLevel::CalcNextTime() - static_cast<int>(std::time(0));
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                    (TIMER_MESSAGE)0x55, delay, 0, 0);
    }

    // 0x59 / 0x5a / 0x5b / 0x61 / 0x66 / 0x89：固定 0 秒
    static const int kMsgs0[] = { 0x59, 0x5a, 0x5b, 0x61, 0x66, 0x89 };
    for (int i = 0; i < 6; ++i)
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                    (TIMER_MESSAGE)kMsgs0[i], 0, 0, 0);

    if (IsLightServer())
    {
        // 0x68：每条 light server notice 一条 0 秒定时器，param2=序号。
        // ORIG 读 CDataManager+0xa4 vector<stLightServerNotice_t> —「推断」布局。
        std::vector<stLightServerNotice_t>* notices =
            reinterpret_cast<std::vector<stLightServerNotice_t>*>(
                reinterpret_cast<char*>(G_CDataManager()) + 0xa4);
        for (unsigned int i = 0; i < notices->size(); ++i)
            G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                        (TIMER_MESSAGE)0x68, 0, (int)i, 0);
    }

    TimerScheduleTenSecond::registNextTimer(10);

    // 0x8d：固定 0 秒
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x8d, 0, 0, 0);

    if (!G_GameWorld()->IsPVPChannel())
        Timer_DungeonInoutOpenTime::RegistNextTimer();

    // 0x8e：delay = (channel_no % 10)^2 * ... ORIG：((c%10)*16 - (c%10))*... 见下
    {
        int c = G_CEnvironment()->get_channel_no();
        int m = c % 10;
        // ORIG 转写：eax=(m<<2)=4m；delay=(4m<<4)-4m=60m。
        int delay = 60 * m;
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                    (TIMER_MESSAGE)0x8e, delay, 0, 0);
    }

    // 0x9a：固定 0 秒
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x9a, 0, 0, 0);

    // TODO(G2)：尾部调用 ARAD::Arad_ServerStateManager::{checkContinus,
    // loadServerState}（全局槽 0x941f7f0）；该类未建模，暂缺。
}


// CommonTime::SetCurTime 声明见 CUserCharacInfo.h（CommonTime 权威定义处）。

// ---------------------------------------------------------------------------
// g_npcScript 全局变量（BSS 零初始化，与 ORIG nm 类型 B 一致）
// ---------------------------------------------------------------------------
char g_npcScriptBaseDirectory[0x100];
STScriptFileList g_npcScriptFileList;

// ---------------------------------------------------------------------------
// ShowLogo() —— ORIG 0x829f966：cout<<endl 后 puts 6 行 ASCII logo，
// 再 SetCurTime 取年月日时分 printf "[!] Service Date (%02d-%02d-%02d/%02d:%02d)\n"。
// ---------------------------------------------------------------------------
void ShowLogo()
{
    std::cout << std::endl;
    std::puts("**********************************************************");
    std::puts("* +---   +-+ +  +----         DUNGEON & FIGHTER          *");
    std::puts("* |   |  | | |  |                                        *");
    std::puts("* |   |  | | |  +----                                    *");
    std::puts("* |   |  | | |  |      Open Beta Test on 2005.08.10~     *");
    std::puts("* +---   + +-+  |      Copyright(c) 2004,2005 Neople Co. *");
    std::puts("**********************************************************");

    CommonTime time;                          // 栈上对象（ebp-0x18，推断布局同 ORIG）
    time.SetCurTime();
    // ORIG 依次读 ebp-0x18/-0x17/-0x16/-0x15/-0x14 五字节 → 年/月/日/时/分（推断顺序）
    std::printf("[!] Service Date (%02d-%02d-%02d/%02d:%02d)\n",
                0, 0, 0, 0, 0);               // 「推断」：CommonTime 字段未建模，置 0 占位
}

// ---------------------------------------------------------------------------
// Neof_startupGlobalInstances() —— ORIG 0x829f7c5：
// 依序 CREATE_* 单例；Neof_shutdownGlobalInstances() 反序 DESTROY_*。
// ---------------------------------------------------------------------------
void Neof_startupGlobalInstances()
{
    CREATE_CEnvironment();
    CREATE_CGameManager();
    CREATE_CDataManager();
    CREATE_InterDispatcher();
    CREATE_EpollHandler();
    CREATE_PacketDispatcher();
    CREATE_Store();
    CREATE_TimerQueue();
    CREATE_TimerDispatcher();
    CREATE_GameWorld();
}

void Neof_shutdownGlobalInstances()
{
    DESTROY_GameWorld();
    DESTROY_TimerDispatcher();
    DESTROY_TimerQueue();
    DESTROY_Store();
    DESTROY_PacketDispatcher();
    DESTROY_EpollHandler();
    DESTROY_InterDispatcher();
    DESTROY_CDataManager();
    DESTROY_CGameManager();
    DESTROY_CEnvironment();
}

// ---------------------------------------------------------------------------
// Init(int, char**) —— ORIG 0x82a08f4（0x181e）。忠实骨架：按反汇编
// call 序列组织主体流程；配置解析细节（GetConfigFileString 各键名、
// proxy 数量参数）为「推断」占位。
// ---------------------------------------------------------------------------
bool Init(int argc, char** argv)
{
    regist_signals();

    // rlimit 提升文件句柄上限（getrlimit/setrlimit @82a09??，「推断」细节）
    struct rlimit rl;
    if (getrlimit(RLIMIT_NOFILE, &rl) == 0)
    {
        rl.rlim_cur = rl.rlim_max;
        setrlimit(RLIMIT_NOFILE, &rl);
    }

    if (!check_argv(argc, argv))
        return false;

    std::srand(static_cast<unsigned>(time(NULL)));

    // 日志初始化："./pid/%s.log" 由 get_file_name 拼接（「推断」路径格式）
    CLog::createInstance();
    char logPath[0x104];
    std::snprintf(logPath, sizeof(logPath), "%s", G_CEnvironment()->get_file_name());
    CLog::instance()->initiate(logPath, logPath, 0);

    Taiwan::AuthGarenaToken::init(G_CEnvironment()->get_channel_name());
    CodePage::initCodePage();

    if (!GlobalData::Init())
        return false;

    HistoryLog::Init();

    G_CDataManager()->init();
    G_CGameManager()->init();

    ARAD::Singleton<ARAD::ScriptManager>::Get()->LoadScript();

    G_CGameManager()->GetQuickPartySystemManager();

    G_GameWorld()->InitChannelType();

    GlobalData::initTournamentDB(G_CEnvironment()->get_file_name());

    // DB 查询计数器挂接（DBMgr@全局 s_db_mgr）
    GlobalData::s_db_mgr->SetQueryCounterPointer(
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_0, E_SERVER_GROUP_NONE /*推断*/)
            ->GetQueryCounter());

    // ---- 三类 ServerProxy 创建（ORIG 按 m_serverGroupMap 遍历配置；
    //      该成员为 private 且 getter 未建模，此处以固定组占位 —「推断」）----
    GlobalData::s_monitor_proxy_mgr->createServerProxy(1, 0, E_SERVER_GROUP_NONE,
        G_CEnvironment()->get_udp_ip_monitor(), G_CEnvironment()->get_tcp_port_monitor(),
        G_CEnvironment()->get_udp_port_monitor());
    GlobalData::s_guild_proxy_mgr->createServerProxy(1, 0, E_SERVER_GROUP_NONE,
        G_CEnvironment()->get_udp_ip_guild(), G_CEnvironment()->get_tcp_port_guild(),
        G_CEnvironment()->get_udp_port_guild());
    GlobalData::s_statistic_proxy_mgr->createServerProxy(1, 0, E_SERVER_GROUP_NONE,
        G_CEnvironment()->get_udp_ip_statistic(), 0,
        G_CEnvironment()->get_udp_port_statistic());

    G_GameWorld()->init();

    GlobalData::s_DailyScheduleManager->CheckAndSendToAllUserSchedule();

    // LoginState 初始化：遍历 statistic proxy（Start..End 索引区间）
    for (int idx = GlobalData::s_statistic_proxy_mgr->GetStartIndex(); ; )
    {
        G_CDataManager()->InitLoginState(
            G_CEnvironment()->get_gc_no_hardcode(E_SERVER_GROUP_NONE),
            G_CEnvironment()->get_channel_no());
        if (idx >= GlobalData::s_statistic_proxy_mgr->GetEndIndex())  // GetNextIndex(&idx) 推断形态
            break;
        GlobalData::s_statistic_proxy_mgr->GetNextIndex(idx);
    }

    if (IsLightServer())
    {
        G_CGameManager()->WarRoomCountAdjustByChannelInfo();
    }
    else
    {
        G_CGameManager()->WarRoomAlloc();
    }
    G_CGameManager()->allocBlueMarble();

    // ---- 事件表装载 ----
    {
        std::vector<int> eventIds;
        if (CEventTable::VerifyEventTable(eventIds)
            && CEventTable::InsertEventTable(eventIds))
        {
            for (int i = 0; i < CEventTable::GetEventTableCount(); ++i)
            {
                CEventBase* base = CEventTable::GetEventBase(i);
                int eventId = CEventTable::GetEventId(i);
                GlobalData::s_event_manager->AddEvent(eventId, base);
            }
            GlobalData::s_event_manager->LoadEventFromDB();

            // 触发周期事件（SystemTime 当前秒 → GetRepeatEvent → TriggerEventStart）
            for (int t = 0; t <= 23; ++t)  // 「推断」小时槽位循环边界
            {
                CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(t);
                if (ev)
                    GlobalData::s_event_manager->TriggerEventStart(t, Word_Param());
            }
        }
    }
    // 公会信息装载
    for (int gi = GlobalData::s_guild_proxy_mgr->GetStartIndex(); ; )
    {
        CGuildServerProxy* gp = GlobalData::s_guild_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE /*推断*/);
        if (gp)
            gp->LoadGuildInfoProxyFromDB(E_SERVER_GROUP_NONE);
        if (gi >= GlobalData::s_guild_proxy_mgr->GetEndIndex())
            break;
        GlobalData::s_guild_proxy_mgr->GetNextIndex(gi);
    }

    // 广告 / 共享服务器消息 / 拍卖均价
    InGameADManagerInstance()->LoadADExposeInfoAtApp();
    G_CGameManager()->GetSharedServerMessageManager()->LoadServerMessageFromDB();
    G_CGameManager()->GetAuctionAveragePriceManager()->LoadAveragePrice();

    np_server_xml::CServerXml xml;  // 「推断」实例化方式（ORIG 直接调用成员）
    xml.StrLoading();

    // 死亡之塔排行榜三表懒构造 + 定时刷新
    WongWork::CDeathTowerRanking::getRankTable(0);
    WongWork::CDeathTowerRanking::getRankTable(1);
    WongWork::CDeathTowerRanking::getRankTable(2);
    WongWork::CDeathTowerRanking::init(0);
    TimerDeathTowerTopRankerReload::registNextTimer(60000);  // 「推断」ms

    InitCipherKeyString();
    RegistServerSchedule();

    // Power War 启动信息请求 + 各定时器注册（ORIG 顺序保留）
    for (int pi = GlobalData::s_guild_proxy_mgr->GetStartIndex(); ; )
    {
        CGuildServerProxy* gp = GlobalData::s_guild_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE /*推断*/);
        if (gp)
            gp->SendReqestPowerWarStartInfo();
        if (pi >= GlobalData::s_guild_proxy_mgr->GetEndIndex())
            break;
        GlobalData::s_guild_proxy_mgr->GetNextIndex(pi);
    }
    Timer_SecretShopStatistic::RegistNextTimer();
    Timer_DailySchedule::RegistNextTimer();
    advancealtar::Timer_MainTick::RegistNextTimerGage();
    Timer_OneADayItemShop::RegistNextTimer();
    Timer_EventManagement::RegistNextTimer(60000);           // 「推断」间隔
    Timer_DungeonStatisticEntrance_Hour::RegistNextTimer();

    create_thread();

    GlobalData::s_db_mgr->Close();

    cMyTrace trace(G_CEnvironment()->get_channel_name(), 0, 0);  // 「推断」参数
    trace("%s %d %s init complete",
          G_CEnvironment()->get_channel_name(), G_CEnvironment()->get_channel_no(),
          G_CEnvironment()->get_file_name());

    return true;
}

// ---------------------------------------------------------------------------
// MainLoop() —— ORIG 0x829fa2b（0x305）：主循环每秒一轮——
// UpdateChannelUser → 心跳（monitor/guild proxy 遍历）→ CmmChannelProxy 上报 →
// 向 NETWORK_Q 投递心跳流包 → sleep_ext(1,0) → ProcessAntiBotInputWrapper。
// ---------------------------------------------------------------------------
void MainLoop()
{
    for (;;)
    {
        G_GameWorld()->UpdateChannelUser();

        if (G_GameWorld()->get_UserCount_InWorld() != 0
            || G_CGameManager()->userPoolRemain() != StaticPool<CUser, 600>::Size())
        {
            ProcessServerStatusLog();
        }

        // monitor 心跳
        for (int mi = GlobalData::s_monitor_proxy_mgr->GetStartIndex(); ; )
        {
            CMonitorServerProxy* mp =
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE /*推断*/);
            if (mp)
                mp->SendHeartBeat(G_CEnvironment()->get_channel_no());
            if (mi >= GlobalData::s_monitor_proxy_mgr->GetEndIndex())
                break;
            GlobalData::s_monitor_proxy_mgr->GetNextIndex(mi);
        }
        // guild 心跳
        for (int gi = GlobalData::s_guild_proxy_mgr->GetStartIndex(); ; )
        {
            CGuildServerProxy* gp =
                GlobalData::s_guild_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE /*推断*/);
            if (gp)
                gp->SendHeartBeat(G_CEnvironment()->get_channel_no());
            if (gi >= GlobalData::s_guild_proxy_mgr->GetEndIndex())
                break;
            GlobalData::s_guild_proxy_mgr->GetNextIndex(gi);
        }

        if (IsLightServer())  // 「推断」SendChannelInfo 的条件谓词（ORIG jne 分支）
            GlobalData::s_channel_proxy->SendChannelInfo();

        // 构造心跳 Stream 包投递到 NETWORK_Q（两个 int 字段，「推断」载荷语义）
        {
            Stream* s = GlobalData::s_small_stream_pool->Acquire(__FILE__, __LINE__);
            CStreamGuard guard(s, true);
            guard << G_CEnvironment()->get_channel_no();
            guard << G_GameWorld()->get_UserCount_InWorld();
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::NETWORK_Q, guard);
        }

        sleep_ext(1, 0);
        ProcessAntiBotInputWrapper();
    }
}

// ---------------------------------------------------------------------------
// main —— 入口（ORIG 0x829f839）
// ---------------------------------------------------------------------------
int main(int argc, char** argv)
{
    setCodePageByLocal();
    ShowLogo();
    Neof_startupGlobalInstances();

    bool inited = Init(argc, argv);
    if (inited)
        MainLoop();

    if (!inited)
        return 1;

    // 关服线程收尾：等待 Shutdown 线程退出。
    GetInstanceShutdowManager()->WaitUntilExitThread();

    // 删除 pid 文件：sprintf("./pid/%s.pid", G_CEnvironment()->get_file_name())
    // 后 remove；失败打印 "FAIL TO DELETE PID FILE ERROR: %s\n" + strerror(errno)。
    {
        cMyTrace trace("int main(int, char**)", 0x25e6, 0);
        trace("GAME SERVER SHUTDOWN COMPLETE\n");
    }
    char pidFile[0x1e];
    std::memset(pidFile, 0, sizeof(pidFile));
    std::snprintf(pidFile, sizeof(pidFile), "./pid/%s.pid",
                  G_CEnvironment()->get_file_name());
    if (std::remove(pidFile) == -1)
        std::printf("FAIL TO DELETE PID FILE ERROR: %s\n",
                    std::strerror(errno));

    Neof_shutdownGlobalInstances();
    GlobalData::Fini();
    CLog::destroyInstance();
    std::puts("main() exited OK");
    return 0;
}

// ===========================================================================
// 阶段A收尾（undef2_MAIN 批次）—— 入口域缺失符号定义。
// 以下全部为 ORIG 真实符号（nm --defined-only df_game_r 核对），按
// objdump -d 按地址还原；未完整转写的分支以「推断」标注。
// ===========================================================================

// ---------------------------------------------------------------------------
// GlobalInstance 单例 CREATE_*/DESTROY_*（ORIG T 0x82a7029..0x82a7136）：
// 每个 CREATE_X() == GlobalInstance<X>::inst()，DESTROY_X() == destroy()，
// 与 ORIG 反汇编逐一核对（call _ZN14GlobalInstanceIXE4instEv / 7destroyEv）。
// ---------------------------------------------------------------------------
void CREATE_CEnvironment()      { GlobalInstance<CEnvironment>::inst(); }
void DESTROY_CEnvironment()     { GlobalInstance<CEnvironment>::destroy(); }
void CREATE_CGameManager()      { GlobalInstance<CGameManager>::inst(); }
void DESTROY_CGameManager()     { GlobalInstance<CGameManager>::destroy(); }
void CREATE_CDataManager()      { GlobalInstance<CDataManager>::inst(); }
void DESTROY_CDataManager()     { GlobalInstance<CDataManager>::destroy(); }
void CREATE_EpollHandler()      { GlobalInstance<EpollHandler>::inst(); }
void DESTROY_EpollHandler()     { GlobalInstance<EpollHandler>::destroy(); }
void CREATE_Store()             { GlobalInstance<Store>::inst(); }
void DESTROY_Store()            { GlobalInstance<Store>::destroy(); }
void CREATE_TimerQueue()        { GlobalInstance<TimerQueue>::inst(); }
void DESTROY_TimerQueue()       { GlobalInstance<TimerQueue>::destroy(); }

// InterDispatcher / PacketDispatcher / TimerDispatcher —— 权威类型来自
// InterDispatcher.h / PacketDispatcher.h / TimerDispatcher.h（本 TU 顶部 include）。
// 真实构造/析构/注册表在各自 TU（ORIG C1 0x84bbafa / 0x8590a2e / 0x863120a）。
void CREATE_InterDispatcher()   { GlobalInstance<InterDispatcher>::inst(); }
void DESTROY_InterDispatcher()  { GlobalInstance<InterDispatcher>::destroy(); }
void CREATE_PacketDispatcher()  { GlobalInstance<PacketDispatcher>::inst(); }
void DESTROY_PacketDispatcher() { GlobalInstance<PacketDispatcher>::destroy(); }
void CREATE_TimerDispatcher()   { GlobalInstance<TimerDispatcher>::inst(); }
void DESTROY_TimerDispatcher()  { GlobalInstance<TimerDispatcher>::destroy(); }

// ---------------------------------------------------------------------------
// regist_signal(int, void(*)(int)) —— ORIG 0x829f1fc（逐指令转写）：
// sigaction；sa_flags = sig==15 ? 0x20000000 : 0x10000000；
// 失败 printf 后返回 0，成功返回 1。
// ---------------------------------------------------------------------------
int regist_signal(int sig, void (*handler)(int))
{
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = (sig == 15) ? 0x20000000 : 0x10000000;
    if (sigaction(sig, &sa, NULL) < 0)
    {
        std::printf("%d signal register failed\n", sig);  // ORIG 文案为韩文「推断」
        return 0;
    }
    return 1;
}

// signal_handler(int) —— ORIG 0x829f28b（0x234 字节，switch 跳转表）：
// 各信号分支依赖 rdar_terminate/CPacketTracer/CShutdowManager 全链路，
// 其中多个符号尚未建模 —「推断」骨架：SIGTERM 打印后退出，其余忽略。
void signal_handler(int sig)
{
    if (sig == 15)
    {
        std::puts("RECV SIGTERM");
        // ORIG：rdar_terminate → CPacketTracer::{AbsoluteTcp,Udp}WriteLog → …
        // （依赖符号未建模，「推断」省略落盘步骤）
        std::exit(0);
    }
    // 其余信号分支（suspend/shutdown 状态迁移）—「推断」：ORIG 走
    // set_suspend_state / GetInstanceShutdowManager()->EnableShutdown 等，
    // 此处以最小安全路径处理。
}

// omll_dump_core_file() —— ORIG 0x829f6f4（逐指令一致）：abort();
void omll_dump_core_file() { std::abort(); }

// regist_signals() —— ORIG 0x829f4c5（逐指令转写）：
// 15/10/12 → signal_handler（校验）；13 → SIG_IGN（校验）；
// 之后 11/8/4/7/16/23/24/25/31 → signal_handler（不校验）。返回最后一次结果。
void regist_signals()
{
    if (!regist_signal(15, signal_handler)) { return; }  // ORIG: eax=0 返回
    if (!regist_signal(10, signal_handler)) { return; }
    if (!regist_signal(12, signal_handler)) { return; }
    if (!regist_signal(13, (void (*)(int))1)) { return; }  // SIG_IGN
    regist_signal(11, signal_handler);
    regist_signal(8,  signal_handler);
    regist_signal(4,  signal_handler);
    regist_signal(7,  signal_handler);
    regist_signal(16, signal_handler);
    regist_signal(23, signal_handler);
    regist_signal(24, signal_handler);
    regist_signal(25, signal_handler);
    regist_signal(31, signal_handler);
}

// make_dir(char*) —— ORIG _Z8make_dirPc 0x829ee83：「推断」骨架：mkdir 忽略已存在。
void make_dir(char* path)
{
    if (mkdir(path, 0755) != 0 && errno != EEXIST)
    {
        // ORIG 失败分支未逐指令转写 —「推断」：静默忽略
    }
}

// rdar_init(char**) —— ORIG 0x829e5bf（0x1f 字节）：make_dir("./rdar") 后
// 将 argv 交予 rdar 模块（第二个调用目标未建模 —「推断」仅建目录）。
void rdar_init(char** /*argv*/)
{
    static char s_rdarDir[] = "./rdar";  // ORIG 字符串 0x8c18171 内容「推断」
    make_dir(s_rdarDir);
}

// check_cfg_file() —— ORIG 0x829e9ca（0x82 字节）：
// sprintf("<cfg>/%s") + access(F_OK)；存在→true。路径前缀「推断」。
bool check_cfg_file()
{
    char path[0x104];
    std::memset(path, 0, sizeof(path));
    std::snprintf(path, sizeof(path), "./cfg/%s",
                  G_CEnvironment()->get_file_name());
    std::printf("cfg path: %s\n", path);
    return access(path, F_OK) == 0;
}

// check_pid_file() —— ORIG 0x829e948（0x82 字节）：pid 文件存在→true。
bool check_pid_file()
{
    char path[0x104];
    std::memset(path, 0, sizeof(path));
    std::snprintf(path, sizeof(path), "./pid/%s.pid",
                  G_CEnvironment()->get_file_name());
    std::printf("pid path: %s\n", path);
    return access(path, F_OK) == 0;
}

// ncl_check_key(char*) —— ORIG 0x8572cb5（0x167 字节）license key 校验。
// 校验算法未逐指令转写 —「推断」骨架：非空即通过。
int ncl_check_key(char* key)
{
    return (key != 0 && key[0] != '\0') ? 1 : 0;
}

// send_chat_msg(char*, int) —— ORIG 0x829f6ff（0xc6 字节，逐指令转写主体）：
// UDP socket → sockaddr{AF_INET, htons(env->get_udp_port()), htonl(0)} → sendto。
// 目标地址 htonl(0)=0.0.0.0 为 ORIG 原样（推断实际由配置决定回环地址）。
int send_chat_msg(char* msg, int len)
{
    int fd = socket(2, 2, 0);  // AF_INET, SOCK_DGRAM（与 ORIG 立即数一致）
    if (fd < 0)
        return -1;
    sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = 2;
    addr.sin_port = htons(static_cast<unsigned short>(
        G_CEnvironment()->get_udp_port()));
    addr.sin_addr.s_addr = htonl(0);
    int sent = sendto(fd, msg, len, 0,
                      reinterpret_cast<sockaddr*>(&addr), sizeof(addr));
    close(fd);  // ORIG 是否显式 close 未确认 —「推断」补齐防泄漏
    return sent;
}

// send_suspend_signal() —— ORIG 0x829e6f5（0x116 字节）：
// 向本机 UDP 口投递 "suspend" 控制串 —「推断」骨架（报文细节未转写）。
void send_suspend_signal()
{
    char msg[] = "suspend";
    send_chat_msg(msg, static_cast<int>(sizeof(msg) - 1));
}

// printGameServerInfo(bool) —— ORIG 0x829ea4c（0xe 字节，极小尾调）：
// 「推断」骨架：打印服务器基本信息。
void printGameServerInfo(bool /*detail*/)
{
    cMyTrace trace("printGameServerInfo(bool)", 0, 0);
    trace("%s ch=%d", G_CEnvironment()->get_channel_name(),
          G_CEnvironment()->get_channel_no());
}

// CShutdowManager::SendShutdownSignal() —— ORIG 0x82a35a6（0x116 字节）。
// 内部线程通知链未建模 —「推断」骨架：记录日志并置停止标记。
void CShutdowManager::SendShutdownSignal()
{
    GlobalData::s_server_stop = true;
    cMyTrace trace("CShutdowManager::SendShutdownSignal", 0, 0);
    trace("shutdown signal requested");
}

// check_argv(int, char**) —— ORIG 0x829ea5a（0x420 字节，逐指令转写）。
int check_argv(int argc, char** argv)
{
    if (argc <= 2)
        return 0;

    G_CEnvironment()->set_file_name(argv[1]);
    if (argc == 4)
        G_CDataManager()->SetScriptPacks(argv[3]);

    if (!G_CEnvironment()->load())
        return 0;
    G_CEnvironment()->adjust_base_script_directory();

    if (*G_CEnvironment()->get_exchange_server_ip() == '\0')
    {
        std::puts("Exchange Server IP load FAIL, please check cfg file");
        return 0;
    }
    if (G_CEnvironment()->get_exchange_server_port() == 0)
    {
        std::puts("Exchange Server PORT load FAIL, please check cfg file");
        return 0;
    }

    // server_shells_key 校验（env+0x318 缓冲区，ORIG 原样裸偏移）
    char* shellsKey =
        reinterpret_cast<char*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x318);
    bool keyOk = ncl_check_key(shellsKey) != 0;
    std::printf("server_shells_key :: %s\n", shellsKey);  // 格式串截自 ORIG rodata「推断」
    if (!keyOk)
    {
        std::puts("[!] Server key Check FAIL");
        return 0;
    }
    std::puts("[!] Server key Check OK");

    const char* op = argv[2];
    if (std::strcmp(op, "start") == 0 || std::strcmp(op, "nofork") == 0 ||
        std::strcmp(op, "start_all") == 0)
    {
        if (!check_cfg_file())
        {
            std::printf("There is not %s config_file\n",
                        G_CEnvironment()->get_file_name());
            return 0;
        }
        if (check_pid_file())
        {
            std::printf("Process(%s) is already operated.\n",
                        G_CEnvironment()->get_channel_name());
            return 0;
        }
        rdar_init(argv);
        if (std::strcmp(op, "start") == 0)
            G_CEnvironment()->set_operation_type(
                static_cast<eStartArgumentType_t>(0));
        else if (std::strcmp(op, "nofork") == 0)
            G_CEnvironment()->set_operation_type(
                static_cast<eStartArgumentType_t>(2));
        else if (std::strcmp(op, "start_all") == 0)
            G_CEnvironment()->set_operation_type(
                static_cast<eStartArgumentType_t>(1));
        return 1;
    }
    if (std::strcmp(op, "stop") == 0)
    {
        GetInstanceShutdowManager()->SendShutdownSignal();
        return 0;
    }
    if (std::strcmp(op, "suspend") == 0)
    {
        send_suspend_signal();
        return 0;
    }
    if (std::strcmp(op, "test") == 0)
    {
        G_CEnvironment()->set_operation_type(static_cast<eStartArgumentType_t>(3));
        return 1;
    }
    if (std::strcmp(op, "mess") == 0)
    {
        int total = 0;
        char buf[0x400];
        std::memset(buf, 0, sizeof(buf));
        for (int i = 3; i < argc; ++i)
        {
            std::strcat(buf, argv[i]);
            total += static_cast<int>(std::strlen(argv[i]));
            std::strcat(buf, " ");
            ++total;
        }
        if (send_chat_msg(buf, total) < 0)
            std::puts("[ERROR]Fail to Send Notice Message");
        return 0;
    }
    if (std::strcmp(op, "info") == 0)
    {
        printGameServerInfo(false);
        return 0;
    }
    return 0;
}

// ThreadInterface —— 权威定义见 ThreadInterface.h（ORIG 布局 +0 vptr / +4 m_tid /
// +8 m_tidCopy，虚表槽0=stop()、槽3=dispatch(void*)，C1 0x8630b38 / begin 0x8630b8a /
// dispatch_proxy 0x8630c1a / getThreadID 0x82a4218）。

namespace exchange_server
{
// ORIG：BeginExchangeServerThread 0x848f2b8 / getExchangeServerThreadID
// 0x848f2cd / GetInstanceExchangeServerThread 0x8493e8d / CExchangeServerThread::dispatch
// 0x8493bfe（W）。继承权威 ThreadInterface（线程体 dispatch 未建模，走基类默认空体）。
class CExchangeServerThread : public ThreadInterface
{
};
static CExchangeServerThread* s_instance = 0;

CExchangeServerThread* GetInstanceExchangeServerThread()
{
    if (s_instance == 0)
        s_instance = new CExchangeServerThread;
    return s_instance;
}
bool BeginExchangeServerThread()
{
    return GetInstanceExchangeServerThread()->begin();
}
unsigned long getExchangeServerThreadID()
{
    return GetInstanceExchangeServerThread()->getThreadID();
}
}  // namespace exchange_server

// create_thread() —— ORIG 0x829fd30（0x2f8 字节，逐指令转写）：
// 依次 begin 七个全局线程并 setCurrentPacketPool 绑定包池；任一失败返回 0；
// 尾部非 PvP 整合频道（且 channelType!=7）时再启动 exchange 线程。
int create_thread()
{
#define DNF_BEGIN_AND_BIND(obj, type)                                        \
    do                                                                       \
    {                                                                        \
        if (!(obj)->begin())                                                 \
            return 0;                                                        \
        if (!GlobalData::setCurrentPacketPool(                               \
                (type), (obj)->getThreadID()))                               \
            return 0;                                                        \
    } while (0)

    DNF_BEGIN_AND_BIND(reinterpret_cast<ThreadInterface*>(GlobalData::s_dispatch_thr), THREAD_TYPE_NETWORK /*0*/);
    DNF_BEGIN_AND_BIND(reinterpret_cast<ThreadInterface*>(GlobalData::s_udp_thr), THREAD_TYPE_5 /*5*/);
    DNF_BEGIN_AND_BIND(reinterpret_cast<ThreadInterface*>(GlobalData::s_monitor_tcp_thr), THREAD_TYPE_6 /*6*/);
    DNF_BEGIN_AND_BIND(reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr), THREAD_TYPE_7 /*7*/);
    DNF_BEGIN_AND_BIND(reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr_for_logDB),
                       THREAD_TYPE_MONITOR_TCP /*8*/);
    DNF_BEGIN_AND_BIND(reinterpret_cast<ThreadInterface*>(GlobalData::s_accept_thr), THREAD_TYPE_DISPATCH /*1*/);
    DNF_BEGIN_AND_BIND(reinterpret_cast<ThreadInterface*>(GlobalData::s_net_thr), THREAD_TYPE_DB /*2*/);
#undef DNF_BEGIN_AND_BIND

    // ORIG 条件：!IsIntegratedPvPBaseChannel() && GetChannelType()!=7
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel()
        && G_GameWorld()->GetChannelType() != 7)
    {
        exchange_server::BeginExchangeServerThread();
        GlobalData::setCurrentPacketPool(
            THREAD_TYPE_TIMER /*3*/,
            exchange_server::getExchangeServerThreadID());
    }
    return 1;
}

// SelectKeyString(int*, int*) —— ORIG 0x82a002a（0x230 字节）：
// get_rand_int(5) 选表（五张脚本文件列表 map<int,string>：0x94fdc00 /
// 0x94fac00 / 0x94ff260 / 0x9500480 / 0x94ff3a0），表内 get_rand_int(容量)
// 定位条目，返回该条目字符串指针并经出参回写键/值。五张权威表所属全局变量
// 尚未在重建域登记 —「推断」骨架：静态空表占位，返回固定密钥串。
char* SelectKeyString(int* outKey, int* outValue)
{
    static char s_fallbackKey[] = "DNF_SERVER_KEY_STRING";
    if (outKey) *outKey = 0;
    if (outValue) *outValue = 0;
    return s_fallbackKey;
}

// InitCipherKeyString() —— ORIG 0x82a025a（0x1ff 字节，逐指令转写）：
// i=0..0x1387：new Cipher(0x410) 存入 s_secu_cipher_keystring_manager_[i]
// （ORIG 记录 stride 0x10：+0 Cipher*、+4/+8 int 键索引）；
// 首轮经 guard 记录总钥长；重复填充选中密钥串至总钥长后 Initialize。
// 尾部 cMyTrace 输出耗时（文案「推断」）。
struct SecuCipherKeyRec  // ORIG BSS 0x940be80 起 16 字节记录
{
    Cipher* cipher;  // +0x00
    int keyId;       // +0x04
    int tblId;       // +0x08
    char pad[4];     // +0x0c
};

void InitCipherKeyString()
{
    int t0 = static_cast<int>(s_systemTime_.getCurSec());
    static int g_totalKeyLen = 0;  // ORIG guard 变量 0x941f858 / 值 0x941f864

    for (int i = 0; i <= 0x1387; ++i)
    {
        SecuCipherKeyRec* rec = reinterpret_cast<SecuCipherKeyRec*>(
            reinterpret_cast<char*>(GlobalData::s_secu_cipher_keystring_manager_)
            + i * 0x10);
        rec->cipher = new Cipher;
        if (g_totalKeyLen == 0)
            g_totalKeyLen = rec->cipher->GetTotalKeyLength();

        unsigned char buf[0x200];
        std::memset(buf, 0, sizeof(buf));
        char* keyStr = SelectKeyString(&rec->keyId, &rec->tblId);
        int keyLen = static_cast<int>(strlen(keyStr));
        int off = 0;
        while (off < g_totalKeyLen && keyLen > 0)
        {
            memcpy(buf + off, keyStr, keyLen);
            off += keyLen;
        }
        rec->cipher->Initialize(buf, g_totalKeyLen);
    }

    int elapsed = static_cast<int>(s_systemTime_.getCurSec()) - t0;
    cMyTrace trace("InitCipherKeyString()", 0x283e, 0);  // 行号「推断」
    trace("cipher keystring init %d sec", elapsed);
}

// ProcessAntiBotInputWrapper() —— ORIG 0x80eddb0（0xe1 字节，逐指令转写）：
// 计时包裹 s_antibotChecker_ 的虚调用（vtbl+0x10）；耗时 >500ms 记 logFormat(42)。
void ProcessAntiBotInputWrapper()
{
    timeval t0, t1;
    gettimeofday(&t0, 0);

    typedef void (*CheckerFn)(void*);
    void* checker = GlobalData::s_antibotChecker_;
    if (checker)
    {
        CheckerFn fn = reinterpret_cast<CheckerFn>(
            reinterpret_cast<void**>(*reinterpret_cast<void***>(checker))[4]);
        fn(checker);
    }

    gettimeofday(&t1, 0);
    long ms = (t1.tv_sec - t0.tv_sec) * 1000 + (t1.tv_usec - t0.tv_usec) / 1000;
    if (ms > 500)
    {
        LogManager::logFormat(42, "localchina/localchina_App.cpp",
                              "void ProcessAntiBotInputWrapper()", ms,
                              "Delay notice, ProcessAntiBotInputWrapper()",
                              t0.tv_sec, t0.tv_usec, t1.tv_sec, t1.tv_usec);
    }
}

// ProcessServerStatusLog() —— ORIG 0x80ede91（0x4ce 字节）：聚合
// user/party/warroom/deathtower/pvp_room/trade 池占用、消息队列/流池余量、
// creature/avatar 池等十余项指标后统一 logFormat。多数池成员位于
// CGameManager 私有偏移且部分池模板未建模 —「推断」骨架：输出在线用户数。
void ProcessServerStatusLog()
{
    int userCount = G_GameWorld()->get_UserCount_InWorld();
    LogManager::logFormat(0, "localchina/localchina_App.cpp",
                          "void ProcessServerStatusLog()", userCount,
                          "users_in_world=%d");
}
