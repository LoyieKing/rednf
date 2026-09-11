// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CClientCrashDownLogStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CClientCrashDownLogStatistic.h"
#include "CEnvironment.h"
#include "CServerProxyMgr.h"
#include "CStatisticServerProxy.h"
#include "DBMgr.h"
#include "GlobalData.h"
#include "MySQL.h"
#include "Packet_Reason_Crash_Down_Info.h"

#include <cstdio>
#include <cstring>
#include <ctime>

// ---- ORIG 符号（nm -S --defined-only df_game_r）----
// 类本体：
//   RegisterClientCrashDown(int)  T 0x0860d038
//   CarryClientCrashDown()        T 0x0860d072
//   MakeClientCrashDownQuery(...) T 0x0860d1fe
//   UpdateClientCrashDown()       T 0x0860d444
//   Init()                        W 0x086102d4
//   CClientCrashDownLogStatistic() W 0x086102da
//   ~CClientCrashDownLogStatistic() W 0x08610800
// record：
//   stClientCrashDownLog_t()        W 0x08610846（仅构造 m_reasonCount，不初始化 m_occTime）
//   ~stClientCrashDownLog_t()       W 0x0861085c
//   stClientCrashDownLog_t(const&)  W 0x08612dfa
//   operator=(const&)               W 0x08612ea8
// 报告：docs/class_func_reports/CClientCrashDownLogStatistic/*.md（4 份全读）

#include "CClientCrashDownLogStatistic/stClientCrashDownLog_t.inc"


#include "CClientCrashDownLogStatistic/stClientCrashDownLog_t_1.inc"



#include "CClientCrashDownLogStatistic/dtor_stClientCrashDownLog_t.inc"


CClientCrashDownLogStatistic::stClientCrashDownLog_t&
CClientCrashDownLogStatistic::stClientCrashDownLog_t::operator=(
    const stClientCrashDownLog_t& other)
{
    m_occTime = other.m_occTime;
    m_reasonCount = other.m_reasonCount;
    return *this;
}

#include "CClientCrashDownLogStatistic/CClientCrashDownLogStatistic.inc"


#include "CClientCrashDownLogStatistic/dtor_CClientCrashDownLogStatistic.inc"


#include "CClientCrashDownLogStatistic/Init.inc"


#include "CClientCrashDownLogStatistic/RegisterClientCrashDown.inc"


#include "CClientCrashDownLogStatistic/CarryClientCrashDown.inc"


#include "CClientCrashDownLogStatistic/MakeClientCrashDownQuery.inc"


#include "CClientCrashDownLogStatistic/UpdateClientCrashDown.inc"

// ----------------------------------------------------------------------------
// GetInstanceClientCrashDownLogStatistic —— ORIG T 0x0860cfa5：
// 函数级静态 CClientCrashDownLogStatistic（guard 0x94732e8 / 实例
// 0x9473420，cxa_guard_acquire → C1 → release → __cxa_atexit(~D1)，
// P20 范式），返回静态实例指针。DB_TimerStatisticsLog::dispatch 消费。
// ----------------------------------------------------------------------------
CClientCrashDownLogStatistic* GetInstanceClientCrashDownLogStatistic()
{
    static CClientCrashDownLogStatistic instance;
    return &instance;
}
