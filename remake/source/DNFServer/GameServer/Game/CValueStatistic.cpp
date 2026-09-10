// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CValueStatistic/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ CValueStatistic 实现（独立 TU，从 GameStubs.cpp 拆出）。
// 对照 docs/class_func_reports/CValueStatistic.md：
//   AddValueStatistic 0x0860d682 / InsertValueStatistic 0x0860d8a4
#include "CInventory.h"
// [Lane37CUser 2026-09-10 解环] CInventory.h 的 `#include "CUser.h"` 已改前向声明
//   （解环）⇒ 依赖传递获得 CUser 完整定义的 TU 需自行直补。
#include "CUser.h"
#include "GlobalData.h"
#include "CServerProxyMgr.h"
#include <string.h>

// ---- CStatisticServerProxy（定义见 GameStubs_remaining.cpp） ----
#include "CStatisticServerProxy.h"

#include "Packet_Value_Statistic.h"

CValueStatistic* GetInstanceValueStatistic()
{
    static CValueStatistic instance;
    return &instance;
}

#include "CValueStatistic/reset.inc"
}

#include "CValueStatistic/AddValueStatistic.inc"
}

#include "CValueStatistic/InsertValueStatistic.inc"
}
