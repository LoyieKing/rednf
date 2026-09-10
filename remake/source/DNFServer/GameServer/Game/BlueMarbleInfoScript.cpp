// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 BlueMarbleInfoScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r BlueMarbleInfoScript（G2-4 大富翁配置脚本）还原（2026-08-17）。
// 逐函数对照 docs/class_func_reports/BlueMarbleInfoScript.md 与 ORIG 反汇编实现；
// 目标：编译产物与 ORIG 逐操作数一致（AE 口径）。
// 脚本解析函数（loadRDARScriptFile/ScanType/ScanInt/ScanFloat）经 asm-label
// extern 调用真实符号。
#include <string>

#include "BlueMarbleInfoScript.h"
#include "DNFLexWrapper.h"   // loadRDARScriptFile/ScanType/ScanInt/ScanFloat（真实符号，§9）

// ============================================================================
// BlueMarbleInfoScript 实现
// ============================================================================

#include "BlueMarbleInfoScript/BlueMarbleInfoScript.inc"
}

#include "BlueMarbleInfoScript/dtor_BlueMarbleInfoScript.inc"
}

#include "BlueMarbleInfoScript/clear.inc"
}

#include "BlueMarbleInfoScript/getRandomDungeonIndex.inc"
}

#include "BlueMarbleInfoScript/getBossDungeonIndex.inc"
}

#include "BlueMarbleInfoScript/getUniqueDungeonIndex.inc"
}

#include "BlueMarbleInfoScript/getDungeonDifficulty.inc"
}

// [b28d-L1] N82 封顶：ORIG 0x088d589c；443 vs 449 条、帧 0x60 vs 0x70。P167 残差 1 处（ScanInt(Pi)/ScanInt(Pb) 计数，疑区间假象；.o 内两类重定位均存在）。残余为槽位/帧形态差（N82/P232），语义等价。
#include "BlueMarbleInfoScript/importScript.inc"
}
