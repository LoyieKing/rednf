// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CSchoolMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CSchoolMgr（学院 PvP 频道用户/学院分区管理器，G2-4 批次）。
// 逐函数对照 docs/class_func_reports/CSchoolMgr.md 与 ORIG 反汇编实现。
// 布局（ORIG ctor 0x856f206 / dtor 0x856f0ba 推导）：
//   CSchoolMgr ：+0x00 std::map<uint,uint> m_schoolUserCount
//                +0x18 CSchoolArea m_schoolArea（sizeof 0x28）
//   CSchoolArea：+0x00 std::map<uint,int> m_areaMap
//                +0x18 std::stack<int> m_areaStack
//   CSchoolMember：CBelong(+0 vptr) + CUser*（sizeof 8）；IsRight 覆写纯虚。
// 外部依赖（RDARScriptStringManager / GameWorld::send_group / CUser 访问器）
// 以 asm-label extern 或本地视图声明对齐 ORIG 符号。
#include <stdio.h>
#include <string.h>

#include <map>
#include <stack>
#include <deque>

#include "InterfacePacketBuf.h"
#include "CEnvironment.h"
#include "GameWorld.h"
#include "CSchoolMgr.h"
#include "RDARScriptStringManager.h"
#include "PacketGuard.h"

// ---------------------------------------------------------------------------
// 外部符号声明（实现属其它 TU）
// ---------------------------------------------------------------------------
class CUser;

// ---------------------------------------------------------------------------
// PacketGuard（ORIG 0xc 字节：InterfacePacketBuf + int + char）
// ---------------------------------------------------------------------------


// ---------------------------------------------------------------------------
// CSchoolArea / CSchoolMgr 类声明见 CSchoolMgr.h（唯一声明点）

// ============================================================================
// CSchoolArea 实现
// ============================================================================

#include "CSchoolMgr/CSchoolArea.inc"
}

#include "CSchoolMgr/dtor_CSchoolArea.inc"
}

#include "CSchoolMgr/GetAreaIndex.inc"
}

#include "CSchoolMgr/SetAreaMapping.inc"
}

#include "CSchoolMgr/AddSchoolArea.inc"
}

#include "CSchoolMgr/DelSchoolArea.inc"
}

#include "CSchoolMgr/SetSchoolArea.inc"
}

#include "CSchoolMgr/GetAreaIndex_1.inc"
}

// ============================================================================
// CBelong / CSchoolMember 实现
// ============================================================================

#include "CSchoolMgr/CSchoolMember.inc"
}

#include "CSchoolMgr/IsRight.inc"
}

// ============================================================================
// CSchoolMgr 实现
// ============================================================================

#include "CSchoolMgr/CSchoolMgr.inc"
}

#include "CSchoolMgr/dtor_CSchoolMgr.inc"
}

#include "CSchoolMgr/AddUser.inc"
}

#include "CSchoolMgr/DelUser.inc"
}

#include "CSchoolMgr/SendBroadcastUserIn.inc"
}

#include "CSchoolMgr/SendBroadcastUserOut.inc"
}

#include "CSchoolMgr/GetUserCount.inc"
}

#include "CSchoolMgr/SetSchoolArea_1.inc"
}

#include "CSchoolMgr/GetSchoolArea.inc"
}
