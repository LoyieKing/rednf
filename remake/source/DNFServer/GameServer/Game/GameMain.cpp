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
#include "Arad_ServerStateManager.h"   // RegistServerSchedule 尾部 checkContinus/loadServerState
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
#include "CInGameAdvertisementManager.h"  // CInGameAdvertisementManager/InGameADManagerInstance（权威头）
#include "GameWorld.h"
#include "HistoryLog.h"
#include "CDeathTowerRanking.h"
#include "CDailyScheduleManager.h"
#include "Arad_DataManager.h"  // ARAD::ScriptManager / Singleton（权威声明）
#include "TaiwanQueries.h"     // Taiwan::AuthGarenaToken 权威（原 TU-local 定义删除）
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
#include <dirent.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <map>
#include <algorithm>

// 定时器注册实现依赖（ORIG G_TimerQueue 0x80f647c / CSystemTime::getCurSec
// 0x80cbc9e / get_rand_int 0x86b1b87 均为 ORIG 真实符号）
#include "PacketTracer.h"   // CPacketTracer / CPacketTracerInstance（signal_handler 依赖）
#include "TaiwanCash.h"      // stIPGStatus_t / CIPGHelper（ProcessServerStatusLog 依赖）
#include "CAvatarItemMgr.h"   // WongWork::CAvatarItemMgr::m_AvatarItemPool（ProcessServerStatusLog 依赖）
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
int regist_signals();                        // _Z14regist_signalsv          0x0829f4c5
int check_argv(int argc, char** argv);       // _Z10check_argviPPc           0x0829ea5a
bool IsLightServer();                        // _Z13IsLightServerv           0x0822ad44 W
int create_thread();                         // _Z13create_threadv           0x0829fd30（返回 bool）
void InitCipherKeyString();                  // _Z19InitCipherKeyStringv     0x082a025a
void RegistServerSchedule();                 // _Z20RegistServerSchedulev    0x082a0459
void sleep_ext(int sec, int usec);           // _Z9sleep_extii
void ProcessAntiBotInputWrapper();           // _Z26ProcessAntiBotInputWrapperv
void ProcessServerStatusLog();               // _Z22ProcessServerStatusLogv

// GetConfigFileString(map<string,string>&, const char*)  _Z19GetConfigFileStringRSt3mapISsSsSt4lessISsESaISt4pairIKSsSsEEEPKc
// ORIG 0x0829d7b3：conf.find(key) 命中且 value 非空 → 返回 value.c_str()；
// 未命中/空值 → logFormat(1,"App.cpp","char* GetConfigFileString(...)",0x1fc7,
// "%s tag error", key) 后返回 ""。
const char* GetConfigFileString(
    std::map<std::string, std::string>& conf, const char* key)
{
    std::string keyStr(key);
    std::map<std::string, std::string>::iterator it = conf.find(keyStr);
    if (it != conf.end() && it->second.length() != 0)
        return it->second.c_str();
    LogManager::logFormat(1, "App.cpp",
                          __PRETTY_FUNCTION__,
                          0x1fc7, "%s tag error", key);
    return "";
}

class CodePage
{
public:
    static void initCodePage();              // _ZN8CodePage12initCodePageEv
};



// np_server_xml::CServerXml::StrLoading()（_ZN13np_server_xml10CServerXml10StrLoadingEv）
namespace np_server_xml
{

class CServerXml
{
public:
    void StrLoading();
};
}  // namespace np_server_xml


// CInGameAdvertisementManager/InGameADManagerInstance 声明改用权威头
// CInGameAdvertisementManager.h（StubS3B：queryADExposeInfo/LoadADExposeInfoAtApp/
// InGameADManagerInstance 真实实现迁移至 CInGameAdvertisementManager.cpp）。

// CmmChannelProxy（ORIG 真实类；头未建，最小声明 —「推断」）
class CmmChannelProxy
{
public:
    void SendChannelInfo();                  // _ZN15CmmChannelProxy15SendChannelInfoEv
};

// 定时器包装类：类型声明已迁至权威头 TimerDispatcher.h（Timer_* 分发器类，
// 静态 RegistNextTimer/registNextTimer 成员与 ORIG nm 一致）；本 TU 仅保留
// 其静态成员定义（下方，逐指令转写）。

