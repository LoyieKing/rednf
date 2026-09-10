// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BlueMarbleMapScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r BlueMarbleMapScript（G2-4 大富翁地图脚本）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarbleMapScript.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 跨类依赖（loadRDARScriptFile/ScanType/ScanInt/BlueMarbleScriptManager/
// BlueMarbleTileScript 构造）经 asm-label extern / 声明调用真实符号。
#include <string>

#include "BlueMarbleMapScript.h"
#include "BlueMarbleTileScript.h"
#include "BlueMarbleScriptManager.h"   // BlueMarbleScriptManager::getInsance/getTile（§9）
#include "DNFLexWrapper.h"             // loadRDARScriptFile/ScanType/ScanInt（§9）

// ============================================================================
// BlueMarbleMapScript 实现
// ============================================================================

#include "BlueMarbleMapScript/BlueMarbleMapScript.inc"
}

#include "BlueMarbleMapScript/dtor_BlueMarbleMapScript.inc"
}

#include "BlueMarbleMapScript/clear.inc"
}

#include "BlueMarbleMapScript/getTile.inc"
}

#include "BlueMarbleMapScript/importScript.inc"
}
