// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ScanJobStr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r ScanJobStr(std::string*, std::string**) _Z10ScanJobStrPSsPS_ 0x08a6143f
// ----------------------------------------------------------------------------
// 语义（objdump 0x08a6143f，全文已读）：
//   token = g_dnfLex.getToken()（全局 @0x9495020）：
//     DNF_TOK_STRING(7)   → *jobName = toTString(g_dnfLex.getString())，found=1；
//     DNF_TOK_PAIR_INT(9) → key = g_dnfLex.getPairInt()；若再取词为
//       DNF_TOK_PAIR_STR(10) → *jobName = g_scriptStringManager_.findString(
//       key, g_dnfLex.getPairStr(), 0)（flag 传 NULL），found=1。
//   out != 0 时：for (i = 0; i <= 10; ++i)：
//     snprintf(buf[0x400], "%s_%d", g_dnfLex.getPairStr(), i)（rodata "%s_%d"）；
//     s = g_scriptStringManager_.findString(key, buf, &ok)；
//     s != 0 && ok → out[i] = new std::string(); *out[i] = s（含异常展开）。
//   返回 found（movzbl 直返，bool）。
// 依赖：DNFLexWrapper（g_dnfLex）/ RDARScriptStringManager（g_scriptStringManager_）
// / toTString(const char*)（SyncScriptDeps.h），均已存在。
// ============================================================================

#include <cstdio>
#include <string>

#include "DNFLexWrapper.h"
#include "RDARScriptStringManager.h"
#include "SyncScriptDeps.h"

// [b28d-11b] 修法：去具名 p 局部（P183/P172 完整下标重算）+ if-else 链改 switch（P251 载入寄存器 cmp 链）→ IDENTICAL 117 @0x08a6143f
#include "ScanJobStr/ScanJobStr.inc"
}
