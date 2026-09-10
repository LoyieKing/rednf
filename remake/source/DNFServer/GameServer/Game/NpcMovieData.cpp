// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 NpcMovieData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// NpcMovieData.cpp — NpcMovieData 强符号（T）实现
//
// 来源（df_game_r，elf32-i386）：
//   NpcMovieData::NpcMovieData()        T 0x089fb78e
//   NpcMovieData::clear()               T 0x089fb7d0
//   NpcMovieData::getNextMoviePath()    T 0x089fb7ee
//     （docs/class_func_reports/NpcMovieData/）
// 析构为 ORIG 弱符号（W 0x08581d96，仅 deque dtor），头内联空析构即可。
// 布局：std::deque<std::string>@+0x00 + int@+0x28（sizeof 0x2c）。
// ============================================================================
#include "NpcMovieData.h"

// ORIG 0x089fb78e：deque 默认构造 → clear()（带 unwind 清理路径）
#include "NpcMovieData/NpcMovieData.inc"
}

// ORIG 0x089fb7d0：deque::clear + 游标置 0
#include "NpcMovieData/clear.inc"
}

// ORIG 0x089fb7ee：返回 std::string（sret 模式）。early-return 结构（P16）：
// 队列空 → 直接返回；游标 < size 时取 deque[游标] 拷入返回值，
// 游标 = (游标+1) % size（无符号 divl 实证）。
#include "NpcMovieData/getNextMoviePath.inc"
}

// ---- ORIG W 弱符号发射段（D1/D2 @0x08581d96，头内联空析构）----
// 本 TU 既有函数不 odr-use 析构（无 NpcMovieData 实例化），头内联空析构的
// 弱符号不会发射；此处真实构造/析构强制发射，不引入导出符号。
namespace
{
#include "NpcMovieData/emitNpcMovieDataDtor.inc"
}
}
