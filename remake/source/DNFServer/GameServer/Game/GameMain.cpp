// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 GameMain/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
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

// 全局单例创建/销毁（ORIG T 0x82a7029..0x82a7136；定义属各自翻译域 TU）
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
#include "GameMain/RegistNextTimer.inc"
}

#include "GameMain/RegistNextTimerGage.inc"
}

#include "GameMain/RegistNextTimer_1.inc"
}

#include "GameMain/RegistNextTimer_2.inc"
}

#include "GameMain/RegistNextTimer_3.inc"
}

#include "GameMain/registNextTimer.inc"
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
#include "GameMain/CalcNextTime.inc"
}

// ORIG 0x8637a62：msg=0x57，间隔=参数秒。
#include "GameMain/registNextTimer_1.inc"
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
#include "GameMain/RegistNextTimer_4.inc"
}

// ---------------------------------------------------------------------------
// RegistServerSchedule() —— ORIG 0x82a0459（0x49b 字节）逐条转写。
// 全部为 G_TimerQueue()->InsertTimer(OBJ_TYPE=2, 0, msg, delaySec, 0, 0)。
// ---------------------------------------------------------------------------
// [b28d-28] DIFF | ORIG 0x082a0459 | N82：槽/寄存器分配差（大函数）。
// [b28d-solo-GameMain] N95/N82 封顶 | 235/242 | 真语义 bug 修：ORIG magic
// 0x88888889+`sar $0x4` = **÷30** ⇒ 源形 `p->tm_min % 30`（半点对齐；旧实现
// 误用 tm_mon/15）；0x55 delay 改内联实参（P24）；残差帧 0x74 vs 0x64 + 槽平移。
#include "GameMain/RegistServerSchedule_1.inc"
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
#include "GameMain/ShowLogo.inc"
}

// ---------------------------------------------------------------------------
// Neof_startupGlobalInstances() —— ORIG 0x829f7c5：
// 依序 CREATE_* 单例；Neof_shutdownGlobalInstances() 反序 DESTROY_*。
// ---------------------------------------------------------------------------
#include "GameMain/Neof_startupGlobalInstances.inc"
}

#include "GameMain/Neof_shutdownGlobalInstances.inc"
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
#include "GameMain/Init.inc"
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
#include "GameMain/MainLoop.inc"
}

// ---------------------------------------------------------------------------
// main —— 入口（ORIG 0x829f839）
// ---------------------------------------------------------------------------
#include "GameMain/main.inc"
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
}

#include "GameMain/rdar_terminate.inc"
#include "GameMain/omll_dump_core_file.inc"
#include "GameMain/TerminateServers.inc"
#include "GameMain/signal_handler.inc"
}

// ORIG 0x0829e80b（_Z14rdar_terminatev，T，87 条）：
//   buf[30] 清零；sprintf("./pid/%s.pid", G_CEnvironment()->get_file_name())；
//   printf("???%s\n")；remove 失败 → perror 风格 strerror 输出，
//   成功 → puts RDAR TERMINATED；arrange_users；TerminateServers()。
#include "GameMain/rdar_terminate_1.inc"
}

// ORIG 0x082a21f9（_Z16TerminateServersv，T，189 字节/91 条）：
//   GlobalData::s_stream_pool->Acquire("App.cpp",0x2e2a) → CStreamGuard(s,true)
//   << 0x3e << 0；s_msgq_mgr->put((QUEUE_IDX)1, guard)。
#include "GameMain/TerminateServers_1.inc"
}

// omll_dump_core_file() —— ORIG 0x829f6f4（逐指令一致）：abort();
#include "GameMain/omll_dump_core_file_1.inc"
#include "GameMain/regist_signals_1.inc"
}

// make_dir(char*) —— ORIG _Z8make_dirPc 0x829ee83：opendir 探测，不存在则 mkdir 0777。
#include "GameMain/make_dir.inc"
}

// rdar_init(char**) —— ORIG 0x829e5bf（0x1f 字节）：make_dir("./rdar") 后
// 将 argv 交予 daemon_init（ORIG 0x829e8b9 调用，argv[2]="start"/"start_all"
// 时 fork 守护化；定义 OnlyOrigBatch11.cpp，ORIG T 0x0829e8b9）。
#include "GameMain/daemon_init.inc"
#include "GameMain/rdar_init.inc"
}

// check_cfg_file() —— ORIG 0x829e9ca（0x82 字节）：
// sprintf("<cfg>/%s") + access(F_OK)；存在→true。路径前缀「推断」。
#include "GameMain/check_cfg_file.inc"
}

// check_pid_file() —— ORIG 0x829e948（0x82 字节）：pid 文件存在→true。
#include "GameMain/check_pid_file.inc"
}

// ncl_check_key(char*) 真实实现（ORIG 0x8572cb5：decrypt → MAC/日期区间校验）
// 已移至 NclKeyTools.cpp，本 TU 仅保留调用点。