// ---------------------------------------------------------------------------
// 定时器注册实现（ORIG 汇编逐条转写；均为 G_TimerQueue()->InsertTimer 包装，
// OBJ_TYPE=2 表示全局对象定时器，param1/key 均为 0）。
// 注：Timer_SecretShopStatistic::RegistNextTimer 与 TimerDispatcher.cpp 重复，
// 已删除（2026-08-29），保留 TimerDispatcher.cpp 权威实现。
// ---------------------------------------------------------------------------
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

    // 0x9a：固定 0 秒（ORIG 0x82a08af，0x8e 之后注册）
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x9a, 0, 0, 0);

    // ORIG 0x82a08cf：尾部 checkContinus + loadServerState（全局 0x941f7f0）
    if (GlobalData::s_serverStateManager_ != 0)
    {
        GlobalData::s_serverStateManager_->checkContinus();
        GlobalData::s_serverStateManager_->loadServerState();
    }
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

    CommonTime time;                          // 栈上对象（ebp-0x18，布局同 ORIG：+0..+4 年/月/日/时/分）
    time.SetCurTime();
    // ORIG 依次读 ebp-0x18/-0x17/-0x16/-0x15/-0x14 五字节（movsbl）→ 年/月/日/时/分
    const char* p = reinterpret_cast<const char*>(&time);
    std::printf("[!] Service Date (%02d-%02d-%02d/%02d:%02d)\n",
                (int)(signed char)p[0], (int)(signed char)p[1],
                (int)(signed char)p[2], (int)(signed char)p[3],
                (int)(signed char)p[4]);      // CommonTime 字段偏移 +0..+4（m_year..m_min）
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
    // ORIG @0x82a08f4：regist_signals 失败 → cout "Fail to regist os signals,
    // Server process is terminated." → return 0（82a090c..82a0937 实证）。
    if (!regist_signals())
    {
        std::cout << "Fail to regist os signals, Server process is terminated."
                  << std::endl;
        return false;
    }

    // ORIG 82a093a..82a0973：getrlimit(RLIMIT_CORE) → rlim_cur=RLIM_INFINITY
    // → setrlimit(RLIMIT_CORE)（不检查返回值）；随后无条件
    // cout "[!] Check Args ..."（82a0976..82a0998）。
    struct rlimit rl;
    getrlimit(RLIMIT_CORE, &rl);
    rl.rlim_cur = static_cast<rlim_t>(-1);
    setrlimit(RLIMIT_CORE, &rl);

    std::cout << "[!] Check Args ..." << std::endl;
    if (!check_argv(argc, argv))
    {
        // ORIG 82a09b3..82a09dc：cout "[!] Check Args Fail" → return 0。
        std::cout << "[!] Check Args Fail" << std::endl;
        return false;
    }
    // ORIG 82a09e1..82a0a02：cout "[!] Check Args Success"。
    std::cout << "[!] Check Args Success" << std::endl;

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
            static_cast<ENUM_SERVER_GROUP>(
                G_CEnvironment()->get_gc_no_hardcode(E_SERVER_GROUP_NONE)),
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
// MainLoop() —— ORIG 0x829fa2b（0x305）：主循环每秒一轮。ORIG 局部为
// 周期/计数器组：-0x34 每 30 轮 UpdateChannelUser+状态日志；-0x40 每 10 轮
// 心跳（monitor/guild proxy 遍历，GetServerProxy(组号)）；-0x24 每 2 轮
// 翻转 CmmChannelProxy 上报；-0x1c 每 0x12c 轮向 NETWORK_Q 投递心跳流包
// （Acquire("App.cpp", 0x2728) + << 0x30a << -1）。退出条件
// !GlobalData::s_server_stop（@0x940bd64），返回 1。
// ---------------------------------------------------------------------------
void MainLoop()
{
    int cnt30 = 0;        // -0x34
    int cnt10 = 0;        // -0x40
    int flip24 = 0;       // -0x24
    int cnt300 = 0;       // -0x1c
    int lastUserCount = 0; // -0x30
    int lastRemain = 0;    // -0x2c

    while (!GlobalData::s_server_stop)
    {
        if (cnt30 == 30)
        {
            G_GameWorld()->UpdateChannelUser();
            int curUser = G_GameWorld()->get_UserCount_InWorld();
            int remain =
                G_CGameManager()->userPoolSize() - G_CGameManager()->userPoolRemain();
            if (lastUserCount != curUser || remain != lastRemain)
            {
                ProcessServerStatusLog();
                lastUserCount = curUser;
                lastRemain = remain;
            }
            cnt30 = 0;
        }

        if (cnt10 > 9)
        {
            for (int mi = GlobalData::s_monitor_proxy_mgr->GetStartIndex(); ; )
            {
                CMonitorServerProxy* mp =
                    GlobalData::s_monitor_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)mi);
                mp->SendHeartBeat(G_CEnvironment()->get_channel_no());
                mi = GlobalData::s_monitor_proxy_mgr->GetNextIndex(mi);
                if (mi >= GlobalData::s_monitor_proxy_mgr->GetEndIndex())
                    break;
            }
            for (int gi = GlobalData::s_guild_proxy_mgr->GetStartIndex(); ; )
            {
                CGuildServerProxy* gp =
                    GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)gi);
                gp->SendHeartBeat(G_CEnvironment()->get_channel_no());
                gi = GlobalData::s_guild_proxy_mgr->GetNextIndex(gi);
                if (gi >= GlobalData::s_guild_proxy_mgr->GetEndIndex())
                    break;
            }
            cnt10 = 0;
        }
        ++cnt10;

        if (flip24 == 1)
        {
            GlobalData::s_channel_proxy->SendChannelInfo();
            flip24 = 0;
        }
        else
        {
            flip24 = 1;
        }

        if (cnt300 == 300)
        {
            Stream* s = GlobalData::s_small_stream_pool->Acquire("App.cpp", 0x2728);
            CStreamGuard guard(s, true);
            guard << 0x30a;
            guard << -1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::NETWORK_Q, guard);
            cnt300 = 0;
        }
        ++cnt300;

        sleep_ext(1, 0);
        ++cnt30;
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
        cMyTrace trace(__PRETTY_FUNCTION__, 0x25e6, 0);
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
// sigaction；sa_flags = sig==14(SIGALRM) ? 0x20000000 : 0x10000000（ORIG 实证 cmpl $0xe）；
// 失败 printf 后返回 0，成功返回 1。
// ---------------------------------------------------------------------------
int regist_signal(int sig, void (*handler)(int))
{
    struct sigaction sa;
    std::memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = (sig == 14) ? 0x20000000 : 0x10000000;
    if (sigaction(sig, &sa, NULL) < 0)
    {
        std::printf("%d\xb9\xf8 signal \xb5\xee\xb7\xcf \xbd\xc7\xc6\xd0\n", sig);  // ORIG 文案（CP949 韩文）
        return 0;
    }
    return 1;
}

