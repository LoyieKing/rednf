// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 itemData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// itemData.cpp — itemData ctor/clear/析构（ORIG W 0x080f1134/0x080f1162/
// 0x080f32fa 逐指令转写）。
//
// ORIG 形状：每次 store 前重读 this（mov 0x8(%ebp),%eax）→ 必须写函数体
// 逐字段赋值（P29：初始化列表会缓存 this 为 ecx，指令形状不同）。
//   movb $0,0x0 / movb $1,0x1 / movb $1,0x2 / movl $0,0x4 / movl $0,0x8
// C1/C2 同址（无虚/无基类，gcc 生成同一体）；clear 为独立函数、指令同 C1。
// D1/D2（W 0x080f32fa）为空体（push/mov/pop/ret）——**隐式** trivial 析构，
// 不再用户声明：ORIG eventReward::ImportRewardScript 0x080ef3a2 对 6 处
// `itemData item = curItem;` 局部副本零 D1Ev 调用（用户空析构会逐点发 call）；
// 删声明后 D1/D2 仍由使用点发射同名 W 符号（P317），符号集不变。
// ============================================================================
#include "itemData.h"

#include "itemData/itemData.inc"
}

#include "itemData/clear.inc"
}
