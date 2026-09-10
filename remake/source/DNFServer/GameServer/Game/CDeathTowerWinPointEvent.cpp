// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CDeathTowerWinPointEvent/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r CDeathTowerWinPointEvent 实现（OrigMiscEvt 批补齐）。
// 声明见 CDeathTowerWinPointEvent.h。报告缺失，依据 ORIG nm+objdump 转写：
//   C1/C2 0x081170e8 D1/D2 0x0811710e D0 0x0811713e
//   StartEvent() 0x0811700e StartEvent(Word_Param) 0x0811706e
//   EndEvent 0x08117096 getBonusRate W 0x08469dcc
// vtable 0x08b4f3a0 = 基类 14 槽；+0xc m_ratio(unsigned)；
// getBonusRate 经 vptr+0x3c IsEventing 判活 → (double)m_ratio/100.0f。
#include "CDeathTowerWinPointEvent.h"
#include "GameTypes.h"   // cMyTrace

// ORIG 0x0811700e：置标志；trace 行号 16；(%x%%) 打印 m_ratio。
// 带参 trace：m_ratio 先物化 ebx 跨 ctor 存活（P37 链式临时形态）。
#include "CDeathTowerWinPointEvent/StartEvent.inc"
}
#include "CDeathTowerWinPointEvent/CDeathTowerWinPointEvent.inc"
}
#include "CDeathTowerWinPointEvent/dtor_CDeathTowerWinPointEvent.inc"
#include "CDeathTowerWinPointEvent/StartEvent_1.inc"
}
// 0x08117096：清标志；trace 行号 34。
#include "CDeathTowerWinPointEvent/EndEvent.inc"
}

// ORIG C1 0x081170e8：基类构造 + vptr + m_ratio(+0xc) 置 0；
// D1 0x0811710e 用户空析构（编译器生成 vptr+基类析构），D0 编译器自动。
// ORIG W 0x08469dcc _ZN24CDeathTowerWinPointEvent12getBonusRateEv：
// IsEventing(0) 虚调判活（vptr+0x3c）；活 → (float)m_ratio/100.0f。
// fildll 零扩展 + flds 100.0f + fdivrp（探针 f14 同形）→ 返回 float。
// b28c-L20: N7 — 残差仅虚调 IsEventing 的间接 call 目标寄存器互换（ORIG 目标 %edx /
// this %eax，我们目标 %eax / this %edx）；flds 100.0f 常量地址按 AE 归一已不计差。
// 4.4 间接 call 恒 %eax（probe4b 七形）⇒ N7 封顶，ndiff=8 仅寄存器互换。
// 4.4.6 后端实测（/tmp/tu_CDeathTowerWinPointEvent_446.o）：本件 ndiff=0 ⇒ 路由切 4.4.6 后可 AE。
#include "CDeathTowerWinPointEvent/getBonusRate.inc"
}
