// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CMemberExpRewardEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r CMemberExpRewardEvent 实现（OrigMiscEvt 批补齐）。
// 声明见 CMemberExpRewardEvent.h。逐函数依据
// docs/class_func_reports/CMemberExpRewardEvent/ 报告转写：
//   C1 0x081b291c（+0xc 置 10）  D1 0x081b2942  D0 0x081b2972
//   StartEvent() 0x081b2990  StartEvent(Word_Param) 0x081b29ec
//   EndEvent 0x081b2a5a  GetMemberExpRewardRatio 0x081b2ab6
// vtable 0x08bb7b80 = 基类 14 槽，派生无新增虚函数。
// +0x0c 为会员经验加成比值（千分数）：GetMemberExpRewardRatio 直返；
// StartEvent()/EndEvent 重置为 10；StartEvent(Word_Param) 写 param。
// ORIG 追踪用 __FUNCTION__（_ZZN21CMemberExpRewardEvent*E12__FUNCTION__）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN21CMemberExpRewardEventC2Ev`（ORIG 0x81b2b01）；ctor 已是首强符号。
#include <iostream>

#include "CMemberExpRewardEvent.h"
#include "GameTypes.h"   // cMyTrace
// ORIG 0x081b2990：置标志；比值重置为 10；trace 行号 22。
// 事件进行中 GetMemberExpRewardRatio() 直返 +0xc（默认 10）。
// StartEvent()/EndEvent 均重置 m_ratio = 10（ORIG movl $0xa,0xc）。
// trace 采用 __FUNCTION__ 宏以对齐 ORIG _ZZN..E12__FUNCTION__ 符号。
// [GI-ioinit L21] 首强符号前移（P327）：ORIG 0x81b2b01 `_ZN21CMemberExpRewardEventC2Ev`。
#include "CMemberExpRewardEvent/CMemberExpRewardEvent.inc"
}

// 报告反编译：SetEventFlag(true) → m_ratio=10 → trace。
#include "CMemberExpRewardEvent/StartEvent.inc"
}
#include "CMemberExpRewardEvent/StartEvent_1.inc"
}
// ORIG 0x081b2a5a：清标志；m_ratio 重置 10；trace 行号 36。
#include "CMemberExpRewardEvent/EndEvent.inc"
}

// ORIG 0x081b29ec：m_ratio = param.m_duration；trace 行号 29（带参）。
// 实参 param.m_duration 在 ORIG 中于 trace ctor 前物化到 ebx（P37 链式）。
// ORIG C1 0x081b291c：基类构造 + vptr + m_ratio(+0xc) 置 10。
// ORIG D1 0x081b2942：用户空析构（编译器生成 vptr+基类析构 + delete 分支）。
#include "CMemberExpRewardEvent/dtor_CMemberExpRewardEvent.inc"
}

// ORIG 0x081b2ab6：直返 +0xc（mov 0xc(%eax),%eax）。
#include "CMemberExpRewardEvent/GetMemberExpRewardRatio.inc"
}