void rdar_terminate();     // ORIG T 0x0829e80b（本文件尾部定义）
void omll_dump_core_file(); // ORIG T 0x0829f6f4（本文件已有定义）
void TerminateServers();    // ORIG T 0x082a21f9（本文件尾部定义）

void signal_handler(int sig)
{
    switch (sig)
    {
    case 15:
    {
        std::puts("RECV SIGTERM");
        rdar_terminate();
        CPacketTracer* tracer = CPacketTracerInstance();
        tracer->AbsoluteTcpWriteLog();
        tracer = CPacketTracerInstance();
        tracer->AbsoluteUdpWriteLog();
        break;
    }
    case 10:
    {
        CEnvironment* env = G_CEnvironment();
        std::printf("RDAR_SUSPEND pointer: %p\n", static_cast<void*>(env));
        env = G_CEnvironment();
        env->set_suspend_state(true);
        break;
    }
    case 12:
    {
        std::puts("RECV SIGUSR2");
        CEnvironment* env = G_CEnvironment();
        std::printf("G_CEnvironment pointer: %p\n", static_cast<void*>(env));
        env = G_CEnvironment();
        env->set_suspend_state(true);
        GetInstanceShutdowManager()->Reset();
        GetInstanceShutdowManager()->EnableShutdown(true);
        GameWorld* world = G_GameWorld();
        const int userCount = world->get_UserCount_InWorld();
        {
            cMyTrace trace(__PRETTY_FUNCTION__, 0x2459, 0);
            trace("current user count (%d)\n", userCount);
        }
        GameWorld* world2 = G_GameWorld();
        if (world2->get_UserCount_InWorld() < 1)
        {
            GetInstanceShutdowManager()->OnAllUserLogout();
        }
        else
        {
            GameWorld* world3 = G_GameWorld();
            world3->arrange_users();
        }
        break;
    }
    case 6:
    case 11:
    {
        std::puts("Recv SIGSEGV signal --> make Dump Core file.");
        CPacketTracer* tracer = CPacketTracerInstance();
        tracer->AbsoluteTcpWriteLog();
        tracer = CPacketTracerInstance();
        tracer->AbsoluteUdpWriteLog();
        omll_dump_core_file();
        break;
    }
    case 8:
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__,
                              0x2478, "Recv FPE signal");
        CPacketTracer* tracer = CPacketTracerInstance();
        tracer->AbsoluteTcpWriteLog();
        tracer = CPacketTracerInstance();
        tracer->AbsoluteUdpWriteLog();
        omll_dump_core_file();
        break;
    }
    case 5:
	case 9:
	case 13:
	case 14:
	case 17:
	case 18:
	case 19:
	case 20:
	case 21:
	case 22:
	case 26:
	case 27:
	case 28:
		break;
	case 4:
    case 7:
    case 16:
    case 23:
    case 24:
    case 25:
    case 29:
    case 30:
    case 31:
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__,
                              0x2486,
                              "Recv %d signal --> process is terminated abnormaly.",
                              sig);
        CPacketTracer* tracer = CPacketTracerInstance();
        tracer->AbsoluteTcpWriteLog();
        tracer = CPacketTracerInstance();
        tracer->AbsoluteUdpWriteLog();
        omll_dump_core_file();
        std::exit(-1);
        break;
    }
}

