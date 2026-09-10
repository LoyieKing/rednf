// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CCollectArchieveEventLog/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 收集图鉴事件日志 CCollectArchieveEventLog 实现。
// ORIG 符号（报告缺失，nm 定址 + objdump 转写，地址见头文件注释）。
#include "CCollectArchieveEventLog.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x08116b48（弱 W）：仅基类构造 + vptr 置位。
#include "CCollectArchieveEventLog/CCollectArchieveEventLog.inc"
}

// ORIG 0x08116b64 / 0x08116b94（弱 W）：标准虚析构（空体编译器生成同形）。
#include "CCollectArchieveEventLog/dtor_CCollectArchieveEventLog.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08116bb2：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x19,0)
// ("Collect Archieve Event Log Start!")。
#include "CCollectArchieveEventLog/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08116c04：thunk——经 vtable slot 2（StartEvent()）虚调用。
// [b28c-31] N7：转调虚调用 callee ORIG %edx vs 4.4 %eax（12=12，仅寄存器名差）。
#include "CCollectArchieveEventLog/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08116c1e：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x23,0)
// ("Collect Archieve Event Log End!")。
#include "CCollectArchieveEventLog/EndEvent.inc"
}
