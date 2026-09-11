// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GameMain/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
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
#include "GameMain/get_rand_int.inc"
#include "GameMain/setCodePageByLocal.inc"
using GlobalData::s_systemTime_;  // ORIG 0x0941f714（CSystemTime 全局实例）

#include "GameMain/CREATE_CEnvironment.inc"
#include "GameMain/CREATE_CGameManager.inc"
#include "GameMain/CREATE_CDataManager.inc"
#include "GameMain/CREATE_InterDispatcher.inc"
#include "GameMain/CREATE_EpollHandler.inc"
#include "GameMain/CREATE_PacketDispatcher.inc"
#include "GameMain/CREATE_Store.inc"
#include "GameMain/CREATE_TimerQueue.inc"
#include "GameMain/CREATE_TimerDispatcher.inc"
#include "GameMain/regist_signals.inc"
#include "GameMain/check_argv.inc"
#include "GameMain/IsLightServer.inc"
#include "GameMain/create_thread.inc"
#include "GameMain/InitCipherKeyString.inc"
#include "GameMain/RegistServerSchedule.inc"
#include "GameMain/sleep_ext.inc"
#include "GameMain/ProcessAntiBotInputWrapper.inc"
#include "GameMain/ProcessServerStatusLog.inc"

#include "GameMain/ProcessAntiBotInputWrapper_1.inc"
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

// GetConfigFileString(tag_name_value_map_t&, const char*)  _Z19GetConfigFileStringRSt3mapISsSsSt4lessISsESaISt4pairIKSsSsEEEPKc
// [P7HeaderAxis 2026-09-11] P139 prett 修正：ORIG prett @0x08c1b180 size 0x3e 实读
//   `char* GetConfigFileString(tag_name_value_map_t&, const char*)`；我方原 size 0x1d0
//   （签名直写 map 展开 + 返回 const char*）。两处拼写改为 ORIG 形式：① 参数写 typedef
//   名（typedef 定义见 GameTypes.h，底层类型由 mangled 第二参
//   `RSt3mapISsSsSt4lessISsESaISt4pairIKSsSsEEE` 唯一确定）；② 返回类型 `const char*`
//   → `char*`（返回类型不入 mangled ⇒ 零 ABI，全库无其它声明/调用点）。cast 不产指令。
// ORIG 0x0829d7b3：conf.find(key) 命中且 value 非空 → 返回 value.c_str()；
// 未命中/空值 → logFormat(1,"App.cpp","char* GetConfigFileString(...)",0x1fc7,
// "%s tag error", key) 后返回 ""。
// [b28d-28] DIFF | ORIG 0x0829d7b3 | N82：迭代器/string 临时槽序 + 槽平移。
// [b28d-solo-GameMain] AE/IDENTICAL | 92=92 | p167 24/24 CLEAR | 修法：find(key)
// 隐式临时 string（ORIG find 后立即 `call string::~string` + EH 链覆盖 find）+
// P225 while 形复现 `test %eax,%eax; setne %al; test %al,%al; je` 物化。
char* GetConfigFileString(
    tag_name_value_map_t& conf, const char* key)
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
            return const_cast<char*>(it->second.c_str());
        }
    }
    LogManager::logFormat(1, "App.cpp",
                          __PRETTY_FUNCTION__,
                          0x1fc7, "%s tag error", key);
    return const_cast<char*>("");
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

#include "GameMain/RegistNextTimer.inc"


#include "GameMain/RegistNextTimerGage.inc"


#include "GameMain/RegistNextTimer_1.inc"


#include "GameMain/RegistNextTimer_2.inc"


#include "GameMain/RegistNextTimer_3.inc"


#include "GameMain/registNextTimer.inc"


// ---------------------------------------------------------------------------
// TimerStatisticLevel / TimerScheduleTenSecond / Timer_DungeonInoutOpenTime
// ---------------------------------------------------------------------------
// ORIG m_nTimeTable 0x93ace24 D：{1,10,14,17,21}（统计小时点表）。
int TimerStatisticLevel::m_nTimeTable[5] = { 1, 10, 0xe, 0x11, 0x15 };

#include "GameMain/CalcNextTime.inc"


#include "GameMain/registNextTimer_1.inc"


#include "GameMain/RegistNextTimer_4.inc"