// ORIG 0x0829e80b（_Z14rdar_terminatev，T，87 条）：
//   buf[30] 清零；sprintf("./pid/%s.pid", G_CEnvironment()->get_file_name())；
//   printf("???%s\n")；remove 失败 → perror 风格 strerror 输出，
//   成功 → puts RDAR TERMINATED；arrange_users；TerminateServers()。
void rdar_terminate()
{
    char buf[30];
    std::memset(buf, 0, sizeof(buf));
    std::sprintf(buf, "./pid/%s.pid", G_CEnvironment()->get_file_name());
    std::printf("???%s\n", buf);
    if (std::remove(buf) == -1)
    {
        std::printf("FAIL TO DELETE PID FILE ERROR: %s\n",
                    strerror(*__errno_location()));
    }
    else
    {
        std::puts("RDAR PROCESS IS TERMINATED.");
    }
    G_GameWorld()->arrange_users();
    TerminateServers();
}

// ORIG 0x082a21f9（_Z16TerminateServersv，T，189 字节/91 条）：
//   GlobalData::s_stream_pool->Acquire("App.cpp",0x2e2a) → CStreamGuard(s,true)
//   << 0x3e << 0；s_msgq_mgr->put((QUEUE_IDX)1, guard)。
void TerminateServers()
{
    Stream* s =
        GlobalData::s_stream_pool->Acquire((const char*)"App.cpp", 0x2e2a);
    {
        CStreamGuard guard(s, true);
        guard << 0x3e;
        guard << 0;
        GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
    }
}

// omll_dump_core_file() —— ORIG 0x829f6f4（逐指令一致）：abort();
void omll_dump_core_file() { std::abort(); }

// 15/10/12/13/11/8/4/7/16/23/24/25/31 → signal_handler（13 → SIG_IGN），
// 任一注册失败即返回 0，全部成功返回 1。
int regist_signals()
{
    if (!regist_signal(15, signal_handler)) { return 0; }
    if (!regist_signal(10, signal_handler)) { return 0; }
    if (!regist_signal(12, signal_handler)) { return 0; }
    if (!regist_signal(13, (void (*)(int))1)) { return 0; }  // SIG_IGN
    if (!regist_signal(11, signal_handler)) { return 0; }
    if (!regist_signal(8,  signal_handler)) { return 0; }
    if (!regist_signal(4,  signal_handler)) { return 0; }
    if (!regist_signal(7,  signal_handler)) { return 0; }
    if (!regist_signal(16, signal_handler)) { return 0; }
    if (!regist_signal(23, signal_handler)) { return 0; }
    if (!regist_signal(24, signal_handler)) { return 0; }
    if (!regist_signal(25, signal_handler)) { return 0; }
    if (!regist_signal(31, signal_handler)) { return 0; }
    return 1;
}

