// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 LC_OnlineTeam/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LinkClosureC 批次：online_preliminary::COnlinePreliminaryTeamMgr 两方法。
// ORIG 依据：
//   FindTeam           T 0x0858875e（报告 online_preliminary__COnlinePreliminaryTeamMgr/FindTeam.md）
//   ResetDailyMidnight T 0x08588c5c（同目录 ResetDailyMidnight.md）
#include "OnlinePreliminary.h"

// ORIG 0x0858875e：m_teamMap(+0x04) find(teamId)；命中返回 second，否则 NULL。
online_preliminary::COnlinePreliminaryTeam*
#include "LC_OnlineTeam/FindTeam.inc"
}

// ORIG 0x08588c5c：遍历 m_teamMap，逐 team 调 COnlinePreliminaryTeam::ResetDailyMidnight。
#include "LC_OnlineTeam/ResetDailyMidnight.inc"
}

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

// ORIG 0x085887c8 T
// _ZN18online_preliminary25COnlinePreliminaryTeamMgr11OnEnterUserEP5CUserPc
//（报告 online_preliminary__COnlinePreliminaryTeamMgr/OnEnterUser.md）。
// 键语义：teamData 头部 int teamId —— find/make_pair 的 const int& 参数在
// ABI 上直接复用 teamData 指针解引用（ORIG 0x01a 传 edx=teamData 作引用）。
// [b28d-54 N] ORIG 0x085887c8：帧/槽整体平移（N82）+ map 迭代器临时槽（N86）；
//   调用序列全等。
#include "LC_OnlineTeam/OnEnterUser.inc"
}

// ORIG 0x08588ae8 T
// _ZN18online_preliminary25COnlinePreliminaryTeamMgr11OnLeaveUserEP5CUser
//（报告 OnLeaveUser.md）。
// [b28d-54 N] ORIG 0x08588ae8：槽整体平移 4（N82/N95）+ 迭代器/临时槽（N86）；
//   调用序列全等。
#include "LC_OnlineTeam/OnLeaveUser.inc"
}
