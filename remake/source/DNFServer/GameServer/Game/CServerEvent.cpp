// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CServerEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CServerEvent（G2-4 批次，服务端事件状态）。
// 全部方法按 ORIG 逐指令转写（GetExpRate 0x0814a6e0 / AddExpRate 0x084e9248 /
// SubExpRate 0x0863bc70 / IsEventing 0x084e927f / TurnOnEvent 0x084e925f /
// TurnOffEvent 0x0863bc87 / GetEventInterval 0x084e9294）。
// ORIG 为静态成员 + 3 个静态数据（B 符号），见 CServerEvent.h。

// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN12CServerEvent10m_nExpRateE`（ORIG 0x8366034）；类静态数据定义为首强符号。
#include <iostream>

#include "CServerEvent.h"

int CServerEvent::m_nExpRate = 0;              // 0x943c138
int CServerEvent::m_nCurEvent[0x100] = {0};    // 0x943c13c
int CServerEvent::m_nCurEventInterval[0x100] = {0};  // 0x943c144

#include "CServerEvent/GetExpRate.inc"
}

#include "CServerEvent/AddExpRate.inc"
}

#include "CServerEvent/SubExpRate.inc"
}

#include "CServerEvent/IsEventing.inc"
}

#include "CServerEvent/TurnOnEvent.inc"
}

#include "CServerEvent/TurnOffEvent.inc"
}

#include "CServerEvent/GetEventInterval.inc"
}