// make_dir(char*) —— ORIG _Z8make_dirPc 0x829ee83：opendir 探测，不存在则 mkdir 0777。
void make_dir(char* path)
{
    if (opendir(path) == 0)
    {
        mkdir(path, 0777);
    }
}

// rdar_init(char**) —— ORIG 0x829e5bf（0x1f 字节）：make_dir("./rdar") 后
// 将 argv 交予 daemon_init（ORIG 0x829e8b9 调用，argv[2]="start"/"start_all"
// 时 fork 守护化；定义 OnlyOrigBatch11.cpp，ORIG T 0x0829e8b9）。
int daemon_init(char** argv);
void rdar_init(char** argv)
{
    static char s_rdarDir[] = "./rdar";  // ORIG 字符串 0x8c18171 内容「推断」
    make_dir(s_rdarDir);
    daemon_init(argv);
}

// check_cfg_file() —— ORIG 0x829e9ca（0x82 字节）：
// sprintf("<cfg>/%s") + access(F_OK)；存在→true。路径前缀「推断」。
bool check_cfg_file()
{
    char path[0x104];
    std::memset(path, 0, sizeof(path));
    std::snprintf(path, sizeof(path), "./cfg/%s.cfg",
                  G_CEnvironment()->get_file_name());
    std::printf("\t- cfg path: %s\n", path);
    return access(path, F_OK) == 0;
}

// check_pid_file() —— ORIG 0x829e948（0x82 字节）：pid 文件存在→true。
bool check_pid_file()
{
    char path[0x104];
    std::memset(path, 0, sizeof(path));
    std::snprintf(path, sizeof(path), "./pid/%s.pid",
                  G_CEnvironment()->get_file_name());
    std::printf("\t- pid path: %s\n", path);
    return access(path, F_OK) == 0;
}

// ncl_check_key(char*) 真实实现（ORIG 0x8572cb5：decrypt → MAC/日期区间校验）
// 已移至 NclKeyTools.cpp，本 TU 仅保留调用点。

// send_chat_msg(char*, int) —— ORIG 0x829f6ff（0xc6 字节，逐指令转写主体）：
// UDP socket → sockaddr{AF_INET, htons(env->get_udp_port()), htonl(0)} → sendto。
// 目标地址 htonl(0)=0.0.0.0 为 ORIG 原样（推断实际由配置决定回环地址）。
int send_chat_msg(char* msg, int len)
{
    // ORIG 0x829f6ff 逐指令转写：UDP socket → sockaddr{AF_INET,
    // htons(get_udp_port()), htonl(0)}（其余字段不初始化，无 memset）；
    // len>0x4f 返回 -1；包 = {0x03, msg[:0x50]} 共 0x52 字节发送；无 close、无 fd 检查。
    char buf[0x52];
    sockaddr_in addr;
    int fd = socket(2, 2, 0);  // AF_INET, SOCK_DGRAM（与 ORIG 立即数一致）
    addr.sin_family = 2;
    addr.sin_addr.s_addr = htonl(0);
    addr.sin_port = htons(static_cast<unsigned short>(
        G_CEnvironment()->get_udp_port()));
    if (len > 0x4f)
        return -1;
    buf[0] = 0x3;
    strncpy(buf + 1, msg, 0x50);
    int sent = sendto(fd, buf, 0x52, 0,
                      reinterpret_cast<sockaddr*>(&addr), 0x10);
    return sent;
}

