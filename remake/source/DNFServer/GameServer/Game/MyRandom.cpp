// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 MyRandom/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "MyRandom.h"

// ORIG 0x8aafe18 _ZN8MyRandomC1Ei (0xd)：
//   mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,(%eax); pop; ret
// 仅保存种子到 m_seed。
#include "MyRandom/MyRandom.inc"
}

// ORIG 0x8aafe26 _ZN8MyRandomclEi (0x14)：
//   mov (%eax),%eax; mov %eax,%edx; sar $0x1f,%edx; idivl 0xc(%ebp); mov %edx,%eax
// 有符号除法取余：返回 m_seed % n，不更新 m_seed。
#include "MyRandom/operator.inc"
}
