// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 PacketGuard/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "PacketGuard.h"
#include "GlobalData.h"
#include <pthread.h>

#include "PacketGuard/PacketGuard.inc"
}

// ORIG T 0x0858de4a（size 0x36，17 insns）：本 ctor 在 ORIG 里**不带** EH landing
// pad（同 TU 的 PacketGuard() @0x0858dd4c 却带）——依据基类 ~InterfacePacketBuf
// 非平凡，唯一自洽解释是 InterfacePacketBuf::AcquirePacketBuf 声明为不抛
// （已在 InterfacePacketBuf.h 落 throw()，实测本函数 32→17 insns，逐条同构 AE）。
#include "PacketGuard/PacketGuard_1.inc"
}

#include "PacketGuard/dtor_PacketGuard.inc"
}
