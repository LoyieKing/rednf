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
// [b28d-28] DIFF | CServerProxyMgr<T>::createServerProxy(int,int,ENUM_SERVER_GROUP,
// char*,int,int) 三实例（ORIG 0x082b3dc2 CMonitor / 0x082b4018 CGuild /
// 0x082b426e CStatistic，定义在共享头 CServerProxyMgr.h、本 TU 隐式实例化）：
// N104/N82：EH landing 块序（4.1 直落 vs 4.4 `jmp` 岛）+ 帧 0x3c vs 0x4c +
// 槽平移 0x10；`lea 0x0(,%ebx,4)` vs `shl $0x2`（N94 族）。
// [b28d-solo-GameMain] N83/N104 封顶 | 192/191 三实例同形 | p167 三实例一致：
// MISSING `T::D1Ev`+`_ZdaPv` / EXTRA `_ZnwjPv`+`_ZdlPvS_` —— 即 **N83**：ORIG 4.1
// 把 `new ((T*)p) T();` 的 placement operator new **前端内联**（0 call），4.4-O0
// 恒发 extern `_ZnwjPv` call + size 装载行 + `_ZdlPvS_` EH cleanup（ctor 加
// throw() 零效，N83 已实证）；叠加 N104（EH landing 块序）+ 帧 0x3c vs 0x4c +
// 槽平移 + `lea 0x0(,%ebx,4)` vs `shl $0x2`（N94 族）。模板定义在共享头
// CServerProxyMgr.h（GlobalData 等 TU 亦实例化，非本泳道写权）⇒ 不改，N 封顶。
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
bool regist_signals();                       // _Z14regist_signalsv          0x0829f4c5（[b28d-Main·P321-T3] 调用点 xor $1 ⇒ bool）
bool check_argv(int argc, char** argv);      // _Z10check_argviPPc           0x0829ea5a（同上）
bool IsLightServer();                        // _Z13IsLightServerv           0x0822ad44 W
bool create_thread();                        // _Z13create_threadv           0x0829fd30（同上）
void InitCipherKeyString();                  // _Z19InitCipherKeyStringv     0x082a025a
void RegistServerSchedule();                 // _Z20RegistServerSchedulev    0x082a0459
void sleep_ext(int sec, int usec);           // _Z9sleep_extii
void ProcessAntiBotInputWrapper();           // _Z26ProcessAntiBotInputWrapperv
void ProcessServerStatusLog();               // _Z22ProcessServerStatusLogv

// ProcessAntiBotInputWrapper() —— ORIG 0x80eddb0（0xe1 字节，逐指令转写）：
// 计时包裹 s_antibotChecker_ 的虚调用（vtbl+0x10）；耗时 >500ms 记 logFormat(42)。
// [b28d-28] DIFF | ORIG 0x080eddb0 | P282/N7：虚调用偏移 vptr+0x10 已核（word[4]）；
// ORIG 无判空（P171 照抄）已修 + 死局部 `limit=500` 已补；残差仅 fn 落 %edx（ORIG）
// vs %eax（4.4.7 间接 call 恒 %eax）⇒ N7 封顶（P230：4.4.6 后端可解，待路由）。
// [b28d-solo-GameMain] NEAR | 63=63 | 修法：函数指针值在调用点 cast（旧形经
// `CheckerFn*` 解引用多发一条 `mov (%eax),%eax`）；残差 N7（间接 call 目标 4.4.7
// 恒 %eax vs ORIG %edx）。**446 后端实测反劣**（-B cc1plus446bin/ 编译本 TU：
// 本件 NEAR→DIFF，TU near 3→2 / diff 12→13，AE 合计 263 不变）⇒ 不推荐路由，
// 按 N7 在 4.4.7 封顶。
void ProcessAntiBotInputWrapper()
{
    // [b28d-28] ORIG 0x080eddb7 `movl $0x1f4,-0x10(%ebp)`：死局部（P171 照抄，
    // 后续比较仍用立即数 `cmpl $0x1f4,-0xc`）。
    long limit = 500;
    (void)limit;
    timeval t0, t1;
    gettimeofday(&t0, 0);

    // [b28d-28] ORIG 0x080eddd1：无判空、无具名 fn/checker 局部——单表达式虚调
    // `mov 0x941f7e4,%eax; mov (%eax),%eax; add $0x10,%eax; mov (%eax),%edx;
    // mov 0x941f7e4,%eax; mov %eax,(%esp); call *%edx`（P282：vptr+0x10 = word[4]，
    // 外部 SDK 类未建模故保留函数指针形）。
    typedef void (*CheckerFn)(void*);
    // 函数指针值直接在调用点 cast（P0 无具名局部）：`(...)[4]` 已取到 fn 值，
    // 再经 `CheckerFn*` 解引用会多发一条 `mov (%eax),%eax`（我方 4 条 mov 链 vs ORIG 3）。
    reinterpret_cast<CheckerFn>(
        (*reinterpret_cast<void***>(GlobalData::s_antibotChecker_))[0x10 / 4])(
        GlobalData::s_antibotChecker_);

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

// GetConfigFileString(map<string,string>&, const char*)  _Z19GetConfigFileStringRSt3mapISsSsSt4lessISsESaISt4pairIKSsSsEEEPKc
// ORIG 0x0829d7b3：conf.find(key) 命中且 value 非空 → 返回 value.c_str()；
// 未命中/空值 → logFormat(1,"App.cpp","char* GetConfigFileString(...)",0x1fc7,
// "%s tag error", key) 后返回 ""。
// [b28d-28] DIFF | ORIG 0x0829d7b3 | N82：迭代器/string 临时槽序 + 槽平移。
// [b28d-solo-GameMain] AE/IDENTICAL | 92=92 | p167 24/24 CLEAR | 修法：find(key)
// 隐式临时 string（ORIG find 后立即 `call string::~string` + EH 链覆盖 find）+
// P225 while 形复现 `test %eax,%eax; setne %al; test %al,%al; je` 物化。
const char* GetConfigFileString(
    std::map<std::string, std::string>& conf, const char* key)
{
    // ORIG 0x0829d7da：find 的实参是 `const char*` 隐式转换的**临时** string
    // （-0x14 匿名槽 + -0xd allocator），find 返回后立即 `call string::~string`
    // （完整表达式结束），EH 块覆盖 find 并析构该临时——不是具名 keyStr。
    std::map<std::string, std::string>::iterator it = conf.find(key);
    // ORIG 0x0829d884 `test %eax,%eax; setne %al; test %al,%al; je`：`&&` 第二
    // 操作数的 bool 物化；4.4 对 `&&` 直接短路折 je，须改 P225 while 形复现
    // （语句表达式会额外物化 `mov $1/$0,%eax` 5 条）。
    if (it != conf.end())
    {
        while (it->second.length() != 0)
        {
            return it->second.c_str();
        }
    }
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
    int interval = 0xe10;   // ORIG -0xc 槽预置（写而不读的 dead local，帧形证据）
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
        t.tm_sec = 0;   // ORIG 0x86378af：movl $0x0,-0x3c(%ebp)（真语义：清残留秒）
        ms = mktime(&t);
    }
    int delay = static_cast<int>(ms) - s_systemTime_.getCurSec();
    // ORIG 0x86378f5：std::max 结果直接入 ebx 实参（mov (%eax),%ebx），无回写 delay 槽
    //（P24 内联实参形）。b28c-33：ORIG 实参槽序为 (%esp)=-0xc(常量 1)、0x4(%esp)=-0x10
    //（delay）⇒ 原码写 std::max(1, delay)（交换律等价，仅装载序差异）。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)6, 0,
                                (TIMER_MESSAGE)0x73, std::max(1, delay), 0, 0);
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
// [b28d-28] DIFF | ORIG 0x0863591a | P196/N82：比较方向反演（ORIG jl/jle vs
// 4.4 jg/jge）+ 槽号平移 4。
// [b28d-solo-GameMain] AE | 81=81 | 修法（两旋钮）：①`std::time_t next;` 提前
// 到 found 之前声明（P244 声明/赋值分离）⇒ 4.4 槽序翻正 next@-0x14 / found@-0xd
// 与 ORIG 全等（旧判「N98 槽序不可控」作废：4.4 按声明序分配，提前声明即可）；
// ②`now > next` 交换比较操作数 ⇒ 复现 ORIG `cmp -0x14(%ebp),%eax; jle`
// （旧 `next < now` 产 `cmp %eax,-0x14(%ebp)`，N18b 方向靠交换操作数翻正）。
// 另保留 P196 `cur.tm_hour >= m_nTimeTable[i]` 装载序形。
int TimerStatisticLevel::CalcNextTime()
{
    std::time_t now = static_cast<std::time_t>(s_systemTime_.getCurSec());
    std::tm cur;
    localtime_r(&now, &cur);
    // [b28d-solo-GameMain] 槽序实验：ORIG next@-0x14 / found@-0xd（next 先于
    // found 分配）⇒ next 提前声明（P244 声明/赋值分离）。
    std::time_t next;
    // ORIG 0x08635944 `movb $0x0,-0xd` 位于 target 拷贝**之前**（声明序证据）。
    bool found = false;
    std::tm target = cur;
    target.tm_min = 0;
    target.tm_sec = 0;
    for (int i = 3; i >= 0; --i)
    {
        // ORIG 0x086359a1 先载 hour 到 %edx 再载表项到 %eax（`cmp %eax,%edx; jl`）
        // ⇒ 源形 `cur.tm_hour >= m_nTimeTable[i]`（交换操作数翻装载序）。
        if (cur.tm_hour >= m_nTimeTable[i])
        {
            target.tm_hour = m_nTimeTable[i + 1];
            found = true;
            break;
        }
    }
    next = mktime(&target);
    // [b28d-solo-GameMain] 比较方向旋钮：ORIG `cmp -0x14(%ebp),%eax`（dst=%eax）
    // vs 4.4 `cmp %eax,-0x14(%ebp)`（N18b）；试交换操作数。
    if (!found || now > next)
    {
        target.tm_hour = m_nTimeTable[0];
        target.tm_mday += 1;
        next = mktime(&target);
    }
    // ORIG 0x08635a24：`add %eax,-0x14(%ebp); mov -0x14(%ebp),%eax` —— 两语句形
    // （+= 后再 return 具名局部），单表达式 `next + call()` 会被 4.4 折成 `add mem,%eax`。
    next += get_rand_int(300);
    return static_cast<int>(next);
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
// [b28d-28] DIFF | ORIG 0x08639b20 | P167 MISSING 14×`_Rb_tree_iterator::ptEv` +
// G_CDataManager：ORIG 每轮迭代重新解引用（我方缓存），语义缺口待补。
// [b28d-solo-GameMain] N95/N82 封顶 | 211/213 | p167 29/29 CLEAR | 修法：逐字段
// `it->second.m_xxx`（14× operator->）+ localtime_r 返回指针（P43）+ begin/end
// 各自独立调 G_CDataManager（P183）+ key 具名槽；残差帧/槽平移（N95）。
void Timer_DungeonInoutOpenTime::RegistNextTimer()
{
    std::time_t now = static_cast<std::time_t>(s_systemTime_.getCurSec());
    std::tm t;
    std::tm* p = localtime_r(&now, &t);   // P43：ORIG 经返回值指针读字段
    const int curHour = p->tm_hour;
    const int curMin = p->tm_min;
    // P183：ORIG 无 table 引用缓存——begin/end 各自独立调 G_CDataManager()
    // （p167 差集 MISSING 1×G_CDataManager 即此）。
    std::map<int, STDungeonInoutTime>::iterator it =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.begin();
    std::map<int, STDungeonInoutTime>::iterator e =
        G_CDataManager()->m_serverParameter.m_dungeonInoutc8.end();
    for (; it != e; ++it)
    {
        int key = it->first;   // ORIG -0x10 槽（循环头一次性读取）
        std::time_t next;
        // P157/P183：ORIG 每个字段访问都重新 `call operator->`（14×，p167 差集
        // MISSING 14×ptEv）——禁 `const T& win = it->second` 引用缓存。
        if (curHour >= it->second.m_startHour &&
            curHour <= it->second.m_endHour &&
            curMin >= it->second.m_startMin &&
            curMin <= it->second.m_endMin)
        {
            p->tm_hour = 0;
            p->tm_min = 0;
            p->tm_sec = 0;
            next = mktime(p);
        }
        else
        {
            const bool beforeStart =
                it->second.m_startHour >= curHour ||
                (it->second.m_endHour >= curHour &&
                 (it->second.m_startMin > curMin ||
                  (it->second.m_startMin <= curMin && it->second.m_endMin < curMin)));
            if (beforeStart)
            {
                // ORIG 0x08639cc5/0x08639cf3：`cmp curHour; setg %al; test; je`
                // —— 条件写作 startHour > curHour（P225 物化方向）。
                if (it->second.m_startHour > curHour)
                    p->tm_hour = it->second.m_startHour;
                else if (it->second.m_startMin > curMin)
                    p->tm_hour = curHour;
                else
                    p->tm_hour = curHour + 1;
                p->tm_min = it->second.m_startMin;
                p->tm_sec = 0;
                next = mktime(p);
            }
            else
            {
                p->tm_hour = it->second.m_startHour;
                p->tm_min = it->second.m_startMin;
                p->tm_sec = 0;
                next = mktime(p) + 0x15180;  // +86400：明天同一开放点
            }
        }
        int delay = static_cast<int>(next) - static_cast<int>(now);
        if (delay < 0)
        {
            delay = 0;
        }
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                    (TIMER_MESSAGE)0x93, delay, 0,
                                    (unsigned int)key);
    }
}