// send_suspend_signal() —— ORIG 0x829e6f5（0x116 字节）：
// 读进程 pid 文件（pid/%s.pid，%s = get_file_name()），向 pid 发 SIGUSR1(10)。
void send_suspend_signal()
{
    char buf[0x1e];
    int pid;
    int r;
    FILE* f;
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "pid/%s.pid", G_CEnvironment()->get_file_name());
    f = fopen(buf, "r");
    if (f == 0)
    {
        printf("%s process id file open ½ÇÆÐ\n", buf);
        return;
    }
    fscanf(f, "%d", &pid);
    if (pid <= 0)
    {
        fclose(f);
        printf("%d¹øÀÇ Àß¸øµÈ process id\n", pid);
        return;
    }
    r = kill(pid, 10);
    if (r < 0)
    {
        fclose(f);
        printf("%d¹ø process·Î Á¾·á signal ¼Û½Å ½ÇÆÐ", pid);
        return;
    }
    printf("SEND SUSPEND SIGNAL TO %d\n", pid);
    fclose(f);
}

// printGameServerInfo(bool) —— ORIG 0x829ea4c（0xe 字节，极小尾调）：
// 「推断」骨架：打印服务器基本信息。
void printGameServerInfo(bool /*detail*/)
{
    // ORIG 0x829ea4c：空函数（0xe 字节：保存参数后直接返回）。
}

// CShutdowManager::SendShutdownSignal() —— ORIG 0x82a35a6（0x116 字节）：
// 与 send_suspend_signal 同构，向 pid 文件记录的进程发 SIGTERM(12)。
void CShutdowManager::SendShutdownSignal()
{
    char buf[0x1e];
    int pid;
    int r;
    FILE* f;
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "pid/%s.pid", G_CEnvironment()->get_file_name());
    f = fopen(buf, "r");
    if (f == 0)
    {
        printf("%s process id file open ½ÇÆÐ\n", buf);
        return;
    }
    fscanf(f, "%d", &pid);
    if (pid <= 0)
    {
        fclose(f);
        printf("%d¹øÀÇ Àß¸øµÈ process id\n", pid);
        return;
    }
    r = kill(pid, 12);
    if (r < 0)
    {
        fclose(f);
        printf("%d¹ø process·Î Á¾·á signal ¼Û½Å ½ÇÆÐ", pid);
        return;
    }
    printf("SEND SHUTDOWN SIGNAL TO %d\n", pid);
    fclose(f);
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
    int ncl_check_key(char* key);  // NclKeyTools.cpp（ORIG 0x8572cb5）
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

