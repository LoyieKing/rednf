// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EventNewCharacterReward/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— EventNewCharacterReward 事件实体（新手角色活动奖励）。
// 全部函数逐指令对照 docs/class_func_reports/EventNewCharacterReward/*.md
// 的 ## 汇编 与 ## 反编译 C 转写（报告地址见各函数头注释）：
//   C1 0x0816bac4 / D1 0x0816baf4 / D0 0x0816bb24 / StartEvent() 0x0816bb42 /
//   StartEvent(Word_Param) 0x0816bb94 / EndEvent() 0x0816bc98 /
//   isEventCharacter 0x0816bcea / eventExpRatio 0x0816bd06 /
//   eventCharacterJob 0x0816bd22
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN23EventNewCharacterRewardC2Ev`（ORIG 0x816bd6d）；ctor 已是首强符号。
#include <iostream>

#include "EventNewCharacterReward.h"
#include "GameTypes.h"   // cMyTrace

// ORIG C1 0x0816bac4：CEventBase ctor + vptr 设置，+0x0c m_job=-1、
// +0x10 m_field10=0（每字段重读 this，函数体赋值形态）。
#include "EventNewCharacterReward/EventNewCharacterReward.inc"
}

// ORIG D1 0x0816baf4：vptr 设置 + CEventBase::~CEventBase() + 删除标志检查。
#include "EventNewCharacterReward/dtor_EventNewCharacterReward.inc"
}

// ORIG 0x0816bb42：SetEventFlag(true) + cMyTrace(__PRETTY_FUNCTION__,0x16,0)。
#include "EventNewCharacterReward/StartEvent.inc"
}

// ORIG 0x0816bb94：param 两个 ushort → +0x0c/+0x10，并同步 m_addInfo；
// m_job∈[0,10] 时日志(0x22) + 虚调 StartEvent()（vptr+8），否则日志(0x27)。
// [b28c-31] N7：末尾虚调用 callee ORIG %edx vs 4.4 %eax（76=76；其余仅字符串
// 立即数/call 目标重定位显示差）。
#include "EventNewCharacterReward/StartEvent_1.inc"
}

// ORIG 0x0816bc98：SetEventFlag(false) + cMyTrace(__PRETTY_FUNCTION__,0x2f,0)。
#include "EventNewCharacterReward/EndEvent.inc"
}

// ORIG 0x0816bcea（size 0x1c）：+0x0c == param_1（分支形态 cmp/jne+mov 1/0）。
#include "EventNewCharacterReward/isEventCharacter.inc"
}

// ORIG 0x0816bd06：fildl +0x10 千分数 / 100.0f（_DAT_08b80014=0x42c80000）。
#include "EventNewCharacterReward/eventExpRatio.inc"
}

// ORIG 0x0816bd22：直返 +0x0c。
#include "EventNewCharacterReward/eventCharacterJob.inc"
}
