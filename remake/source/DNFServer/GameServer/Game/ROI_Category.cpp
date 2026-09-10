// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ROI_Category/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// LinkHelper 核销批次 —— ROI_Category 实现（/tmp/undef_r21.txt：ROI_Category::_sort）。
// ORIG 证据：
//   _ZN12ROI_CategoryC1Ev   W @0x0822ac10 sz 0x1f：+0/+4/+8 三个 dword 清零。
//   _ZN12ROI_Category5_sortEv W @0x0822ac30 sz 0x10a：3 记录 (short key, byte mark)
//   冒泡排序，objdump 逐指令转写。
// ============================================================================

#include "ROI_Category.h"

#include "ROI_Category/ROI_Category.inc"
}

// ORIG 0x0822ac30：记录 i 的 key=m_keys[i]，mark=m_marks[i]。
// 三次比较交换：if(k0>k1){swap k0,k1 & m0,m1}
//               if(k1>k2){swap k1,k2 & m1,m2}
//               if(k0>k1){swap k0,k1 & m0,m1}
// short 字段直比（无 static_cast）复现 ORIG `movzwl (%eax),%edx` 一步直落 +
// jle 有符号比较（b28c-lane25 探针 /tmp/l25probe/roi.cpp fA/fB 实证）。
#include "ROI_Category/_sort.inc"
}
