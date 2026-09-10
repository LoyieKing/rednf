// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CFirstServerOpenEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r CFirstServerOpenEvent 实现（OrigMiscEvt 批补齐）。
// 声明见 CFirstServerOpenEvent.h。逐函数依据
// docs/class_func_reports/CFirstServerOpenEvent/ 报告转写：
//   C1 0x0811ca78  D1 0x0811ca94  D0 0x0811cac4
//   StartEvent() 0x0811cae2  StartEvent(Word_Param) 0x0811cb34
//   EndEvent 0x0811cb86  IsEventing 0x0811cbd8
// vtable 0x08b530e0 = 基类 14 槽 + 派生新增槽 15（IsEventing 非 const
// 重载，未覆盖基类 const IsEventing）；无派生数据字段（sizeof 0xc）。
// 追踪用 __FUNCTION__（_ZZN21CFirstServerOpenEvent*E12__FUNCTION__）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN21CFirstServerOpenEventC2Ev`（ORIG 0x811cc46）；ctor 已是首强符号。
#include <iostream>

#include "CFirstServerOpenEvent.h"
#include "GameTypes.h"   // cMyTrace

// ORIG C1 0x0811ca78：仅基类构造 + vptr（无派生字段）。
#include "CFirstServerOpenEvent/CFirstServerOpenEvent.inc"
}
// ORIG D1 0x0811ca94：用户空析构（编译器生成 vptr+基类析构）。
#include "CFirstServerOpenEvent/dtor_CFirstServerOpenEvent.inc"
#include "CFirstServerOpenEvent/IsEventing.inc"
}
// ORIG 0x0811cae2：置标志；trace 行号 32。
#include "CFirstServerOpenEvent/StartEvent.inc"
}
// ORIG 0x0811cb34：忽略参数；置标志；trace 行号 46。
// 报告反编译：StartEvent(Word_Param) 与无参版完全相同（参数未使用），
// 汇编仅 SetEventFlag + cMyTrace 两段，无字段写入（该类无派生字段）。
// 与 StartEvent() 的唯一差异是行号（0x2e vs 0x20）与 __FUNCTION__ 符号。
// 参数按值传入（Word_Param 4 字节），无成员写入。
// 行号 0x2e=46（ORIG movl $0x2e,0x8(%esp)）。
// param 未参与任何运算（ORIG 无 movzwl 读参）。
// Word_Param 按值传参（4 字节），汇编仅 SetEventFlag 段后即 trace。
#include "CFirstServerOpenEvent/StartEvent_1.inc"
}
#include "CFirstServerOpenEvent/EndEvent.inc"
}
