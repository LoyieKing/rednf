// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 STPassiveObjectAdditionalParticleInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// STPassiveObjectAdditionalParticleInfo.cpp — C1/clear 实现
//
// 来源（df_game_r，elf32-i386）：
//   C1   W 0x08a3dad0：初始化列表 3 × string 默认构造 → 函数体 call clear()
//     （P69：ctor 内 call clear 必须外联定义；含 unwind 清理路径）
//   clear W 0x08a3da5a：3 × string::operator=(const char*) 赋 ""（0x8e1dc00）→
//     +0x0c/+0x10/+0x14/+0x18 movl $0 直写 → +0x1c movb $0 直写。
// ============================================================================
#include "STPassiveObjectAdditionalParticleInfo.h"

#include "STPassiveObjectAdditionalParticleInfo/STPassiveObjectAdditionalParticleInfo.inc"
}

#include "STPassiveObjectAdditionalParticleInfo/clear.inc"
}
