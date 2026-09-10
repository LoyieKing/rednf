// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 TileIndexInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// TileIndexInfo（G2 蓝海币/地图批次）
// ----------------------------------------------------------------------------
// ORIG：
//   ctor  _ZN13TileIndexInfoC1Ev  0x80d9f04（+0/+4 两 int 置 0）
//   reset _ZN13TileIndexInfo5resetEv  0x80d9f1c（+0/+4 两 int 置 0）
// 布局：{ int m_dungeon; unsigned int m_item; }（8 字节），与 BlueMarblePOD.h 一致。
// 经 BlueMarbleUserInfo.cpp 的 asm-label extern（_ZN13TileIndexInfoC1Ev / reset）引用。
// ============================================================================

#include "BlueMarblePOD.h"

#include "TileIndexInfo/TileIndexInfo.inc"
}

#include "TileIndexInfo/reset.inc"
}