// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CCoinEventPerDay/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 每日币事件 CCoinEventPerDay 实现。
// 报告：docs/class_func_reports/CCoinEventPerDay/（7 函数全读）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN16CCoinEventPerDayC2Ev`（ORIG 0x810ad89）；ctor 已是首强符号。
#include <iostream>

#include "CCoinEventPerDay.h"

#include "GameTypes.h"             // cMyTrace

// ---------------------------------------------------------------------------
// ORIG 0x0810ab10：基类构造 + vptr 置位 + 4 个档位字节（+0x09..+0x0c）置 1。
#include "CCoinEventPerDay/CCoinEventPerDay.inc"
}

// ORIG 0x0810ab48 / 0x0810ab78：D1 重置 vptr 后调 CEventBase::~CEventBase（末尾
// __in_chrg 常量折叠不 delete）；D0 调 D1 后 operator delete。空体 + 虚析构由
// 编译器生成同形。
#include "CCoinEventPerDay/dtor_CCoinEventPerDay.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0810ab96：SetEventFlag(true) + 4 个档位字节置 1 +
// cMyTrace(__FUNCTION__,0x38,0) ("Coin Event Per Day Start!")。
#include "CCoinEventPerDay/StartEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0810ac04：SetEventFlag(true) + 整 4 字节写 param 到 +0x09 +
// cMyTrace(__FUNCTION__,0x41,0) 输出 4 个档位字节
// ("Coin Event Per Day Start! coin(%d) coin(%d) coin(%d) coin(%d) ")。
// b28d-76（N76 封顶，语义等价）：ORIG `lea 0xc(%ebp),%edx; mov 0x8(%ebp),%eax;
// mov (%edx),%edx; mov %edx,0x9(%eax)` = 4.1 求值序「RHS 地址 → LHS 地址 → RHS 值」；
// 4.4-O0 对 `m_coinPack = *(unsigned int*)&param;` 恒 RHS 完整求值先
// （`lea ...,%eax; mov (%eax),%edx; mov this,%eax`）。20 形探针（volatile 读/写、
// 视图/引用解引用、字段 int↔unsigned 同异型化、数组下标、语句表达式、具名中转）
// 只能产「this 先」或「RHS 值先」两族，无一复现 ORIG 前两条顺序 ⇒ N76。
#include "CCoinEventPerDay/StartEvent_1.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0810aca8：SetEventFlag(false) + 4 个档位字节置 1 +
// cMyTrace(__FUNCTION__,0x63,0) ("Coin Event Per Day End")。
#include "CCoinEventPerDay/EndEvent.inc"
}

// ---------------------------------------------------------------------------
// ORIG 0x0810ad16：level < 0x12 → +0x09；< 0x1b → +0x0a；否则 +0x0b（movzbl 字节读）。
#include "CCoinEventPerDay/GetCoinNoPerDay.inc"
}
