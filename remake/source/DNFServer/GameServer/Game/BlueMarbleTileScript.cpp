// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BlueMarbleTileScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r Game/ BlueMarbleTileScript 专属 TU（从 GameStubs.cpp 拆出）。
// 逐方法对照 ORIG df_game_r 反汇编实现。
#include "BlueMarbleTileScript.h"

#include <string>

#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanStr/ScanInt

// ORIG 0x88d5068：ctor 仅调 clear()。
#include "BlueMarbleTileScript/BlueMarbleTileScript.inc"
}

// ORIG 0x88d509c（0x16d 字节）逐分支转写：
//   loadRDARScriptFile("", path) 失败 → 0；否则循环 ScanType：
//   "[type]"→ScanStr 后 toType 写回 m_type；"[enter dungeon]"→ScanInt，
//   非 0 置 m_char4=1。扫描结束返回 1。
#include "BlueMarbleTileScript/importScript.inc"
}

// ORIG 0x88d520a（0x1cb 字节）：字符串链式比较映射类型枚举。
#include "BlueMarbleTileScript/toType.inc"
}

// ORIG 0x88d507c：m_type=0、m_char4=0、m_value=0。
#include "BlueMarbleTileScript/clear.inc"
}