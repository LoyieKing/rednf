// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LC_OnlineTeam/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LinkClosureC 批次：online_preliminary::COnlinePreliminaryTeamMgr 两方法。
// ORIG 依据：
//   FindTeam           T 0x0858875e（报告 online_preliminary__COnlinePreliminaryTeamMgr/FindTeam.md）
//   ResetDailyMidnight T 0x08588c5c（同目录 ResetDailyMidnight.md）
#include "OnlinePreliminary.h"

// ORIG 0x0858875e：m_teamMap(+0x04) find(teamId)；命中返回 second，否则 NULL。
online_preliminary::COnlinePreliminaryTeam*
#include "LC_OnlineTeam/FindTeam.inc"


#include "LC_OnlineTeam/ResetDailyMidnight.inc"


// ============================================================================
// 头依赖：包构造（PacketGuard/InterfacePacketBuf）、CUser/CUserCharacInfo、
// 流/消息队列（CStreamGuard/StreamPool/MsgQueueMgr/GlobalData）、
// 定时器（TimerDispatcher.h）与 GameWorld 广播。
// ============================================================================
#include <string.h>

#include "CUser.h"
#include "CUserCharacInfo.h"   // SetCurCharacOnlinePreliminaryTeam /
                               // GetCurCharacOnlinePreliminaryTeamId
#include "PacketGuard.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "TimerDispatcher.h"   // Timer_UpdateOnlinePreliminaryTeam::RegistNextTimer
#include "GameWorld.h"         // G_GameWorld / send_all

#include "LC_OnlineTeam/OnEnterUser.inc"


#include "LC_OnlineTeam/OnLeaveUser.inc"

