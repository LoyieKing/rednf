#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LinkMiscSecuTimerSolo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "TimerDispatcher.h"
#include "TimerQueue.h"

#include "STMonsterScript.h"

#include "CServerProxyMgr.h"
#include "CGuildServerProxy.h"
#include "CGuildServerProxy.h"   // CEventManager 完整定义（CServerProxyMgr 模板析构）

#include "PacketTypes.h"

#include "CInventory.h"          // CCreatureMgr
#include "CCreature.h"           // CCreature::CheckDiedCreature

#include "CDataManager.h"        // CAutoMarketConditionsControl
#include "CEnvironment.h"
#include "GameWorld.h"
#include "GameTypes.h"           // cMyTrace
#include "CreatureScript.h"      // MODULE_NAME_TAG（ORIG nm 权威名）
#include "DNFLexWrapper.h"       // ScanType
#include "LogManager.h"
#include "PackSet.h"             // PackSet::is_exist（ORIG T 0x08ac498c）

#include "GlobalData.h"           // s_stream_pool / s_msgq_mgr / s_systemTime_
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "SigTypes.h"             // SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL
#include "InterDispatcher.h"   // Inter_MonitorServerEventStart
#include "PacketGuard.h"
#include "InterfacePacketBuf.h"

#include <time.h>
#include <string>
#include <string>
#include <vector>
#include <map>

#include "../../../Library/Include/Core/SecureStdio.h"   // ss_strcpy / ss_strcat

// ---------------------------------------------------------------------------
// RDAR 脚本域全局（定义于 ScriptExtras.cpp；ORIG B 0x949b168 / 0x949b180）
// ---------------------------------------------------------------------------
extern std::vector<std::string> g_szScriptFilePriorityBaseDirectory;
extern char g_szScriptFileBaseDirectory[0x104];
// s_bUseScriptPack 为 ORIG RDARScript.cpp TU-local static（d 0x93f4d24，重建定义
// 于 ScriptExtras.cpp）；跨 TU 经其真实访问器 UseScriptPack（ORIG T 0x088bb7cf）。
extern bool UseScriptPack();  // [b28e P248 档② 两全形] 与 ScriptExtras.cpp 同步：
                              // 定义侧 `static bool s_bUseScriptPack` + `bool` 返回
                              // ⇒ 本体 `movzbl mem; ret` 直返（无归一化、identical 保持）；
                              // 本 TU 调用点 `call; xor $1; test %al; je` = ORIG 同形。
                              // [历史] 仅改返回类型（字段仍 char）会令本体插 test/setne
                              // （ScriptExtras 合计 293→292）⇒ 已按 P248 档② 字段同型化
                              // 消解；调用点仍差 1 条 call vs TU-local movzbl（P189 封顶）。
extern PackSet g_script_package;  // ORIG B 0x9494fe0（OnlyOrigBatch12.cpp 定义）

// [SemA02 2026-09-12 正位迁移] findFileWithServerToken 已迁往 ScriptExtras.cpp
// （ORIG 中它与 UseScriptPack/s_bUseScriptPack 同属 RDARScript.cpp 域）⇒ 本处不再定义。
extern bool findFileWithServerToken(char* path);   // ORIG T 0x088bb7db（ScriptExtras.cpp）


#include "LinkMiscSecuTimerSolo/getScriptFileFullPath.inc"


#include "LinkMiscSecuTimerSolo/processModuleTagSet.inc"


#include "LinkMiscSecuTimerSolo/processModuleTagSet_1.inc"

#include "LinkMiscSecuTimerSolo/GetLinearDataForMonsterAbility.inc"


// ===========================================================================
// Timer 域注册/生命周期。RegistNextTimer 族（TimerNatType/TimerNatTypeUser/
// TimerQueueSizeLog/TimerStatisticsDetailChannelInfo）已由 TimerDispatcher.cpp
// 权威定义（本文件重复实现于 2026-08-29 删除，避免多重定义）。
// 其余 Timer 生命周期 ctor/dtor 仍在本 TU。
// ===========================================================================

// TimerBossDieTimeout ctor W 0x0863c7fa / dtor W 0x0863fe2e（仅 vptr）。
__attribute__((weak)) TimerBossDieTimeout::TimerBossDieTimeout() throw()
{
}

__attribute__((weak)) TimerBossDieTimeout::~TimerBossDieTimeout() throw()
{
}

// TimerStartVillageAttacked ctor W 0x0863cd92 / dtor W 0x0863ee98。
__attribute__((weak)) TimerStartVillageAttacked::TimerStartVillageAttacked() throw()
{
}

__attribute__((weak)) TimerStartVillageAttacked::~TimerStartVillageAttacked() throw()
{
}

// ===========================================================================
// CServerProxyMgr<CGuildServerProxy>::GetServerProxy 显式实例化
// （ORIG W 0x0811d3b8；模板定义在 CServerProxyMgr.h，缺实例化点）。
// ===========================================================================
template CGuildServerProxy*
CServerProxyMgr<CGuildServerProxy>::GetServerProxy(ENUM_SERVER_GROUP);

// ===========================================================================
// competition_reward_result::competition_reward_result  ORIG W 0x081b47a8
// （+0x00/+0x04 int 清零 + +0x08 字节清零）。
// ===========================================================================
// b28c-l23 修复：兄弟 lane 已给冻结头 PacketTypes.h:2011 该 ctor 声明加
// `throw ()`（P216），本 TU 的定义必须同步异常规范，否则 4.4 报
// "declaration ... throws different exceptions" 整 TU 编译失败。
__attribute__((weak)) competition_reward_result::competition_reward_result() throw()
{
    m_0 = 0;
    m_4 = 0;
    m_8 = 0;
}

#include "LinkMiscSecuTimerSolo/CheckDiedCreature.inc"


#include "LinkMiscSecuTimerSolo/GetMarketConditionRate.inc"


#include "LinkMiscSecuTimerSolo/MakeNotiSetDurability.inc"


#include "LinkMiscSecuTimerSolo/SendNotiSetDurability.inc"


#include "LinkMiscSecuTimerSolo/SendNotiSetDurability_1.inc"
{
    // [bughunt13] ORIG 0x082f85b7 `fldl 0x8c1fc58` = 1000.0（与 (CUser*) 重载同常量；旧码 100.0 误）。
    SendNotiSetDurability(static_cast<int>(m_durCurrent * 1000.0));
}

// ===========================================================================
// DB_UpdateAutoMarketConditionsControl  ORIG T 0x08433d70 makeRequest /
// 头 0x15b；SIG_UPDATE_AUTO_MARKET_CONDITIONS_CONTROL（SigTypes.h，pad 0x30）
// 逐字段填充（+0x00..+0x28，DB 侧按此布局读）。
// ===========================================================================
class DB_UpdateAutoMarketConditionsControl
{
public:
    // [SweepTimerDispatcher2] 判据㉓：ORIG 该符号为 **T**（我方原类内 inline ⇒ W）
    // ⇒ 改为类外定义发射强符号（调用点仅本 TU 578 行）。
    static void makeRequest(unsigned long long a, unsigned long long b,
                            unsigned long long c, unsigned long long d,
                            int e, int f, int g);
};

#include "LinkMiscSecuTimerSolo/makeRequest.inc"


#include "LinkMiscSecuTimerSolo/SendEventNotiPacket.inc"

#include "LinkMiscSecuTimerSolo/LoadDatabase.inc"

