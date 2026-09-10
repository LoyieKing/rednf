// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CNRDScriptData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
#include <string>
#include "DNFLexWrapper.h"
#include "CNRDScriptData.h"

// ============================================================================
// df_game_r CNRDScriptData 实现
// ORIG 见 docs/class_func_reports/CNRDScriptData.md（构造 0x807f858，import
// 0x807fb2c，getInt 0x807fe8a，getFloat 0x807febc，getString 0x807fef4，
// clear 0x807fa58，reset 0x807fac8，create 0x807fabc，destroy 0x807fac2，
// 析构 0x807f966）。脚本解析辅助（loadRDARScriptFile/ScanType/ScanInt/ScanFloat/
// ScanStr）与 logFormat 经 asm-label 调用 ORIG 真实符号（与其它 ST*Script 导入
// 一致）。
// ============================================================================







#include "CNRDScriptData/CNRDScriptData.inc"
}

#include "CNRDScriptData/dtor_CNRDScriptData.inc"
}

#include "CNRDScriptData/clear.inc"
}

#include "CNRDScriptData/reset.inc"
}

#include "CNRDScriptData/create.inc"
}

#include "CNRDScriptData/destroy.inc"
}

#include "CNRDScriptData/getInt.inc"
}

#include "CNRDScriptData/getCLong.inc"
}

#include "CNRDScriptData/getFloat.inc"
}

#include "CNRDScriptData/getCFloat.inc"
}

#include "CNRDScriptData/getString.inc"
}

#include "CNRDScriptData/import.inc"
}