// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CLogGameChannel/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "CLogGameChannel.h"
#include "SigTypes.h"
#include "GlobalData.h"
#include "DBMgr.h"
#include "MySQL.h"
#include "TimerQueue.h"
#include "StreamPool.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "CEnvironment.h"
#include <string.h>

// [A31-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit；GetLogTargetDB
// 定义前移为本 TU 首个强外部符号 ⇒ GI 键名
// `_GLOBAL__I__ZN8WongWork15CLogGameChannel14GetLogTargetDBEv`（ORIG 0x8550763）。
#include <iostream>

// df_game_r 还原 —— WongWork::CLogGameChannel（LogGameChannel 定时落库）。
// 参照 docs/class_func_reports/WongWork__CLogGameChannel/ 反汇编逐函数实现。
// 布局见 CLogGameChannel.h（ctor 0x082a71c4：+0/+4/+8 清零，+0xc period=10）。

namespace WongWork
{

// ORIG 0x085503c4
#include "CLogGameChannel/GetLogTargetDB.inc"
}

// ORIG 0x082a71c4 W
#include "CLogGameChannel/CLogGameChannel.inc"
}

// ORIG 0x085503ea
#include "CLogGameChannel/StartLogTimer.inc"
}

// ORIG 0x085503fe
#include "CLogGameChannel/SetNextLogTimer.inc"
}

// ORIG 0x085506de
#include "CLogGameChannel/IncInUser.inc"
}

// ORIG 0x08550700
#include "CLogGameChannel/IncOutUser.inc"
}

// ORIG 0x085506c6
#include "CLogGameChannel/ResetInOutUser.inc"
}

// ORIG 0x085503ea
#include "CLogGameChannel/ReqSaveLog.inc"
}

// ORIG 0x08550612
#include "CLogGameChannel/DoSaveLog.inc"
}

} // namespace WongWork
