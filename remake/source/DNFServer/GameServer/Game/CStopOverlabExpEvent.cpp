// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CStopOverlabExpEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 停止重叠经验加成事件 CStopOverlabExpEvent 实现。
// 报告缺失（§3.2 兜底）：nm 定址 + objdump 逐指令转写（0x081177a0..0x081178c7）。
#include "CStopOverlabExpEvent.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x081177a0：仅基类构造 + vptr 置位（无自有成员初始化）。
#include "CStopOverlabExpEvent/CStopOverlabExpEvent.inc"
}

// ORIG 0x081177bc / 0x081177ec：D1 重置 vptr 后调 CEventBase::~CEventBase；
// D0 调 D1 后 operator delete。空体 + 虚析构由编译器生成同形。
#include "CStopOverlabExpEvent/dtor_CStopOverlabExpEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811780a：SetEventFlag(true) +
// cMyTrace(__PRETTY_FUNCTION__,0x15,0)
// ("CStopOverlabExpEvent Event Start!")。
#include "CStopOverlabExpEvent/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0811785c：thunk —— 经 vptr slot2 虚调 StartEvent()（mov (%eax),%eax;
// add $0x8; mov (%eax),%edx; call *%edx），不读 param。
#include "CStopOverlabExpEvent/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x08117876：SetEventFlag(false) +
// cMyTrace(__PRETTY_FUNCTION__,0x1f,0)
// ("CStopOverlabExpEvent Event End!")。
#include "CStopOverlabExpEvent/EndEvent.inc"
}
