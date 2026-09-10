// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 MatchRoomList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— 匹配房间列表（MatchingSystem 链）实现
// 依据 docs/class_func_reports/{CWaitingRoomList,CLeagueMatchRoomList,
// CMultiMatchRoomList}.md 与 ORIG 汇编（df_game_r，见 MatchRoomList.h）。
// ORIG 符号：
//   CWaitingRoomList：C1 0x0856904e / D1 0x08569062 / find 0x08568872 /
//     erase 0x085688be / insert 0x085688d8 / doMatch 0x08566d9c
//   CLeagueMatchRoomList：C1 0x08568fb0 / AddRoom 0x08567e9e /
//     DelRoom 0x08567f72 / Match 0x08567d00
//   CMultiMatchRoomList：C1 0x08566efc / AddRoom 0x08567954 /
//     DelRoom 0x08567bba / Match 0x085675d0 /
//     GetWaitingRoomCombination 0x0856703c
//   GetUserName 0x08563ff7；fair_pvp::CFairMatch::GetPlayCountPerOneTeam
//   0x08562834（恒返 1）
// 日志依赖：CSwitchLog（_ZN10CSwitchLogC1EPKciii / clEPKcz，CSwitchLog.cpp）、
//   LogManager::logFormat（LogManager.h）。
// ============================================================================

#include <cstdio>

#include "MatchRoomList.h"
#include "CUser.h"
#include "CFairMatch.h"   // fair_pvp::CFairMatch 完整定义（原嵌套 stub 已迁）
#include "LogManager.h"

// ---- CSwitchLog（PvP 日志；与 PvP_Room.cpp 同款局部声明） ----
class CSwitchLog
{
public:
    CSwitchLog(const char* name, int line, int a, int b);
    void operator()(const char* fmt, ...);

private:
    char m_pad[0x10];
};

// ============================================================================
// CWaitingRoomList
// ============================================================================

#include "MatchRoomList/CWaitingRoomList.inc"
}

#include "MatchRoomList/dtor_CWaitingRoomList.inc"
}

#include "MatchRoomList/find.inc"
}

#include "MatchRoomList/erase.inc"
}

#include "MatchRoomList/insert.inc"
}

#include "MatchRoomList/doMatch.inc"
}

// ============================================================================
// CLeagueMatchRoomList
// ============================================================================

#include "MatchRoomList/AddRoom.inc"
}

#include "MatchRoomList/DelRoom.inc"
}

#include "MatchRoomList/Match.inc"
}

// ============================================================================
// CMultiMatchRoomList
// ============================================================================

#include "MatchRoomList/CMultiMatchRoomList.inc"
}

#include "MatchRoomList/AddRoom_1.inc"
}

#include "MatchRoomList/DelRoom_1.inc"
}

#include "MatchRoomList/Match_1.inc"
}

#include "MatchRoomList/GetWaitingRoomCombination.inc"
}

// ============================================================================
// fair_pvp::CFairMatch::GetPlayCountPerOneTeam（ORIG 0x08562834 恒返 1）
// 由 MatchRoomLCB.cpp 唯一定义；GetUserName 及三类房间表方法为
// MatchRoomList.cpp 唯一 owner（key function → vtable 0x8ca3ba8/0x8ca3bc0）。
// ============================================================================