#include "GameMain/RegistServerSchedule_1.inc"



// CommonTime::SetCurTime 声明见 CUserCharacInfo.h（CommonTime 权威定义处）。

// ---------------------------------------------------------------------------
// g_npcScript 全局变量（BSS 零初始化，与 ORIG nm 类型 B 一致）
// ---------------------------------------------------------------------------
char g_npcScriptBaseDirectory[0x100];
STScriptFileList g_npcScriptFileList;

#include "GameMain/ShowLogo.inc"


#include "GameMain/Neof_startupGlobalInstances.inc"


#include "GameMain/Neof_shutdownGlobalInstances.inc"


#include "GameMain/Init.inc"


#include "GameMain/MainLoop.inc"


#include "GameMain/main.inc"


// ===========================================================================
// 阶段A收尾（undef2_MAIN 批次）—— 入口域缺失符号定义。
// 以下全部为 ORIG 真实符号（nm --defined-only df_game_r 核对），按
// objdump -d 按地址还原；未完整转写的分支以「推断」标注。
// ===========================================================================

#include "GameMain/CREATE_CEnvironment_1.inc"
#include "GameMain/DESTROY_CEnvironment.inc"
#include "GameMain/CREATE_CGameManager_1.inc"
#include "GameMain/DESTROY_CGameManager.inc"
#include "GameMain/CREATE_CDataManager_1.inc"
#include "GameMain/DESTROY_CDataManager.inc"
#include "GameMain/CREATE_EpollHandler_1.inc"
#include "GameMain/DESTROY_EpollHandler.inc"
#include "GameMain/CREATE_Store_1.inc"
#include "GameMain/DESTROY_Store.inc"
#include "GameMain/CREATE_TimerQueue_1.inc"
#include "GameMain/DESTROY_TimerQueue.inc"

#include "GameMain/CREATE_InterDispatcher_1.inc"
#include "GameMain/DESTROY_InterDispatcher.inc"
#include "GameMain/CREATE_PacketDispatcher_1.inc"
#include "GameMain/DESTROY_PacketDispatcher.inc"
#include "GameMain/CREATE_TimerDispatcher_1.inc"
#include "GameMain/DESTROY_TimerDispatcher.inc"

#include "GameMain/regist_signal.inc"
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

#include "GameMain/rdar_terminate.inc"
#include "GameMain/omll_dump_core_file.inc"
#include "GameMain/TerminateServers.inc"

#include "GameMain/signal_handler.inc"


#include "GameMain/rdar_terminate_1.inc"


#include "GameMain/TerminateServers_1.inc"


#include "GameMain/omll_dump_core_file_1.inc"

#include "GameMain/regist_signals_1.inc"


#include "GameMain/make_dir.inc"


#include "GameMain/daemon_init.inc"
#include "GameMain/rdar_init.inc"


#include "GameMain/check_cfg_file.inc"


#include "GameMain/check_pid_file.inc"


// ncl_check_key(char*) 真实实现（ORIG 0x8572cb5：decrypt → MAC/日期区间校验）
// 已移至 NclKeyTools.cpp，本 TU 仅保留调用点。

#include "GameMain/send_chat_msg.inc"


#include "GameMain/send_suspend_signal.inc"


#include "GameMain/printGameServerInfo.inc"


#include "GameMain/SendShutdownSignal.inc"


#include "GameMain/check_argv_1.inc"


// ThreadInterface —— 权威定义见 ThreadInterface.h（ORIG 布局 +0 vptr / +4 m_tid /
// +8 m_tidCopy，虚表槽0=stop()、槽3=dispatch(void*)，C1 0x8630b38 / begin 0x8630b8a /
// dispatch_proxy 0x8630c1a / getThreadID 0x82a4218）。


#include "GameMain/create_thread_1.inc"


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

#include "GameMain/InitCipherKeyString_1.inc"


// user_creature 全局池尺寸包装（定义于 CreatureOnlySimple.cpp，无独立头）。
namespace user_creature {
#include "GameMain/GetCreatureEggPoolSize.inc"
#include "GameMain/GetCreaturePoolSize.inc"
#include "GameMain/GetCreatureEggPoolRemainSize.inc"
#include "GameMain/GetCreaturePoolRemainSize.inc"
}

#include "GameMain/ProcessServerStatusLog_1.inc"