// SelectKeyString(int*, int*) —— ORIG 0x82a002a（0x230 字节，逐指令转写）：
// get_rand_int(5) 选表（五张脚本文件列表 map<int,string>：0x94fdc00 /
// 0x94fac00 / 0x94ff260 / 0x9500480 / 0x94ff3a0），表内 get_rand_int(容量)
// 定位条目，返回该条目字符串指针并经出参回写键/值。
// 五张表 = g_stStackableScriptFileList / g_stEquipmentScriptFileList /
// g_stMonsterScriptFileList / g_questScriptFileList / g_stPassiveObjectScriptFileList。
extern std::map<int, std::string> g_stStackableScriptFileList;    // ORIG B 0x94fdc00
extern STScriptFileList g_stEquipmentScriptFileList;              // ORIG B 0x94fac00
extern std::map<int, std::string> g_stMonsterScriptFileList;      // ORIG B 0x94ff260
extern std::map<int, std::string> g_questScriptFileList;        // ORIG B 0x9500480
extern std::map<int, std::string> g_stPassiveObjectScriptFileList;// ORIG B 0x94ff3a0
char* SelectKeyString(int* outKey, int* outValue)
{
    static char s_fallbackKey[] = "DNF_SERVER_KEY_STRING";
    int r = get_rand_int(5);
    std::map<int, std::string>* tbl = 0;
    int n = 0;
    switch (r)
    {
    case 0: tbl = &g_stStackableScriptFileList;    n = get_rand_int(0x7d0); break;
    case 1: tbl = &g_stEquipmentScriptFileList.m_fileMap; n = get_rand_int(0x1388); break;
    case 2: tbl = &g_stMonsterScriptFileList;      n = get_rand_int(0x1f4); break;
    case 3: tbl = &g_questScriptFileList;          n = get_rand_int(0x3e8); break;
    case 4: tbl = &g_stPassiveObjectScriptFileList; n = get_rand_int(0x3e8); break;
    default: tbl = &g_questScriptFileList;         n = 0; break;
    }
    std::map<int, std::string>::iterator it = tbl->begin();
    std::map<int, std::string>::iterator e = tbl->end();
    for (int i = 0; i < n; ++i)
    {
        ++it;
        if (it == e)
        {
            --it;
            break;
        }
    }
    if (outKey) *outKey = r;
    if (outValue) *outValue = it->first;
    return const_cast<char*>(it->second.c_str());
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
    static int needed_key_length = 0;  // ORIG guard 变量 0x941f858 / 值 0x941f864

    for (int i = 0; i <= 0x1387; ++i)
    {
        SecuCipherKeyRec* rec = reinterpret_cast<SecuCipherKeyRec*>(
            reinterpret_cast<char*>(GlobalData::s_secu_cipher_keystring_manager_)
            + i * 0x10);
        rec->cipher = new Cipher;
        if (needed_key_length == 0)
            needed_key_length = rec->cipher->GetTotalKeyLength();

        unsigned char buf[0x200];
        std::memset(buf, 0, sizeof(buf));
        char* keyStr = SelectKeyString(&rec->keyId, &rec->tblId);
        int keyLen = static_cast<int>(strlen(keyStr));
        int off = 0;
        while (off < needed_key_length && keyLen > 0)
        {
            memcpy(buf + off, keyStr, keyLen);
            off += keyLen;
        }
        rec->cipher->Initialize(buf, needed_key_length);
    }

    int elapsed = static_cast<int>(s_systemTime_.getCurSec()) - t0;
    cMyTrace trace("void InitCipherKeyString()", 0x283e, 9, true, true);
    trace("[!] Init CipherKeyString TotalTime : %ds", elapsed);
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
        LogManager::logFormat(1, "localchina/localchina_App.cpp",
                              __PRETTY_FUNCTION__, 42,
                              "Delay notice, ProcessAntiBotInput : %d, %ld, %ld, %ld, %ld",
                              ms, t0.tv_sec, t0.tv_usec, t1.tv_sec, t1.tv_usec);
    }
}

// user_creature 全局池尺寸包装（定义于 CreatureOnlySimple.cpp，无独立头）。
namespace user_creature {
int GetCreatureEggPoolSize();
int GetCreaturePoolSize();
int GetCreatureEggPoolRemainSize();
int GetCreaturePoolRemainSize();
}

