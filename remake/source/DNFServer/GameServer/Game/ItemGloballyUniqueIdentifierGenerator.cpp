// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ItemGloballyUniqueIdentifierGenerator/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CItemGloballyUniqueIdentifierGenerator / itemGloballyUniqueIdentifier_t
// ORIG 证据（df_game_r nm -S / objdump -d，报告缺失按 §3.2 objdump 直转）：
//   reset()  W 0x085139e8：char 循环 i=0..9 把 this 前 10 字节清零（setle 判界）。
//   generate T 0x0889246c：SetServerId(field)；SetDatetime(time(0))；
//            SetSequenceNo((ushort)getSeq())。
//   getSeq   W 0x088947e2：++m_seq；若 (unsigned)m_seq > 0xea5f 则归零；返回。
#include "ItemGloballyUniqueIdentifierGenerator.h"
#include <time.h>   // time（generate SetDatetime）

// [b28d-D43] N4 scaled 写 vs lea 折叠：ORIG `movb $0,(%edx,%eax,1)` vs 4.4 `lea (%edx,%eax,1); movb $0,(%eax)`（ORIG 0x08899dfa）。
#include "ItemGloballyUniqueIdentifierGenerator/reset.inc"
}

#include "ItemGloballyUniqueIdentifierGenerator/getSeq.inc"
}

#include "ItemGloballyUniqueIdentifierGenerator/generate.inc"
}

CItemGloballyUniqueIdentifierGenerator g_ItemGloballyUniqueIdentifierGenerator;