// ---------------------------------------------------------------------------
// RegistServerSchedule() —— ORIG 0x82a0459（0x49b 字节）逐条转写。
// 全部为 G_TimerQueue()->InsertTimer(OBJ_TYPE=2, 0, msg, delaySec, 0, 0)。
// ---------------------------------------------------------------------------
// [b28d-28] DIFF | ORIG 0x082a0459 | N82：槽/寄存器分配差（大函数）。
// [b28d-solo-GameMain] N95/N82 封顶 | 235/242 | 真语义 bug 修：ORIG magic
// 0x88888889+`sar $0x4` = **÷30** ⇒ 源形 `p->tm_min % 30`（半点对齐；旧实现
// 误用 tm_mon/15）；0x55 delay 改内联实参（P24）；残差帧 0x74 vs 0x64 + 槽平移。
void RegistServerSchedule()
{
    // 0x50 / 0x52：固定 0 秒
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x50, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x52, 0, 0, 0);

    // 0x54：对齐到整半点（ORIG：1740 - 60*(tm_mon - 30*(tm_mon/15)) + (60 - tm_sec)）
    {
        std::time_t now = std::time(0);
        std::tm t;
        // P43：ORIG 0x082a04fb `mov %eax,-0x18; mov 0x4(%eax),%ecx` 经返回值指针读字段。
        std::tm* p = localtime_r(&now, &t);
        // ORIG 0x082a0504 magic 0x88888889 + `sar $0x4` = **÷30**（sar 3 才是 ÷15），
        // 随后 `v = tm_min - 30*q` ⇒ 源形 `tm_min % 30`（半点对齐；旧实现误用
        // tm_mon/15，属真语义 bug）。v 无槽（寄存器直算），故内联进 delay 表达式。
        int delay = 1740 - 60 * (p->tm_min % 30) + (60 - p->tm_sec);
        G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0,
                                    (TIMER_MESSAGE)0x54, delay, 0, 0);
    }

    // 0x55：TimerStatisticLevel 下一统计点距当前秒数（ORIG 0x082a0588：差在 %ebx
    // 跨 G_TimerQueue() 调用保活，无具名槽 ⇒ P24 内联实参）。
    G_TimerQueue()->InsertTimer(
        (TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x55,
        TimerStatisticLevel::CalcNextTime() - static_cast<int>(std::time(0)), 0, 0);

    // 0x59 / 0x5a / 0x5b / 0x61 / 0x66 / 0x89：ORIG 逐个独立 G_TimerQueue()->InsertTimer
    // （非循环；循环只发 1 个 call 点，P167 缺 5）。
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x59, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x5a, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x5b, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x61, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x66, 0, 0, 0);
    G_TimerQueue()->InsertTimer((TimerEntry::OBJ_TYPE)2, 0, (TIMER_MESSAGE)0x89, 0, 0, 0);

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
// [b28d-28] DIFF | ORIG 0x0829f966 | N82/P183：ORIG CommonTime 五字节单条 disp
// （`movzbl -0x18..-0x14`），我方经 m_pad 数组索引产 lea+add（字段化待 Main 批）；
// 帧 0x30 vs 0x40。
// [b28d-solo-GameMain] NEAR | 47=47 | 修法：CUserCharacInfo.h CommonTime 匿名 union
// 具名视角（m_year..m_min）复现 5×`movzbl disp`；残差 N95（4.4 槽基 ebp-8 vs
// 4.1 ebp-4：CommonTime -0x1c vs -0x18）+ N115 帧 0x40 vs 0x30。
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
    // [b28c-lane24] 不缓存 `const char* p`（P183 地址重物化铁律）：具名指针使
    // 4.4 生 `lea -0x18; mov %eax,-0xc; mov -0xc,%eax; add $K` 链，ORIG 直接
    // `movzbl -0x18(%ebp),%eax` 栈相对 disp ⇒ 每次读取内联全表达式。
    // 具名标量视角（CUserCharacInfo.h CommonTime 匿名 union）：ORIG 单条
    // `movzbl -0x18..-0x14(%ebp),%eax` disp 读，禁数组成员索引（lea+add）。
    std::printf("[!] Service Date (%02d-%02d-%02d/%02d:%02d)\n",
                (int)time.m_year, (int)time.m_month, (int)time.m_day,
                (int)time.m_hour, (int)time.m_min);
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
// [b28d-28] DIFF | ORIG 0x082a08f4 | P167 MISSING 2×G_CEnvironment/2×c_str/
// ostream/sprintf/cMyTrace（初始化尾部日志路径缺口）+ N82 大函数槽。
// [b28d-solo-GameMain] N82 封顶（**语义已完整**）| p167 **CLEAR 269/269**
// （0 missing / 0 extra，§6.2 三件套之②达成；残差仅 N82 大函数槽/帧）。修法：
// ①`sprintf` 照抄（P171）+ garenaPubkey 去缓存（P183）+ tournament db Fail 分支补
//   `cout << ... << endl`；
// ②**真语义 bug ×5**：a) guild 装载循环 -0x2c0 独立计数器 + LoadGuildInfoProxyFromDB
//   实参 = (ENUM_SERVER_GROUP)gi（旧形 gi 自增 + 传 E_SERVER_GROUP_NONE）；
//   b) Power War 段 ORIG 为**单次调用**（0x82a1f9f..fb7，旧形写成 guild 遍历循环）；
//   c) `G_CDataManager()->init()` 返回值被丢弃（ORIG 0x82a0e7b 检查失败 → 行号
//   0x29f3 "[!] Init DataManager Fail" + return false）；
//   d) initTournamentDB 实参 ORIG = `G_CEnvironment()+0x98`（m_dbTblFile，Init 为
//   friend 直访），旧形 get_file_name()（= m_serverEnvir.m_fileName）取错；
//   e) statistic 循环 InitLoginState 实参 ORIG = `(idx, get_gc_no_hardcode(idx))`，
//   旧形 `get_gc_no_hardcode(E_SERVER_GROUP_NONE) + get_channel_no` 取错；
//   ③两个遍历循环均改独立计数器 + 链式单表达式（P52/P183 begin/end 各自重物化）。
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

    // 日志初始化（ORIG 82a0a19..82a0ae5）：
    //   CLog::createInstance → cout "[!] Init Log ..." → sprintf
    //   "./log/%s/"（0x8c187b8）→ initiate(path, 0, 0)（第 2 参 ORIG 传 0，
    //   m_logFile 前缀回落 "Log"）→ cout "	- log path: " << path
    //   → cout "[!] Init Log Success"。
    CLog::createInstance();
    std::cout << "[!] Init Log ..." << std::endl;
    char logPath[0x104];
    // ORIG 0x082a0a64 调 `sprintf@plt`（无 size 参数，P171 照抄；旧实现 snprintf
    // 多压 size 实参，p167 EXTRA +1）。
    std::sprintf(logPath, "./log/%s/", G_CEnvironment()->get_file_name());
    CLog::instance()->initiate(logPath, 0, 0);
    std::cout << "	- log path: " << logPath << std::endl;
    std::cout << "[!] Init Log Success" << std::endl;

    // ORIG 82a0aea..82a0bc0：
    //   cMyTrace trace("AuthGarenaToken::Init"@0x8c1b026, 0x29c9, 9, 1, 0);
    //   trace("[!] Init Taiwan Garena Auth Key ..."@0x8c187e8);
    //   init(env+0x314 = m_serverEnvir.m_garenaPublickey.c_str())；
    //   init 返回 1（成功）→ xor 翻转 0 → je 82a0bc5（Success 分支：trace
    //   0x29d3 " %s Success" → 继续 GlobalData::Init）；init 返回 0（失败）
    //   → 82a0b57 起 Fail 分支：trace 0x29d0 " %s Fail" → jmp 82a2105
    //   （Init 返回 0，main 退出）。影子实证：publickey.pem Fail 两侧都退出，
    //   第二轮均 pid 残留 already operated。
    cMyTrace authTrace("AuthGarenaToken::Init", 0x29c9, 9, 1, 0);
    authTrace("[!] Init Taiwan Garena Auth Key ...");
    // P183：ORIG 0x082a0b36/0x082a0b57/0x082a0bc5 三处各自重算
    // `G_CEnvironment()+0x314 → c_str()`，无 garenaPubkey 具名缓存
    // （p167 MISSING 2×G_CEnvironment + 2×c_str 即此）。
    if (!Taiwan::AuthGarenaToken::init(
            G_CEnvironment()->m_serverEnvir.m_garenaPublickey.c_str()))
    {
        // ORIG 82a0b57..bb6：Fail trace 构造 (name, 0x29d0, 9, 0, 1)
        // ——bFieldC=0、bFieldD=1（+0xc=1 → bAppend/bTimestamp=1 → 尾部 CRLF）。
        cMyTrace failTrace("AuthGarenaToken::Init", 0x29d0, 9, 0, 1);
        failTrace(" %s Fail",
                  G_CEnvironment()->m_serverEnvir.m_garenaPublickey.c_str());
        return false;
    }
    cMyTrace okTrace("AuthGarenaToken::Init", 0x29d3, 9, 0, 1);
    okTrace(" %s Success",
            G_CEnvironment()->m_serverEnvir.m_garenaPublickey.c_str());
    CodePage::initCodePage();

    // ORIG 82a0bc5..：各启动阶段 cMyTrace(5-arg) 记录 + 失败即 abort（返回 0）。
    // 此前重建忽略多数返回值（带病启动），现按 ORIG 逐阶段补失败处理与 trace 日志。
    { cMyTrace t(__PRETTY_FUNCTION__, 0x29dd, 9, true, true); t("[!] Init GlobalData ..."); }
    if (!GlobalData::Init())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x29e0, 9, true, true); t("[!] Init GlobalData Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x29e3, 9, true, true); t("[!] Init GlobalData Success"); }

    { cMyTrace t(__PRETTY_FUNCTION__, 0x29e5, 9, true, false); t("[!] Init HistoryLog"); }
    if (!HistoryLog::Init())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x29e8, 9, false, true); t("Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x29eb, 9, false, true); t(" Success"); }

    { cMyTrace t(__PRETTY_FUNCTION__, 0x29f0, 9, true, true); t("[!] Init DataManager ..."); }
    // [b28d-solo-GameMain] 真语义 bug 修：ORIG 0x82a0e7b..0x82a0ed3 检查 init 返回值
    // （`xor $1,%eax; test %al; jmp 成功`），失败分支 ctor 行号 0x29f3 + 串
    // "[!] Init DataManager Fail" 后返回 false；旧形丢弃返回值直接往下走。
    if (!G_CDataManager()->init())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x29f3, 9, true, true);
          t("[!] Init DataManager Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x29f6, 9, true, true); t("[!] Init DataManager Success"); }

    { cMyTrace t(__PRETTY_FUNCTION__, 0x29f9, 9, true, true); t("[!] Init GameManager ..."); }
    if (!G_CGameManager()->init())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x29fc, 9, true, true); t("[!] Init GameManager Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x29ff, 9, true, true); t("[!] Init GameManager Success"); }

    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a02, 9, true, true);
      t("[!] Init Arad GameScriptManager ..."); }
    if (!ARAD::Singleton<ARAD::ScriptManager>::Get()->LoadScript())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x2a05, 9, true, true);
          t("[!] Init Arad GameScriptManager Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a08, 9, true, true);
      t("[!] Init Arad GameScriptManager Success"); }

    G_CGameManager()->GetQuickPartySystemManager();

    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a1f, 9, true, false); t("[!] Init ChannelType "); }
    if (!G_GameWorld()->InitChannelType())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x2a22, 9, true, true); t("[!] Init ChannelType Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a25, 9, false, true); t("Success"); }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a27, 9, true, true);
      t("\t- Channel Type : %d", G_GameWorld()->GetChannelType()); }

    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a29, 9, true, false); t("[!] Init tournament db "); }
    // [b28d-solo-GameMain] 真语义 bug 修：ORIG 0x82a12e2 `call G_CEnvironment;
    // add $0x98,%eax` ⇒ 实参是 **CEnvironment+0x98 = m_dbTblFile**（Init 为该类
    // friend，直访 private 成员）；旧形 get_file_name() 返回 m_serverEnvir.m_fileName
    // （+0x198 区），实参取错（p167 EXTRA get_file_name +1 / MISSING G_CEnvironment -1）。
    if (!GlobalData::initTournamentDB(G_CEnvironment()->m_dbTblFile))
    {
        // ORIG 0x082a12fb：失败分支是 cout << "[!] Init tournament db Fail" << endl
        // （p167 MISSING 2×ostream 即此），非 cMyTrace。
        std::cout << "[!] Init tournament db Fail" << std::endl;
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2a33, 9, false, true); t("Success"); }

    // DB 查询计数器挂接（DBMgr@全局 s_db_mgr）
    GlobalData::s_db_mgr->SetQueryCounterPointer(
        GlobalData::s_db_mgr->GetDBHandle(DB_HANDLE_0, E_SERVER_GROUP_NONE /*推断*/)
            ->GetQueryCounter());

    // ---- 三类 ServerProxy 创建（ORIG 82a13ac..1855 结构）----
    // ORIG：GetChannelType()==7 跳过全部；IsIntegratedPvPBaseChannel() 时
    // 遍历 m_serverGroupMap（config 键 *ip_of_* / *port_of_*）每组建 proxy；
    // 否则固定 getter 单组。
    if (G_GameWorld()->GetChannelType() != 7)
    {
        if (G_GameWorld()->IsIntegratedPvPBaseChannel())
        {
            typedef std::map<ENUM_SERVER_GROUP,
                             std::map<std::string, std::string> > GrpMap;
            // ORIG 82a15xx：单循环遍历 m_serverGroupMap，每迭代依次建 monitor/guild/
            // statistic proxy；每 createServerProxy 前重取 G_CEnvironment()->…size()
            // 作 count 参，失败 cerr + abort。idx（-0x34）随迭代自增。
            // P183：begin/end 各自重物化 `G_CEnvironment()->m_serverGroupMap`
            // （ORIG `++it` 后紧跟 `call G_CEnvironment; add $0x50; call end()`；
            // 缓存 GrpMap& 引用会缺该调用点，p167 MISSING G_CEnvironment -1）。
            int idx = 0;
            for (GrpMap::iterator it = G_CEnvironment()->m_serverGroupMap.begin();
                 it != G_CEnvironment()->m_serverGroupMap.end(); ++it, ++idx)
            {
                ENUM_SERVER_GROUP g = it->first;
                std::map<std::string, std::string>& c = it->second;

                int mTcp = atoi(GetConfigFileString(c, "tcp_port_of_monitor"));
                int mUdp = atoi(GetConfigFileString(c, "udp_port_of_monitor"));
                char* mIp = const_cast<char*>(GetConfigFileString(c, "udp_ip_of_monitor"));
                if (!GlobalData::s_monitor_proxy_mgr->createServerProxy(
                        static_cast<int>(G_CEnvironment()->m_serverGroupMap.size()),
                        idx, g, mIp, mUdp, mTcp))
                {
                    std::cerr << "[s_monitor_proxy_mgr::Init] Fail Init (monitor server proxy)"
                              << std::endl;
                    return false;
                }

                int gTcp = atoi(GetConfigFileString(c, "tcp_port_of_guild"));
                int gUdp = atoi(GetConfigFileString(c, "udp_port_of_guild"));
                char* gIp = const_cast<char*>(GetConfigFileString(c, "udp_ip_of_guild"));
                if (!GlobalData::s_guild_proxy_mgr->createServerProxy(
                        static_cast<int>(G_CEnvironment()->m_serverGroupMap.size()),
                        idx, g, gIp, gUdp, gTcp))
                {
                    std::cerr << "[s_guild_proxy_mgr::Init] Fail Init (guild server proxy)"
                              << std::endl;
                    return false;
                }

                int sUdp = atoi(GetConfigFileString(c, "udp_port_of_statistic"));
                char* sIp = const_cast<char*>(GetConfigFileString(c, "udp_ip_of_statistic"));
                if (!GlobalData::s_statistic_proxy_mgr->createServerProxy(
                        static_cast<int>(G_CEnvironment()->m_serverGroupMap.size()),
                        idx, g, sIp, sUdp, 0))
                {
                    std::cerr << "[s_statistic_proxy_mgr::Init] Fail Init (statistic server proxy)"
                              << std::endl;
                    return false;
                }
            }
        }
        else
        {
            // 固定 getter 单组（ORIG 82a16aa 非 Integrated 分支）
            // ORIG 每 createServerProxy 检查返回值，失败 cerr + abort。
            if (!GlobalData::s_monitor_proxy_mgr->createServerProxy(
                    1, 0, E_SERVER_GROUP_NONE,
                    G_CEnvironment()->get_udp_ip_monitor(),
                    G_CEnvironment()->get_tcp_port_monitor(),
                    G_CEnvironment()->get_udp_port_monitor()))
            {
                std::cerr << "[s_monitor_proxy_mgr::Init] Fail Init (monitor server proxy)"
                          << std::endl;
                return false;
            }
            if (!GlobalData::s_guild_proxy_mgr->createServerProxy(
                    1, 0, E_SERVER_GROUP_NONE,
                    G_CEnvironment()->get_udp_ip_guild(),
                    G_CEnvironment()->get_tcp_port_guild(),
                    G_CEnvironment()->get_udp_port_guild()))
            {
                std::cerr << "[s_guild_proxy_mgr::Init] Fail Init (guild server proxy)"
                          << std::endl;
                return false;
            }
            if (!GlobalData::s_statistic_proxy_mgr->createServerProxy(
                    1, 0, E_SERVER_GROUP_NONE,
                    G_CEnvironment()->get_udp_ip_statistic(), 0,
                    G_CEnvironment()->get_udp_port_statistic()))
            {
                std::cerr << "[s_statistic_proxy_mgr::Init] Fail Init (statistic server proxy)"
                          << std::endl;
                return false;
            }
        }
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2ac8, 9, true, false); t("[!] Init GameWorld "); }
    if (!G_GameWorld()->init())
    {
        { cMyTrace t(__PRETTY_FUNCTION__, 0x2acb, 9, true, true); t("[!] Init GameWorld Fail"); }
        return false;
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2ace, 9, false, true); t("Success"); }

    GlobalData::s_DailyScheduleManager->CheckAndSendToAllUserSchedule();

    // LoginState 初始化：遍历 statistic proxy（Start..End 索引区间）。
    // ORIG 0x2ad7/0x2aec：前后 cMyTrace；InitLoginState 失败(0x2ae8/0x2adf) abort。
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2ad7, 9, true, false); t("[!] Init Login State "); }
    // [b28d-solo-GameMain] P222 块序：ORIG 真分支（Integrated → statistic 遍历循环）
    if (G_GameWorld()->IsIntegratedPvPBaseChannel())
    {
        // 同 guild 循环：ORIG -0x?? 独立计数器（GetNextIndex(int&) 引用出参，初值 0，
        // cond `m_count > next`）；旧形 idx 自身自增（跳过条目）。
        int statNext = 0;
        for (int idx = GlobalData::s_statistic_proxy_mgr->GetStartIndex();
             GlobalData::s_statistic_proxy_mgr->GetEndIndex() > statNext; )
        {
            // [b28d-solo-GameMain] 真语义 bug 修：ORIG 0x82a19e4..0x82a1a0d 实参为
            // `InitLoginState((ENUM_SERVER_GROUP)idx, get_gc_no_hardcode(idx))`
            // （esi=idx 作第一参数、ebx=get_gc_no_hardcode(idx) 作第二参数）；
            // 旧形取 get_gc_no_hardcode(E_SERVER_GROUP_NONE) + get_channel_no（错）。
            if (!G_CDataManager()->InitLoginState(
                    static_cast<ENUM_SERVER_GROUP>(idx),
                    G_CEnvironment()->get_gc_no_hardcode(
                        static_cast<ENUM_SERVER_GROUP>(idx))))
            {
                { cMyTrace t(__PRETTY_FUNCTION__, 0x2adf, 9, true, true);
                  t("[!] Init Login State Fail"); }
                return false;
            }
            idx = GlobalData::s_statistic_proxy_mgr->GetNextIndex(statNext);
        }
    }
    else
    {
        // ORIG 非 Integrated：单发 InitLoginState(m_serverGroup, m_gcNo)。
        if (!G_CDataManager()->InitLoginState(
                static_cast<ENUM_SERVER_GROUP>(
                    G_CEnvironment()->m_serverEnvir.m_serverGroup),
                G_CEnvironment()->m_serverEnvir.m_gcNo))
        {
            { cMyTrace t(__PRETTY_FUNCTION__, 0x2ae8, 9, true, true);
              t("[!] Init Login State Fail"); }
            return false;
        }
    }
    { cMyTrace t(__PRETTY_FUNCTION__, 0x2aec, 9, false, true); t("Success"); }

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
        // ORIG 82a1bce..：Verify/Insert/LoadEventFromDB 任一失败记 cMyTrace 并 abort。
        if (!CEventTable::VerifyEventTable(eventIds))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2b0e, 9, true, true);
            t("Fail verify Event Table");
            return false;
        }
        if (!CEventTable::InsertEventTable(eventIds))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2b14, 9, true, true);
            t("Fail insert Event Table");
            return false;
        }
        for (int i = 0; i < CEventTable::GetEventTableCount(); ++i)
        {
            CEventBase* base = CEventTable::GetEventBase(i);
            int eventId = CEventTable::GetEventId(i);
            GlobalData::s_event_manager->AddEvent(eventId, base);
        }
        if (!GlobalData::s_event_manager->LoadEventFromDB())
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2b25, 9, true, true);
            t("Fail Load Event From DB");
            return false;
        }
        // 触发周期事件（ORIG 82a1d56..dd8：getCurSec → GetRepeatEvent(0x3a)
        // → ev->IsEventing(0) 且 curSec>0x4cbe3f0f(约 2017 时间戳) 则
        // TriggerEventStart(0x3a, Word_Param())；单事件非小时槽循环）。
        long curSec = static_cast<long>(s_systemTime_.getCurSec());
        CEventBase* ev = GlobalData::s_event_manager->GetRepeatEvent(0x3a);
        if (ev && ev->IsEventing(0) && curSec > 0x4cbe3f0f)
            GlobalData::s_event_manager->TriggerEventStart(0x3a, Word_Param());
    }
    // 公会信息装载。[b28d-solo-GameMain] 真语义 bug 修 ×2（ORIG 0x82a1dd8..0x82a1ea6）：
    // ①-0x2c0 是**独立计数器**（GetNextIndex(int&) 引用出参，初值 0，cond `m_count
    //   > next`），旧形用 gi 自身自增（跳过条目）；
    // ②LoadGuildInfoProxyFromDB 实参 = **当前索引 gi**（ORIG `mov -0x20,%ebx` 直传），
    //   旧形传 E_SERVER_GROUP_NONE（取错实参）；
    // ③ORIG 无 gp 判空（P171 照抄），链式单表达式（P52）。
    int guildNext = 0;
    for (int gi = GlobalData::s_guild_proxy_mgr->GetStartIndex();
         GlobalData::s_guild_proxy_mgr->GetEndIndex() > guildNext; )
    {
        if (!GlobalData::s_guild_proxy_mgr->GetServerProxy((ENUM_SERVER_GROUP)gi)
                 ->LoadGuildInfoProxyFromDB((ENUM_SERVER_GROUP)gi))
        {
            cMyTrace t(__PRETTY_FUNCTION__, 0x2b88, 9, true, true);
            t("Fail Load Guild Info Proxy");
            return false;
        }
        gi = GlobalData::s_guild_proxy_mgr->GetNextIndex(guildNext);
    }

    // 广告 / 共享服务器消息 / 拍卖均价
    // ORIG 0x2b91：LoadADExposeInfoAtApp 失败记 cMyTrace 并 abort。
    if (!InGameADManagerInstance()->LoadADExposeInfoAtApp())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x2b91, 9, true, true);
        t("Fail Load In Game Advertisement Expose Info");
        return false;
    }
    G_CGameManager()->GetSharedServerMessageManager()->LoadServerMessageFromDB();
    G_CGameManager()->GetAuctionAveragePriceManager()->LoadAveragePrice();

    np_server_xml::CServerXml xml;  // 「推断」实例化方式（ORIG 直接调用成员）
    xml.StrLoading();

    // 死亡之塔排行榜 4 表（type 1..4）懒构造 + 逐表 init（ORIG 82a1f.. 循环）；
    // registNextTimer 传 0（ORIG 实证，非 60000）。
    for (int i = 1; i < 5; ++i)
    {
        // ORIG 0x082a1f60..0x082a1f75：getRankTable(i) 返回表指针作 this 传参，
        // 再 init(i)（非静态成员，esp[0]=this、esp[4]=i）。
        WongWork::CDeathTowerRanking* rankTable =
            (WongWork::CDeathTowerRanking*)WongWork::CDeathTowerRanking::getRankTable(i);
        rankTable->init(i);
    }
    TimerDeathTowerTopRankerReload::registNextTimer(0);

    InitCipherKeyString();
    RegistServerSchedule();

    // Power War 启动信息请求 + 各定时器注册（ORIG 顺序保留）。
    // [b28d-solo-GameMain] 真语义 bug 修：ORIG 0x82a1f9f..0x82a1fb7 是**单次调用**
    // （`mov 0x940be2c,%eax; movl $0,0x4(%esp); call GetServerProxy; mov %eax,(%esp);
    // call SendReqestPowerWarStartInfo`，无循环、无判空）；旧形写成 guild 遍历循环
    // （多发 GetStartIndex/GetEndIndex/GetNextIndex 各 1，p167 EXTRA 三连即此）。
    GlobalData::s_guild_proxy_mgr->GetServerProxy(E_SERVER_GROUP_NONE)
        ->SendReqestPowerWarStartInfo();
    Timer_SecretShopStatistic::RegistNextTimer();
    Timer_DailySchedule::RegistNextTimer();
    advancealtar::Timer_MainTick::RegistNextTimerGage();
    Timer_OneADayItemShop::RegistNextTimer();
    Timer_EventManagement::RegistNextTimer(60000);           // 「推断」间隔
    Timer_DungeonStatisticEntrance_Hour::RegistNextTimer();

    // ORIG 0x2bf6：create_thread 失败记 "Fail to Create Threads." 并 abort。
    if (!create_thread())
    {
        cMyTrace t(__PRETTY_FUNCTION__, 0x2bf6, 9, true, true);
        t("Fail to Create Threads.");
        return false;
    }
    // ORIG 82a204e：仅 master 连接器 s_db_connector 非空才 Close()。
    if (GlobalData::s_db_connector != 0)
        GlobalData::s_db_connector->Close();
    // ORIG 0x2c0e：启动成功 trace（file_name / channel_no / channel_name 序）。
    cMyTrace t(__PRETTY_FUNCTION__, 0x2c0e, 9, true, true);
    t("[!] The %s channel %d('%s') server has been started successfully.",
      G_CEnvironment()->get_file_name(), G_CEnvironment()->get_channel_no(),
      G_CEnvironment()->get_channel_name());
    return true;
}

