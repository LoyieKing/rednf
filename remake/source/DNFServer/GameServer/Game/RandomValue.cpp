// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 RandomValue/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// CRandomValue（从 GameStubs.cpp 桩迁移后按 ORIG 反汇编真实实现）。
#include "RandomValue.h"
#include <algorithm>
#include <cstdlib>

// ORIG _ZN12CRandomValueC1ERSt6vectorIiSaIiEEiii（0x085df7fe，0x28 字节）：
// +0x00=vector& 地址、+0x04=第4参、+0x08=第3参、+0x0c=第5参。
#include "RandomValue/CRandomValue.inc"
}

// ORIG _ZN12CRandomValueclEv（0x085df826，0xea 字节）：
// value 初值 = m_a(+0x8)；循环至多 m_tryCount 次：
//   r = rand()；value = m_a + r % (m_b - m_a + 1)；
//   若 value 不在排除列表 m_list 中则提前返回。
// 循环耗尽仍返回最后一次 value。
// b28d-D42 verdict=DIFF [N86+N82] ORIG 0x085df826 orig/ours=75/78 ndiff=66。
// 残差为 vector begin/end 返回对象的 copy-elision 差（N86：4.1 恒临时槽+拷贝、
// 4.4 直构）+ 迭代器槽号连锁平移（-0x1c/-0x18/-0x14 vs -0x24/-0x20/-0x18）。
#include "RandomValue/operator.inc"
}
