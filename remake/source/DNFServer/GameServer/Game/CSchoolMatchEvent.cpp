// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CSchoolMatchEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// CSchoolMatchEvent —— 学院匹配 repeat 事件实现。
// 全部函数逐段对齐 docs/class_func_reports/CSchoolMatchEvent/ 下 7 份报告：
//   CSchoolMatchEvent.md(0x08273ecc ctor) / ~CSchoolMatchEvent.md(0x08273f1a) /
//   ~CSchoolMatchEvent_08273f7c.md(0x08273f7c) / StartEvent.md(0x08273f9a) /
//   StartEvent_08273fec.md(0x08273fec) / EndEvent.md(0x0827423c) /
//   AppendInfo.md(0x082742a8)。
#include "CSchoolMatchEvent.h"

#include <algorithm>

#include "CEnvironment.h"   // G_CEnvironment / get_channel_no
#include "GameWorld.h"      // G_GameWorld / GameWorld::GetChannelType
#include "CLeagueMatch.h"   // CLeagueMatch::SetMatchAble/SetPlayCountPerOneTeam/SetBattleMode
#include "OnlinePreliminary.h"  // PvPGlobalEnvironmentVariable::school_match_elo_k_/k2_
#include "GameTypes.h"      // cMyTrace
#include "PacketGuard.h"    // PacketGuard（AppendInfo 参数完整类型）

#include "CSchoolMatchEvent/CSchoolMatchEvent.inc"
}

#include "CSchoolMatchEvent/dtor_CSchoolMatchEvent.inc"
}

#include "CSchoolMatchEvent/StartEvent.inc"
}

#include "CSchoolMatchEvent/EndEvent.inc"
}

#include "CSchoolMatchEvent/AppendInfo.inc"
}

#include "CSchoolMatchEvent/StartEvent_1.inc"
}