// ProcessServerStatusLog() —— ORIG 0x80ede91（0x4ce 字节，逐指令转写）：
// 聚合 user/party/warroom/deathtower/pvp_room/trade 池占用、消息队列/流池余量、
// IPG 状态、creature/avatar/egg 池等指标后统一 cMyTrace 输出，随后按用户数
// 低于 party/pvp/trade 池占用打印三条 SESSION ERROR 日志。
void ProcessServerStatusLog()
{
    int userCount = G_GameWorld()->get_UserCount_InWorld();          // -0x6c
    int userUsed = G_CGameManager()->m_userPool.Size() -
                   G_CGameManager()->m_userPool.Remain();            // -0x68
    int partyUsed = G_CGameManager()->m_partyPool.Size() -
                    G_CGameManager()->m_partyPool.Remain();          // -0x64
    int warRoomUsed = G_CGameManager()->m_warRoomPool.Size() -
                      G_CGameManager()->m_warRoomPool.Remain();      // -0x60
    int deathTowerUsed = G_CGameManager()->m_deathTowerPool.Size() -
                         G_CGameManager()->m_deathTowerPool.Remain();// -0x5c
    int pvpUsed = G_CGameManager()->m_pvpRoomPool.Size() -
                  G_CGameManager()->m_pvpRoomPool.Remain();          // -0x58
    int tradeUsed = G_CGameManager()->m_tradeSpacePool.Size() -
                    G_CGameManager()->m_tradeSpacePool.Remain();     // -0x54
    int msgq1 = static_cast<int>(GlobalData::s_msgq_mgr->remain(
        MsgQueueMgr::QUEUE_IDX::DISPATCH_Q));                      // -0x50
    int msgq2 = static_cast<int>(GlobalData::s_msgq_mgr->remain(
        MsgQueueMgr::QUEUE_IDX::DB_Q));                              // -0x4c
    int streamRemain = GlobalData::s_stream_pool->Remain();          // -0x48
    int streamSize = GlobalData::s_stream_pool->Size();              // -0x44
    int smallRemain = GlobalData::s_small_stream_pool->Remain();     // -0x40
    int smallSize = GlobalData::s_small_stream_pool->Size();         // -0x3c
    int bigRemain = GlobalData::s_big_stream_pool->Remain();         // -0x38
    int bigSize = GlobalData::s_big_stream_pool->Size();             // -0x34

    WongWork::IPG::stIPGStatus_t status;                             // -0x88
    GlobalData::s_pIPGHelper->getIPGStatus(status);

    int avatarRemain = WongWork::CAvatarItemMgr::m_AvatarItemPool.Remain();    // -0x30
    int avatarSize = WongWork::CAvatarItemMgr::m_AvatarItemPool.Size();        // -0x2c
    int creatureRemain = user_creature::GetCreaturePoolRemainSize(); // -0x28
    int creatureSize = user_creature::GetCreaturePoolSize();         // -0x24
    int eggRemain = user_creature::GetCreatureEggPoolRemainSize();   // -0x20
    int eggSize = user_creature::GetCreatureEggPoolSize();           // -0x1c

    int tradeSize = G_CGameManager()->m_tradeSpacePool.Size();       // ebx
    int pvpSize = G_CGameManager()->m_pvpRoomPool.Size();            // esi
    int deathTowerSize = G_CGameManager()->m_deathTowerPool.Size();  // -0xa4
    int warRoomSize = G_CGameManager()->m_warRoomPool.Size();        // -0xa0
    int partySize = G_CGameManager()->m_partyPool.Size();            // -0x9c
    int userSize = G_CGameManager()->m_userPool.Size();              // -0x98
    int userSize2 = G_CGameManager()->m_userPool.Size();             // -0x94

    {
        cMyTrace trace("void ProcessServerStatusLog()", 0x5c, 0);    // -0x7c
        trace(
            "\n[DISPATCHER_Q (%3d) DB_Q (%3d) IPGQueue_Size (%3d)]\n"
            "[Small_Pool (%3d/%3d) Default_Pool (%3d/%3d) Big_Pool (%3d/%3d) "
            "IPGPool_Size(%3d/%3d)]\n"
            "[AvatarPool_Size(%3d/%3d), CreaturePoolSize(%3d/%3d), "
            "CreatureEggPoolSize(%3d/%3d)]\n"
            "[Sessions (%3d/%3d) Users (%3d/%3d) Parties (%3d/%3d) "
            "WarRooms (%3d/%3d) DeathTowers (%3d/%3d) PVPs (%3d/%3d) "
            "Trades (%3d/%3d)]",
            msgq1, msgq2, status.m_field08,
            smallSize, smallRemain, streamSize, streamRemain,
            bigSize, bigRemain, status.m_field04, status.m_field00,
            avatarSize, avatarRemain, creatureSize, creatureRemain,
            eggSize, eggRemain,
            userUsed, userSize2, userCount, userSize,
            partyUsed, partySize, warRoomUsed, warRoomSize,
            deathTowerUsed, deathTowerSize, pvpUsed, pvpSize,
            tradeUsed, tradeSize);
    }

    if (userCount < partyUsed)
    {
        LogManager::logFormat(1, "localchina/localchina_App.cpp",
                              "void ProcessServerStatusLog()", 0x77,
                              "[SESSION ERROR] CurrentUserCount(%d) < CurrentPartyCount(%d)",
                              userCount, partyUsed);
    }
    if (userCount < pvpUsed)
    {
        LogManager::logFormat(1, "localchina/localchina_App.cpp",
                              "void ProcessServerStatusLog()", 0x79,
                              "[SESSION ERROR] CurrentUserCount(%d) < CurrentPVPCount(%d)",
                              userCount, pvpUsed);
    }
    if (userCount < tradeUsed)
    {
        LogManager::logFormat(1, "localchina/localchina_App.cpp",
                              "void ProcessServerStatusLog()", 0x7b,
                              "[SESSION ERROR] CurrentUserCount(%d) < CurrentTraceSpaceCount(%d)",
                              userCount, tradeUsed);
    }
}
