// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 PvP_GuildWar_Log/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// ============================================================================
// df_game_r PvP_GuildWar_Log（G2-4 公会战 PvP 日志，0x68 字节）
// 逐函数对照 docs/class_func_reports/PvP_GuildWar_Log.md 与 ORIG 反汇编实现；
// 布局：+0x00 char m_guildCnt、+0x04 int m_startTime、+0x08 STPvPGuild[8]（0xc 每项）。
// 注意：PvP_deps.h 桩签名 SetpvpStartTime(long long) 与 ORIG mangling El（long）
// 不符，本 TU 以 ORIG 为准（long）；STPvPGuild ctor 一并实现（ORIG 085df7d0 弱）。
// ============================================================================

#include <cstdio>
#include <cstring>

#include "CSystemTime.h"
#include "CUser.h"           // CUser / CUserCharacInfo（基类，offset 0）
#include "CEnvironment.h"    // G_CEnvironment() / get_file_name()

// ---- CMyFileLog / STPvPGuild / PvP_GuildWar_Log：唯一声明点为权威头 ----
//   CMyFileLog.h（ctor/operator() 由 ServerCommon/DNFFileLog.cpp、CMyFileLog.cpp 提供）
//   PvPTypes.h（STPvPGuild 0xc / PvP_GuildWar_Log 0x68 权威布局，本 TU 提供方法实现）
#include "CMyFileLog.h"
#include "PvPTypes.h"

#include "PvP_GuildWar_Log/STPvPGuild.inc"


#include "PvP_GuildWar_Log/PvP_GuildWar_Log.inc"


#include "PvP_GuildWar_Log/dtor_PvP_GuildWar_Log.inc"


#include "PvP_GuildWar_Log/Clear.inc"


#include "PvP_GuildWar_Log/SetpvpStartTime.inc"


#include "PvP_GuildWar_Log/InsertPvPGuild.inc"


#include "PvP_GuildWar_Log/WriteGuildWarPvPLog.inc"

