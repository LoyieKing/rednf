// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BreakAwayPreventImpl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "BreakAwayPreventSystem.h"
#include "CUser.h"

#include "SigTypes.h"
#include "GlobalData.h"
#include "DB_UpdateGiftCount.h"      // DB_UpdateBreakAwayUvList 权威声明


char* NumberToString(unsigned int value, int radix);   // ORIG T 0x0810904b

#include "BreakAwayPreventImpl/getPrevScheduleTime.inc"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "LogManager.h"
#include "InterDispatcher.h"   // Inter_LoadEtc::giveBreakAwayReward（giveBreakAwayDungeonClearReward）

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <string>


namespace break_away_prevent
{

#include "BreakAwayPreventImpl/getDiffConnectDay.inc"


#include "BreakAwayPreventImpl/calBreakAwayUserPoint.inc"


#include "BreakAwayPreventImpl/selectReward.inc"


#include "BreakAwayPreventImpl/updateUvList.inc"


#include "BreakAwayPreventImpl/startBreakAwayPreventSystem.inc"


} // namespace break_away_prevent

#include "BreakAwayPreventImpl/makeRequest.inc"


// ============================================================================
// LinkFinal2 核销 —— break_away_prevent::CBreakAwayPreventSystem::GM_UpdateUvList
// （ORIG T @0x0831ca5a，0xfb）objdump 逐指令转写。
//   this+0x30b UV 名单缓冲（0x1f 字节）：先清零，再按 day 选择预设表：
//     day 1/2/3 → 0x8c242c0（31 字节字符串 "0000…111000010001"）
//     day 4     → 0x8c242e0（"0000…111000001000"）
//   然后 DB_UpdateBreakAwayUvList::makeRequest(accId, this+0x30b) +
//   CLocalChina_DB_BreakAwayReset::makeRequest(accId)；恒返 1。
// 同 TU 补齐两个 CLocalChina DB makeRequest（成员 mangled 与基类无关，
// W8LT_DBTask.cpp 的 DBDispatcher 派生类仅提供 C1/D1/vtable）：
//   CLocalChina_DB_BreakAwayReset::makeRequest(uint)    ORIG T 0x08127f42 (0xd7)
//   CLocalChina_DB_LastDay_BreakAway::makeRequest(uint,int,int) ORIG T 0x08128152 (0x10b)
// ============================================================================

class CLocalChina_DB_BreakAwayReset
{
public:
    static void makeRequest(unsigned int accId);
};

class CLocalChina_DB_LastDay_BreakAway
{
public:
    // ORIG 0x08128152 无 this：三参落 ebp+0x08/0x0c/0x10 → 静态成员
    // （静态/非静态成员 mangled 相同：_ZN32...11makeRequestEjii）。
    static void makeRequest(unsigned int accId, int day, int flag);
};

#include "BreakAwayPreventImpl/makeRequest_1.inc"


#include "BreakAwayPreventImpl/makeRequest_2.inc"
                                                   int day, int flag)
{
    CStreamGuard guard(GlobalData::s_stream_pool->Acquire(
        "localchina/localchina_DBThread.cpp", 0x438), true);   // ORIG 字符串 .cpp
    guard.operator*()->operator<<(0x2de);  // ORIG opcode
    guard.operator*()->operator<<(-1);
    guard.operator*()->operator<<(accId);  // ORIG operator<<(unsigned)（ebp+0x08）
    guard.operator*()->operator<<(flag);   // ORIG operator<<(int)（ebp+0x10，第三参）
    guard.operator*()->operator<<(day);    // ORIG operator<<(int)（ebp+0x0c，第二参）
    GlobalData::s_msgq_mgr->put(MsgQueueMgr::DB_Q, guard);
}

namespace break_away_prevent
{

static const char* const kUvListDay123 = "000000000000000000111000010001";  // 0x8c242c0
static const char* const kUvListDay4   = "000000000000000000111000001000";  // 0x8c242e0

#include "BreakAwayPreventImpl/GM_UpdateUvList.inc"

#include "BreakAwayPreventImpl/giveBreakAwayDungeonClearReward.inc"


}  // namespace break_away_prevent
