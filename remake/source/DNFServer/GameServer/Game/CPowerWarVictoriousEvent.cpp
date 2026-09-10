// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPowerWarVictoriousEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 力战胜利事件 CPowerWarVictoriousEvent 实现。
// 报告：docs/class_func_reports/CPowerWarVictoriousEvent/ 全部 6 个方法
// （CPowerWarVictoriousEvent / ~CPowerWarVictoriousEvent / ~CPowerWarVictoriousEvent_08268968 /
//  StartEvent / StartEvent_08268a68 / EndEvent）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN24CPowerWarVictoriousEventC2Ev`（ORIG 0x8268c0b）；ctor 已是首强符号。
#include <iostream>

#include "CPowerWarVictoriousEvent.h"

#include "CEnvironment.h"          // G_CEnvironment / m_serverEnvir.m_channelNo
#include "CDataManager.h"          // G_CDataManager / m_powerParameter.m_victoriousRewardTime
#include "TimerQueue.h"            // G_TimerQueue / TimerEntry / TIMER_MESSAGE
#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0826891c：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CPowerWarVictoriousEvent/CPowerWarVictoriousEvent.inc"
}

// ORIG 0x08268938 / 0x08268968：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CPowerWarVictoriousEvent/dtor_CPowerWarVictoriousEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08268986：IsEventing(0) 为假时置事件标志、挂 0x8b 定时器
// （时长 = m_victoriousRewardTime × 0x3c 分钟）并打日志。
#include "CPowerWarVictoriousEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08268a68：同 StartEvent()（ORIG 未写 m_param）。
#include "CPowerWarVictoriousEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08268b4a：IsEventing(0) 为真时清事件标志、清零 m_param（movw×2）并打日志。
#include "CPowerWarVictoriousEvent/EndEvent.inc"
}
