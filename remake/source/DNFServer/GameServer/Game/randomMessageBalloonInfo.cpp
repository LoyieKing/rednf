// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 randomMessageBalloonInfo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// randomMessageBalloonInfo.cpp — randomMessageBalloonInfo 强符号（T）实现
//
// 来源（df_game_r，elf32-i386）：
//   randomMessageBalloonInfo::randomMessageBalloonInfo()  T 0x08a00c08
//   randomMessageBalloonInfo::clear()                     T 0x08a00bd2
// 析构为 ORIG 弱符号（W 0x08581d50，vector D1 + string D1），编译器隐式生成
// （ORIG 无 D2，P12），本文件末尾强制实例化以发出 D1/D0。
// 布局：std::string@+0x00 + int@+0x04 + std::vector<std::string>@+0x08。
// ============================================================================
#include "randomMessageBalloonInfo.h"

// ORIG 0x08a00c08：string 默认构造 → vector 默认构造 → clear()（带 unwind 清理路径）
#include "randomMessageBalloonInfo/randomMessageBalloonInfo.inc"
}

// ORIG 0x08a00bd2：+0x04 置 0 + vector::clear + vector::reserve(0xa)
#include "randomMessageBalloonInfo/clear.inc"
}

// 强制实例化隐式析构（D1/D0），使 ORIG 弱符号 _ZN24randomMessageBalloonInfoD1Ev
// 在链接期可得；函数本身为内部链接，不产生对外符号。
namespace
{
#include "randomMessageBalloonInfo/force_randomMessageBalloonInfo_dtor.inc"
}
}