// send_chat_msg(char*, int) —— ORIG 0x829f6ff（0xc6 字节，逐指令转写主体）：
// UDP socket → sockaddr{AF_INET, htons(env->get_udp_port()), htonl(0)} → sendto。
// 目标地址 htonl(0)=0.0.0.0 为 ORIG 原样（推断实际由配置决定回环地址）。
#include "GameMain/send_chat_msg.inc"
}

// send_suspend_signal() —— ORIG 0x829e6f5（0x116 字节）：
// 读进程 pid 文件（pid/%s.pid，%s = get_file_name()），向 pid 发 SIGUSR1(10)。
#include "GameMain/send_suspend_signal.inc"
}

// printGameServerInfo(bool) —— ORIG 0x829ea4c（0xe 字节，极小尾调）：
// 「推断」骨架：打印服务器基本信息。
#include "GameMain/printGameServerInfo.inc"
}

// CShutdowManager::SendShutdownSignal() —— ORIG 0x82a35a6（0x116 字节）：
// 与 send_suspend_signal 同构，向 pid 文件记录的进程发 SIGTERM(12)。
#include "GameMain/SendShutdownSignal.inc"
}

// check_argv(int, char**) —— ORIG 0x829ea5a（0x420 字节，逐指令转写）。
// [b28d-28] DIFF | ORIG 0x0829ea5a | N82：槽/寄存器分配差（263=263）。
// [b28d-solo-GameMain] N98 封顶 | 261/263 | p167 58/58 CLEAR | 修法：shellsKey 去
// 具名缓存（P0 表达式直用）+ P232 `char framePad[0x10]` 撑帧（0x424→**0x434 与
// ORIG 全等**）+ P244 op 声明/赋值分离提前（槽 -0x14→-0x18）；残差仅 op 槽
// -0x18 vs ORIG -0x1c（4.4 紧凑分配 vs 4.1 在 -0x18/-0x14 预留两槽，未使用标量
// 4.4 不占槽 ⇒ N98 不可翻；再补 int dead 反使帧 0x444 劣化，已回退）。
// [A18 本波] **263=263（gap 0）、ndiff 2**（旧 gap -13）。三修：
//  ①散布 `return 0;` 代 `goto L_RET0`（P325 反向）：4.4 对共享标签恒发
//    单一尾块（jmp 3acd），改散布 return 后逐点 `mov $0,%eax; jmp <epilogue>`，
//    与 ORIG 0x0829ea6a..0x0829ee6e 十八处就地 mov+jmp 全等；
//  ②stop/suspend 两分支保留 `goto L_RET0_TAIL;`（函数尾标签 + fall-through）：
//    ORIG 唯此两点共享 +0x41b 中继块（`jmp 0829ee75`），其余点就地 ⇒ 混合形；
//  ③`total = (int)((unsigned)total + (unsigned)strlen(...))` 双 cast 链
//    （P211/P274 异型化）：复现 ORIG `mov -0x14,%edx; add %edx,%eax;
//    mov %eax,-0x14` 读-加-写三段（裸 `total + strlen` 4.4 恒折 `add %eax,-0x14`）。
// 残差 = **N111**：ORIG 0x0829eb6f `test %al,%al; jmp 0829eb87` 为无入口死块
// （FAIL 段 0x0829eb71..0x0829eb82 永不可达、Ghidra C 亦无该分支），4.4 恒
// 条件化（我方 `je`，语义正确且可达）⇒ 合法 C++ 不可造，1 条指令差封顶。
#include "GameMain/check_argv_1.inc"
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
#include "GameMain/create_thread_1.inc"
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
#include "GameMain/InitCipherKeyString_1.inc"
}

// user_creature 全局池尺寸包装（定义于 CreatureOnlySimple.cpp，无独立头）。
namespace user_creature {
#include "GameMain/GetCreatureEggPoolSize.inc"
#include "GameMain/GetCreaturePoolSize.inc"
#include "GameMain/GetCreatureEggPoolRemainSize.inc"
#include "GameMain/GetCreaturePoolRemainSize.inc"
}

// ProcessServerStatusLog() —— ORIG 0x80ede91（0x4ce 字节，逐指令转写）：
// 聚合 user/party/warroom/deathtower/pvp_room/trade 池占用、消息队列/流池余量、
// IPG 状态、creature/avatar/egg 池等指标后统一 cMyTrace 输出，随后按用户数
// 低于 party/pvp/trade 池占用打印三条 SESSION ERROR 日志。
// [b28d-28] DIFF | ORIG 0x080ede91 | N82：大函数槽/视图寻址差（302 vs 296）。
// [b28d-solo-GameMain] N82 封顶 | 296/302 | 大函数帧 0x124 vs 0x12c + 槽平移；
// 池 Size()/Remain() 计算与 IPG 视图寻址展开差异（4.4 内联/槽复用策略）。
#include "GameMain/ProcessServerStatusLog_1.inc"
}
