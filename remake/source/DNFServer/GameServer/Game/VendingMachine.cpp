// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 VendingMachine/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— CVendingMachineStatistic / TimerVendingMachineItemNotify
// 实现。声明见 VendingMachine.h（唯一）。
//
// 依据 docs/class_func_reports/CVendingMachineStatistic/{addOutputItem,
// updateDataIntoDB}.md 与 TimerVendingMachineItemNotify/registTimer.md，并以
// df_game_r objdump 逐指令核对。C++98 / gnu++0x / -m32 / O0 基线。
// ============================================================================

#include <algorithm>

#include "VendingMachine.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "TimerQueue.h"

// ----------------------------------------------------------------------------
// 构造 / 析构（ORIG 0x0861062c C1/C2、0x08610694 D1/D2）
// ----------------------------------------------------------------------------
#include "VendingMachine/CVendingMachineStatistic.inc"
}

#include "VendingMachine/dtor_CVendingMachineStatistic.inc"
}

// ----------------------------------------------------------------------------
// _init（ORIG 0x086105ac）
// ----------------------------------------------------------------------------
#include "VendingMachine/_init.inc"
}

// ----------------------------------------------------------------------------
// incUseCount（ORIG 0x0822d714）
// ----------------------------------------------------------------------------
#include "VendingMachine/incUseCount.inc"
}

// ----------------------------------------------------------------------------
// addOutputItem（ORIG 0x0860f5a4）
// ----------------------------------------------------------------------------
// [b28c-31] AE：`if (it == end) insert else +=` 正向形复现 ORIG 0x0860f6xx
// `call eq; test; je <+= 块>` 布局（95=95）。
#include "VendingMachine/addOutputItem.inc"
}

// ----------------------------------------------------------------------------
// updateDataIntoDB（ORIG 0x0860f6de）
// ----------------------------------------------------------------------------
// [b28d-30] 复核 DIFF：帧/槽位跨版本差（N82/N93 族），ORIG 见 class_func_reports/CVendingMachineStatistic/updateDataIntoDB.md。
#include "VendingMachine/updateDataIntoDB.inc"
}

// ----------------------------------------------------------------------------
// GetInstanceVendingMachineStatistic（ORIG 0x0860f8ad）—— 函数内静态单例
// ----------------------------------------------------------------------------
CVendingMachineStatistic* GetInstanceVendingMachineStatistic()
{
    static CVendingMachineStatistic instance;
    return &instance;
}

// ----------------------------------------------------------------------------
// TimerVendingMachineItemNotify::registTimer（ORIG 0x08638074）—— static
// ----------------------------------------------------------------------------
#include "VendingMachine/registTimer.inc"
}
