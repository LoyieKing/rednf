// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CPartyExpBonusEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r CPartyExpBonusEvent 实现（LinkMisc3 批补齐；repeat 事件 0x18）。
// 布局：CEventBase（0xc）+ m_field0c（+0x0c，经验加成千分数）。
#include "CPartyExpBonusEvent.h"
#include "GameTypes.h"   // cMyTrace

// ORIG C1/C2 0x08116d4a：基类构造 + vptr 装载 + m_field0c = 0。
#include "CPartyExpBonusEvent/CPartyExpBonusEvent.inc"
}

// ORIG D1/D2 0x08116d70：恢复基类 vptr + 基类析构（编译器自动）。
#include "CPartyExpBonusEvent/dtor_CPartyExpBonusEvent.inc"
}

// ORIG T 0x08116c70：置事件标志后经 cMyTrace 输出
// "Party Exp Bonus Event Start! (%x%%)"（实参为 m_field0c 千分数）。
#include "CPartyExpBonusEvent/StartEvent.inc"
}

// ORIG T 0x08116cd0：m_field0c = param.m_duration 后转调无参 StartEvent()
//（经 vptr+0x08 槽虚调用）。
#include "CPartyExpBonusEvent/StartEvent_1.inc"
}

// ORIG T 0x08116cf8：清事件标志并输出 "Party Exp Bonus Event End!"。
#include "CPartyExpBonusEvent/EndEvent.inc"
}

// ORIG W 0x085bfe1a _ZN19CPartyExpBonusEvent12getBonusRateEib
// 事件进行中 且 memberCount>1 且 !isHelpAbuse → m_field0c/100.0f，否则 0.0f。
    // N7 封顶：唯一残差=虚调用 callee 寄存器互换（N7，探针 /tmp/l22p/vprobe*.cpp）。
#include "CPartyExpBonusEvent/getBonusRate.inc"
}
