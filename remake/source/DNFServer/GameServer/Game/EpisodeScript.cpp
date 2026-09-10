// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 EpisodeScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— EpisodeScript（史诗任务脚本条目）+
// importEpicQuestScript（RDARScriptQuest.cpp 解析器）。
// 对照 ORIG 反汇编：
//   EpisodeScript C1 0x8a66028 / D1 0x8a66154 / C1ERKS_ 0x8a6a830
//                clear 0x8a660bc / setEpisodeIndex 0x8a66108
//   importEpicQuestScript 0x8a65ad0（报告
//   docs/class_func_reports/_lt_global_gt_/importEpicQuestScript.md）
// 解析辅助经 DNFLexWrapper.h 权威声明调用真实符号。
// ============================================================================

#include "EpisodeScript.h"

#include "DNFLexWrapper.h"
#include "LogManager.h"
#include "STQuestScript.h"  // g_questScriptBaseDirectory

#include "EpisodeScript/EpisodeScript.inc"
}

#include "EpisodeScript/dtor_EpisodeScript.inc"
}

#include "EpisodeScript/EpisodeScript_1.inc"
}

#include "EpisodeScript/clear.inc"
}

#include "EpisodeScript/setEpisodeIndex.inc"
}

#include "EpisodeScript/importEpicQuestScript.inc"
}