// ---------------------------------------------------------------------------
// MainLoop() —— ORIG 0x829fa2b（0x305）：主循环每秒一轮。ORIG 局部为
// 周期/计数器组：-0x34 每 30 轮 UpdateChannelUser+状态日志；-0x40 每 10 轮
// 心跳（monitor/guild proxy 遍历，GetServerProxy(组号)）；-0x24 每 2 轮
// 翻转 CmmChannelProxy 上报；-0x1c 每 0x12c 轮向 DISPATCH_Q 投递心跳流包
// （Acquire("App.cpp", 0x2728) + << 0x30a << -1）。退出条件
// !GlobalData::s_server_stop（@0x940bd64），返回 1。
// ---------------------------------------------------------------------------
// SIGFIX:MainLoop —— ORIG __PRETTY_FUNCTION__ 串 "bool MainLoop()"；ORIG 尾
// 0x829fd24 `mov $0x1,%eax` 后 leave/ret（循环退出返回 1，P174）。
// [b28d-28] DIFF | ORIG 0x0829fa2b | P167 CLEAR（36/36）；N82：常量数组初始化槽
// （-0x48..-0x38）+ 帧 0x60 vs 0x50 + 槽平移。
// [b28d-solo-GameMain] AE | 190=190 | 真语义 bug 修 ×2：
// ①ORIG 0x0829fb02 `cmpl $0x9; setg; addl $1,-0x40; test; je` ⇒ 源形
//   `if (cnt10++ > 9) {…; cnt10=0;}`（旧形真分支后值 1≠ORIG 0）；
// ②心跳循环 ORIG -0x4c 是**独立计数器**（GetNextIndex(int&) 引用出参，初值 0，
//   cond `m_count > next`）；旧 `mi = GetNextIndex(mi)` 把 proxy 索引当计数器
//   （首轮跳 indexArray[1]）⇒ 漏发心跳。
// 修法：P52 链式单表达式 `mgr->GetServerProxy(mi)->SendHeartBeat(G_CEnvironment()
// ->get_channel_no())`（实参先物化 %ebx 跨 receiver 保活）+ 两循环复用同一 next
// 槽（guild 前复位 0，ORIG 同 -0x4c）+ P259 `Acquire(...)` 内联进 CStreamGuard
// ctor 实参（去 s 具名槽往返）+ 6 个 dead 局部复现帧头 12 条初始化。
// 帧 0x60 与 ORIG 全等，指令数 190=190。
bool MainLoop()
{
    // ORIG 0x0829fa33..0x0829fa87：函数头 12 个具名局部初始化，槽深 -0x48..-0x1c。
    // 其中 6 个为只写不读的 dead 局部（lim30/lim10/dead3/lim20/dead8/lim300）——
    // 它们撑起 ORIG 帧 0x60（我方旧版 0x50）并决定后续槽号；比较处 ORIG 仍用立即数。
    int lim30 = 30;         // -0x48（dead）
    int lim10 = 10;         // -0x44（dead）
    int cnt10 = 0;          // -0x40
    int dead3 = 0;          // -0x3c（dead）
    int lim20 = 20;         // -0x38（dead）
    int cnt30 = 0;          // -0x34
    int lastUserCount = 0;  // -0x30
    int lastRemain = 0;     // -0x2c
    int dead8 = 0;          // -0x28（dead）
    int flip24 = 0;         // -0x24
    int lim300 = 300;       // -0x20（dead）
    int cnt300 = 0;         // -0x1c
    (void)lim30; (void)lim10; (void)dead3; (void)lim20; (void)dead8; (void)lim300;

    while (!GlobalData::s_server_stop)
    {
        if (cnt30 == 30)
        {
            G_GameWorld()->UpdateChannelUser();
            int curUser = G_GameWorld()->get_UserCount_InWorld();
            int remain =
                G_CGameManager()->m_userPool.Size() - G_CGameManager()->m_userPool.Remain();
            if (lastUserCount != curUser || remain != lastRemain)
            {
                ProcessServerStatusLog();
                lastUserCount = curUser;
                lastRemain = remain;
            }
            cnt30 = 0;
        }

        // ORIG 0x0829fb02 `cmpl $0x9,-0x40; setg %al; addl $0x1,-0x40; test %al,%al;
        // je`——比较用旧值、自增在测前、真分支内重置 0 ⇒ 源形 `if (cnt10++ > 9)`。
        // 旧实现 `if(cnt10>9){…;cnt10=0;} ++cnt10;` 真分支后值为 1 ≠ ORIG 0（真语义 bug）。
        if (cnt10++ > 9)
        {
            // [b28d-solo-GameMain] 真语义 bug 修：ORIG -0x4c 是**独立计数器**
            // （GetNextIndex(int&) 的引用出参，初值 0，cond 用 `m_count > next`）；
            // 旧实现 `mi = GetNextIndex(mi)` 把 proxy 索引当计数器自增（首轮即
            // 跳过 indexArray[1]，且 cond 用索引值比较）⇒ 漏发心跳。
            // 链式单表达式（P52）：get_channel_no 实参先物化 %ebx 跨 GetServerProxy
            // 保活（旧形经具名 mp 局部槽化 + 调用序反）。
            int next = 0;
            for (int mi = GlobalData::s_monitor_proxy_mgr->GetStartIndex();
                 GlobalData::s_monitor_proxy_mgr->GetEndIndex() > next; )
            {
                GlobalData::s_monitor_proxy_mgr->GetServerProxy(
                    (ENUM_SERVER_GROUP)mi)->SendHeartBeat(
                        G_CEnvironment()->get_channel_no());
                mi = GlobalData::s_monitor_proxy_mgr->GetNextIndex(next);
            }
            // ORIG 两循环复用同一 -0x4c 槽（guild 前重新置 0）⇒ 用同一局部复位。
            next = 0;
            for (int gi = GlobalData::s_guild_proxy_mgr->GetStartIndex();
                 GlobalData::s_guild_proxy_mgr->GetEndIndex() > next; )
            {
                GlobalData::s_guild_proxy_mgr->GetServerProxy(
                    (ENUM_SERVER_GROUP)gi)->SendHeartBeat(
                        G_CEnvironment()->get_channel_no());
                gi = GlobalData::s_guild_proxy_mgr->GetNextIndex(next);
            }
            cnt10 = 0;
        }

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
            // [b28d-solo-GameMain] P259：ORIG `call Acquire` 返回值经 %eax 直传
            // CStreamGuard ctor 实参（无 s 具名局部槽往返）；旧形多 store+load 2 条。
            CStreamGuard guard(
                GlobalData::s_small_stream_pool->Acquire("App.cpp", 0x2728), true);
            **guard << 0x30a;
            **guard << -1;
            GlobalData::s_msgq_mgr->put(MsgQueueMgr::DISPATCH_Q, guard);
            cnt300 = 0;
        }
        ++cnt300;

        sleep_ext(1, 0);
        ++cnt30;
        ProcessAntiBotInputWrapper();
    }
    return 1;
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
    // [A18 本波实测] 把 pidFile 声明提前到 cMyTrace 块之前无效（ndiff 18→18、
    // 槽位 0x3f/0x10/0x21 不变）⇒ 4.4 按块/使用序分配聚合槽，声明序不可翻
    // （N93/N82附 族：ORIG [pidFile 0x1e, trace 0x3c, inited 0x4f] vs 我方
    // [trace 0x10, pidFile 0x21, inited 0x3f]）。
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x25e6, 0);
        trace("GAME SERVER SHUTDOWN COMPLETE\n");
    }
    char pidFile[0x1e];
    std::memset(pidFile, 0, sizeof(pidFile));
    std::sprintf(pidFile, "./pid/%s.pid",
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
// P248 档①：ORIG regist_signals 调用点为 `call; xor $0x1,%eax; test %al,%al; je`
// （无 setne）⇒ callee 返回类型必须为 bool（int 在 4.4 恒产 `test; sete` 物化）。
// 返回类型不入 mangled（_Z13regist_signaliPFviE），ORIG 尾 `mov $0x1,%eax` 与 bool 兼容。
// ---------------------------------------------------------------------------
bool regist_signal(int sig, void (*handler)(int))
{
    struct sigaction sa;
    struct sigaction oldact;
    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sig == 14)
        sa.sa_flags |= 0x20000000;
    else
        sa.sa_flags |= 0x10000000;
    if (sigaction(sig, &sa, &oldact) < 0)
    {
        std::printf("%d\xb9\xf8 signal \xb5\xee\xb7\xcf \xbd\xc7\xc6\xd0\n", sig);  // ORIG 文案（CP949 韩文）
        return 0;
    }
    return 1;
}

void rdar_terminate();     // ORIG T 0x0829e80b（本文件尾部定义）
void omll_dump_core_file(); // ORIG T 0x0829f6f4（本文件已有定义）
void TerminateServers();    // ORIG T 0x082a21f9（本文件尾部定义）

// [b28d-28] DIFF | ORIG 0x0829f28b | P167 CLEAR（48/48）；N82 槽/寄存器分配差
// （我方多 28 条）。
// [b28d-solo-GameMain] AE | 121/122 | p167 48/48 CLEAR | 修法：CPacketTracerInstance()/
// G_CEnvironment()/G_GameWorld() 全表达式直用（P0，消 tracer/env/world 具名局部）+
// P227 cMyTrace 全表达式临时链（userCount 跨 ctor %ebx 保活）。
void signal_handler(int sig)
{
    switch (sig)
    {
    case 15:
    {
        std::puts("RECV SIGTERM");
        rdar_terminate();
        // P0：ORIG `call CPacketTracerInstance; mov %eax,(%esp); call ...` 无具名局部。
        CPacketTracerInstance()->AbsoluteTcpWriteLog();
        CPacketTracerInstance()->AbsoluteUdpWriteLog();
        break;
    }
    case 10:
    {
        std::printf("RDAR_SUSPEND pointer: %p\n", G_CEnvironment());
        G_CEnvironment()->set_suspend_state(true);
        break;
    }
    case 12:
    {
        std::puts("RECV SIGUSR2");
        std::printf("G_CEnvironment pointer: %p\n", G_CEnvironment());
        G_CEnvironment()->set_suspend_state(true);
        GetInstanceShutdowManager()->Reset();
        GetInstanceShutdowManager()->EnableShutdown(true);
        // P227 全表达式临时链：ORIG 先 `call get_UserCount_InWorld` 入 %ebx，
        // 再 cMyTrace ctor，ctor 后 `mov %ebx,0x8(%esp)`（无具名 userCount 槽）。
        cMyTrace(__PRETTY_FUNCTION__, 0x2459, 0)(
            "current user count (%d)\n", G_GameWorld()->get_UserCount_InWorld());
        if (G_GameWorld()->get_UserCount_InWorld() < 1)
        {
            GetInstanceShutdowManager()->OnAllUserLogout();
        }
        else
        {
            G_GameWorld()->arrange_users();
        }
        break;
    }
    case 6:
    case 11:
    {
        std::puts("Recv SIGSEGV signal --> make Dump Core file.");
        CPacketTracerInstance()->AbsoluteTcpWriteLog();
        CPacketTracerInstance()->AbsoluteUdpWriteLog();
        omll_dump_core_file();
        break;
    }
    case 8:
    {
        LogManager::logFormat(1, "App.cpp", __PRETTY_FUNCTION__,
                              0x2478, "Recv FPE signal");
        CPacketTracerInstance()->AbsoluteTcpWriteLog();
        CPacketTracerInstance()->AbsoluteUdpWriteLog();
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
        CPacketTracerInstance()->AbsoluteTcpWriteLog();
        CPacketTracerInstance()->AbsoluteUdpWriteLog();
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
    CStreamGuard guard(
        GlobalData::s_stream_pool->Acquire((const char*)"App.cpp", 0x2e2a), true);
    **guard << 0x3e;
    **guard << 0;
    GlobalData::s_msgq_mgr->put((MsgQueueMgr::QUEUE_IDX)1, guard);
}

// omll_dump_core_file() —— ORIG 0x829f6f4（逐指令一致）：abort();
void omll_dump_core_file() { std::abort(); }

// 15/10/12/13/11/8/4/7/16/23/24/25/31 → signal_handler（13 → SIG_IGN），
// 任一注册失败即返回 0，全部成功返回 1。
// [b28d-28] DIFF | ORIG 0x0829f4c5 | P167 CLEAR（13/13）；N82 槽/寄存器分配差。
// [b28d-solo-GameMain] AE | 131=131 | p167 13/13 CLEAR | 修法：bool ok 槽形（前 3 次
// 直测 + 后 10 次 `mov %al,-0x9` 槽，P198① 预清）+ regist_signal 返回类型 int→bool
// （P248 档①：调用点 `xor $1,%eax` 复现；返回类型不入 mangled）。
bool regist_signals()
{
    // ORIG 0x0829f4cb `movb $0x0,-0x9(%ebp)`：bool 局部预清（P198①）。
    // 前三次注册 ORIG 直接 `call; xor $1; test; je`（无槽往返）；自第 4 次
    // （sig 13）起为 `mov %al,-0x9; movzbl -0x9; xor $1; test; je`（具名 ok 槽）。
    bool ok = false;
    if (!regist_signal(15, signal_handler)) { return 0; }
    if (!regist_signal(10, signal_handler)) { return 0; }
    if (!regist_signal(12, signal_handler)) { return 0; }
    ok = regist_signal(13, (void (*)(int))1);  // SIG_IGN
    if (!ok) { return 0; }
    ok = regist_signal(11, signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(8,  signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(4,  signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(7,  signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(16, signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(23, signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(24, signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(25, signal_handler);
    if (!ok) { return 0; }
    ok = regist_signal(31, signal_handler);
    if (!ok) { return 0; }
    return 1;
}

// make_dir(char*) —— ORIG _Z8make_dirPc 0x829ee83：opendir 探测，不存在则 mkdir 0777。
void make_dir(char* path)
{
    // ORIG 0x829ee94：opendir 结果存 -0xc 槽（具名局部），cmpl/jne 直判。
    int ret = (int)opendir(path);
    if (ret == 0)
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
    char path[0x1e];   // ORIG 0x1e（38B）
    std::memset(path, 0, 0x1e);
    std::sprintf(path, "./cfg/%s.cfg",
                 G_CEnvironment()->get_file_name());
    std::printf("\t- cfg path: %s\n", path);
    // ORIG 0x829ea35：test/sete 物化 + mov$1/mov$0 块选择（P56 勿直 return 表达式）。
    if (access(path, F_OK) == 0)
        return true;
    return false;
}

// check_pid_file() —— ORIG 0x829e948（0x82 字节）：pid 文件存在→true。
bool check_pid_file()
{
    char path[0x1e];   // ORIG 0x1e（38B）
    std::memset(path, 0, 0x1e);
    std::sprintf(path, "./pid/%s.pid",
                 G_CEnvironment()->get_file_name());
    std::printf("\t- pid path: %s\n", path);
    if (access(path, F_OK) == 0)
        return true;
    return false;
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
    // ORIG：`movb $0x3,-0x72` 在 len 检查之前（hoist）；len<=0x4f 分支
    // `mov %al,-0x21` → buf[0x51] = (char)len（发送 0x52 字节含尾 len 字节）。
    buf[0] = 0x3;
    if (len > 0x4f)
        return -1;
    buf[0x51] = (char)len;
    strncpy(buf + 1, msg, 0x50);
    int sent = sendto(fd, buf, 0x52, 0,
                      reinterpret_cast<sockaddr*>(&addr), 0x10);
    return sent;
}

// send_suspend_signal() —— ORIG 0x829e6f5（0x116 字节）：
// 读进程 pid 文件（pid/%s.pid，%s = get_file_name()），向 pid 发 SIGUSR1(10)。
void send_suspend_signal()
{
    int pid;
    char buf[0x1e];
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
    int pid;
    char buf[0x1e];
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
// [b28d-28] DIFF | ORIG 0x0829ea5a | N82：槽/寄存器分配差（263=263）。
// [b28d-solo-GameMain] N98 封顶 | 261/263 | p167 58/58 CLEAR | 修法：shellsKey 去
// 具名缓存（P0 表达式直用）+ P232 `char framePad[0x10]` 撑帧（0x424→**0x434 与
// ORIG 全等**）+ P244 op 声明/赋值分离提前（槽 -0x14→-0x18）；残差仅 op 槽
// -0x18 vs ORIG -0x1c（4.4 紧凑分配 vs 4.1 在 -0x18/-0x14 预留两槽，未使用标量
// 4.4 不占槽 ⇒ N98 不可翻；再补 int dead 反使帧 0x444 劣化，已回退）。
bool check_argv(int argc, char** argv)
{
    // P244：op 提前声明（赋值仍在使用点）。
    const char* op;
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
    bool keyOk;   // 声明/赋值分离：goto L_RET0 不得跨初始化（C++ 规则）
    // P0：ORIG 无 shellsKey 具名局部（实参表达式直用 `mov %eax,(%esp)`）。
    keyOk = ncl_check_key(
        reinterpret_cast<char*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x318)) != 0;
    // ORIG printf 再独立 G_CEnvironment()+0x318（P183 重物化，勿缓存 shellsKey）。
    std::printf("lls_key :: %s\n",
                reinterpret_cast<char*>(reinterpret_cast<char*>(G_CEnvironment()) + 0x318));
    // ORIG 0x829eb66-0x829eb6f：keyOk 存栈后 movzbl/xor/test 紧跟无条件
    // jmp 0x829eb87——FAIL 路径（0x829eb71）无跳转引用，系 -O0 死块（P184），
    // 源码保留 if(!keyOk){puts(FAIL);return 0;} 以发射该 puts。
    if (!keyOk)
    {
        std::puts("[!] Server key Check FAIL");
        return 0;
    }
    std::puts("[!] Server key Check OK");

    op = argv[2];
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
        // ORIG 0x0829ed0b：stop/suspend 两分支共享 +0x41b 中继块
        // （`mov $0x0,%eax` + fall-through epilogue）——与其它失败点的就地
        // `mov $0,%eax; jmp` 不同 ⇒ 仅此两点用 goto 到函数尾标签。
        goto L_RET0_TAIL;
    }
    if (std::strcmp(op, "suspend") == 0)
    {
        send_suspend_signal();
        goto L_RET0_TAIL;
    }
    if (std::strcmp(op, "test") == 0)
    {
        G_CEnvironment()->set_operation_type(static_cast<eStartArgumentType_t>(3));
        return 1;
    }
    if (std::strcmp(op, "mess") == 0)
    {
        int total = 0;
        int ret;
        char buf[0x400];
        std::memset(buf, 0, sizeof(buf));
        for (int i = 3; i < argc; ++i)
        {
            std::strcat(buf, argv[i]);
            // ORIG 0x0829ede0：`mov -0x14,%edx; add %edx,%eax; mov %eax,-0x14`
            // 读-加-写三段（`+=` 在 4.4 恒折 `add %eax,-N(%ebp)` 两段）。
            total = static_cast<int>(static_cast<unsigned int>(total) +
                                     static_cast<unsigned int>(std::strlen(argv[i])));
            std::strcat(buf, " ");
            ++total;
        }
        // ORIG 0x0829ee28：`mov %eax,-0x10(%ebp); cmpl $0x0,-0x10(%ebp); jns`
        // ⇒ 具名 int ret 局部（P0 槽往返），非直测返回值。
        ret = send_chat_msg(buf, total);
        if (ret < 0)
            std::puts("[ERROR]Fail to Send Notice Message");
        return 0;
    }
    if (std::strcmp(op, "info") == 0)
    {
        printGameServerInfo(false);
        return 0;
    }
    // ORIG 0x0829ee75：全部 return 0 路径共享函数尾 `mov $0x0,%eax` 块
    // （P325 多 return 共享尾出口）。
    return 0;
L_RET0_TAIL:
    return 0;
}

// ThreadInterface —— 权威定义见 ThreadInterface.h（ORIG 布局 +0 vptr / +4 m_tid /
// +8 m_tidCopy，虚表槽0=stop()、槽3=dispatch(void*)，C1 0x8630b38 / begin 0x8630b8a /
// dispatch_proxy 0x8630c1a / getThreadID 0x82a4218）。


// create_thread() —— ORIG 0x829fd30（0x2f8 字节，逐指令转写）：
// 依次 begin 七个全局线程并 setCurrentPacketPool 绑定包池；任一失败返回 0；
// 尾部非 PvP 整合频道（且 channelType!=7）时再启动 exchange 线程。
// [b28d-28] DIFF | ORIG 0x0829fd30 | P167 CLEAR（28/28）；N82/P73：ORIG bool
// 局部 16× 槽往返（`mov %al,-0x9; movzbl -0x9`），4.4 寄存器化（202 vs 160）。
// [b28d-solo-GameMain] AE | 202/202 | p167 28/28 CLEAR | 修法：宏展开为 bool ok
// 槽形（14 处 `ok = begin()/setCurrentPacketPool(); if (!ok) return 0;`）+
// exchange 分支 2 处（BeginExchangeServerThread 返回类型 bool 由 Main 代落）。
bool create_thread()
{
    // ORIG 0x0829fd43 每步均为 `mov %al,-0x9(%ebp); movzbl -0x9; xor $1; test;
    // je`（P73/P248 档②）——bool 具名局部 ok 的槽往返，16 次复用同一槽 -0x9；
    // `if (!obj->begin())` 直测形（4.4 寄存器化）会少 42 条。
    bool ok;
    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_dispatch_thr)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_NETWORK /*0*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_dispatch_thr)->getThreadID());
    if (!ok) return 0;

    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_udp_thr)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_5 /*5*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_udp_thr)->getThreadID());
    if (!ok) return 0;

    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_monitor_tcp_thr)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_6 /*6*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_monitor_tcp_thr)->getThreadID());
    if (!ok) return 0;

    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_7 /*7*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr)->getThreadID());
    if (!ok) return 0;

    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr_for_logDB)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_MONITOR_TCP /*8*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_db_thr_for_logDB)->getThreadID());
    if (!ok) return 0;

    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_accept_thr)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_DISPATCH /*1*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_accept_thr)->getThreadID());
    if (!ok) return 0;

    ok = reinterpret_cast<ThreadInterface*>(GlobalData::s_net_thr)->begin();
    if (!ok) return 0;
    ok = GlobalData::setCurrentPacketPool(
        THREAD_TYPE_DB /*2*/,
        reinterpret_cast<ThreadInterface*>(GlobalData::s_net_thr)->getThreadID());
    if (!ok) return 0;

    // ORIG 条件：!IsIntegratedPvPBaseChannel() && GetChannelType()!=7
    if (!G_GameWorld()->IsIntegratedPvPBaseChannel()
        && G_GameWorld()->GetChannelType() != 7)
    {
        // ORIG 0x0829ffdb..0x082a0028：同样两步 ok 槽检查（各 4+8 条）。
        // BeginExchangeServerThread 返回类型 bool 已由 Main 代落（0x848f2b8 透传）。
        ok = exchange_server::BeginExchangeServerThread();
        if (!ok) return 0;
        ok = GlobalData::setCurrentPacketPool(
            THREAD_TYPE_TIMER /*3*/,
            exchange_server::getExchangeServerThreadID());
        if (!ok) return 0;
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
// [b28d-28] DIFF | ORIG 0x082a002a | P167 MISSING 4×begin/4×end/2×C1Ev/2×deEv
// vs EXTRA 2×ptEv：ORIG 迭代器声明-赋值两段形（P50）我方直接初始化；语义缺口待补。
// [b28d-solo-GameMain] AE | 146=146 | p167 24/24 CLEAR | 修法：P50 迭代器先
// 声明后赋值（C1Ev ×2）+ switch 每 case 独立 `begin()/end()`（5 对）+ 单表达式
// `if (++it == e)` + 尾部 `(*it).first/(*it).second`（deEv 非 ptEv）+ 去出参判空
// （P171）；**新范式**：`for(;i<n;i++)` 的 incr 被 4.4 另立独立块置于 cond 前，
// 改 `while (i++ < n)` 后 cond 块逐条复现 ORIG `mov;cmp;setl;addl $1;test;jne`
// （比较用旧值、自增在 setl 之后；do-while 形语义多跑一轮，禁用）。
char* SelectKeyString(int* outKey, int* outValue)
{
    int i = 0;                  // ORIG -0x14（函数头声明+初始化，先于 get_rand_int）
    int r = get_rand_int(5);    // ORIG -0x10
    // ORIG 0x082a0046/0x082a0051：两迭代器「先声明后赋值」（P50 复活，各发
    // _Rb_tree_iterator::C1Ev）；begin/end 在 switch 各 case 内对**具体表**调用
    // （每 case 一对，共 5 对；P167 差集 MISSING 4×begin/4×end 即此）。
    std::map<int, std::string>::iterator it;
    std::map<int, std::string>::iterator e;
    int n;                      // ORIG -0xc（无初值；default 不可达）
    switch (r)
    {
    case 0:
        it = g_stStackableScriptFileList.begin();
        e  = g_stStackableScriptFileList.end();
        n = get_rand_int(0x7d0);
        break;
    case 1:
        it = g_stEquipmentScriptFileList.m_fileMap.begin();
        e  = g_stEquipmentScriptFileList.m_fileMap.end();
        n = get_rand_int(0x1388);
        break;
    case 2:
        it = g_stMonsterScriptFileList.begin();
        e  = g_stMonsterScriptFileList.end();
        n = get_rand_int(0x1f4);
        break;
    case 3:
        it = g_questScriptFileList.begin();
        e  = g_questScriptFileList.end();
        n = get_rand_int(0x3e8);
        break;
    case 4:
        it = g_stPassiveObjectScriptFileList.begin();
        e  = g_stPassiveObjectScriptFileList.end();
        n = get_rand_int(0x3e8);
        break;
    }
    // [b28d-solo-GameMain] while 形：ORIG cond 块为 `mov i;cmp n;setl;addl
    // $1,i;test;jne`（比较用旧值、自增在 setl 之后）⇒ 源形 `while (i++ < n)`；
    // for(;i<n;i++) 4.4 会另立 incr 块置于 cond 前（入口跳过），顺序不同。
    // 语义与 for 等价（入口先检查；do-while 会多跑一轮，不可用）。
    while (i++ < n)
    {
        // ORIG 0x082a01ec：`call ppEv` 的返回值（%eax=&it）直接作 operator== 的
        // this（无重新 lea）⇒ 源形为单表达式 `++it == e`（非两条语句）。
        if (++it == e)
        {
            --it;
            break;
        }
    }
    // ORIG 无出参判空（P171 照抄），且尾部调 operator*（deEv）非 operator->（P167）。
    *outKey = r;
    *outValue = (*it).first;
    return const_cast<char*>((*it).second.c_str());
}

// InitCipherKeyString() —— ORIG 0x82a025a（0x1ff 字节，逐指令转写）：
// i=0..0x1387：new Cipher(0x410) 存入 s_secu_cipher_keystring_manager_[i]
// （ORIG 记录 stride 0x10：+0 Cipher*、+4/+8 int 键索引）；
// 首轮经 guard 记录总钥长；重复填充选中密钥串至总钥长后 Initialize。
// 尾部 cMyTrace 输出耗时（文案「推断」）。
// [b28d-28] DIFF | ORIG 0x082a025a | P167 MISSING __cxa_guard_acquire/release/
// abort + _Unwind_Resume vs EXTRA memset：ORIG 静态局部 guard 形（P21）我方
// 以 memset 直清，语义等价但形态不可复现。
// [b28d-solo-GameMain] AE | 135=135 | 真 bug 修：buf[0x800]（ORIG rep stos 计数
// 0x200 dwords = 0x800 字节；旧 buf[0x200] 少清 0x600 字节）+ 函数局部 static
// guard 形（P20/P21）+ P227 全表达式临时链 + 去 rec 指针缓存（P183）。
void InitCipherKeyString()
{
    int t0 = static_cast<int>(s_systemTime_.getCurSec());

    for (int i = 0; i <= 0x1387; ++i)
    {
        // P183：ORIG 每处重算 `shl $0x4; add $&manager`，不缓存 rec 指针。
        GlobalData::s_secu_cipher_keystring_manager_[i].cipher = new Cipher;
        // ORIG 0x082a02c1：函数局部 static 的 guard 形（P20/P21）——guard 变量
        // _ZGVZ19InitCipherKeyStringvE17needed_key_length，值取 **[0]** 的 cipher
        // 总钥长（ORIG 直接传 &manager 基址，非当前 i）。
        static int needed_key_length =
            GlobalData::s_secu_cipher_keystring_manager_[0].cipher->GetTotalKeyLength();

        // ORIG 0x082a0327：`rep stos` 计数 0x200 dwords = **0x800 字节**（P116/P253，
        // 旧实现 buf[0x200] 少清 0x600 字节，属真语义 bug）。
        unsigned char buf[0x800] = {0};
        char* keyStr = SelectKeyString(
            &GlobalData::s_secu_cipher_keystring_manager_[i].keyId,
            &GlobalData::s_secu_cipher_keystring_manager_[i].tblId);
        int keyLen = static_cast<int>(strlen(keyStr));
        int off = 0;
        // ORIG 0x082a03a6 条件只有 `off < needed_key_length`（P171 照抄，无 keyLen 守卫）。
        while (off < needed_key_length)
        {
            memcpy(buf + off, keyStr, keyLen);
            off += keyLen;
        }
        GlobalData::s_secu_cipher_keystring_manager_[i].cipher->Initialize(
            buf, needed_key_length);
    }

    // P118/P139：ORIG 传 _ZZ19InitCipherKeyStringvE19__PRETTY_FUNCTION__；
    // P227：全表达式临时链令 elapsed 在 ctor 前求值并以 %ebx 跨 ctor 保活（无槽）。
    cMyTrace(__PRETTY_FUNCTION__, 0x283e, 9, true, true)(
        "[!] Init CipherKeyString TotalTime : %ds",
        static_cast<int>(s_systemTime_.getCurSec()) - t0);
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
// [b28d-28] DIFF | ORIG 0x080ede91 | N82：大函数槽/视图寻址差（302 vs 296）。
// [b28d-solo-GameMain] N82 封顶 | 296/302 | 大函数帧 0x124 vs 0x12c + 槽平移；
// 池 Size()/Remain() 计算与 IPG 视图寻址展开差异（4.4 内联/槽复用策略）。
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
