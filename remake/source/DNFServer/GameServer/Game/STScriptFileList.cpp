#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 STScriptFileList/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— STScriptFileList（ORIG T 0x88bad3a 族）。
//   FindIndexByFullScan(char const*) const（ORIG T 0x88bae56，mangled
//   _ZNK16STScriptFileList19FindIndexByFullScanEPKc）：
//     遍历 std::map<int,std::string> m_fileMap（+0x00），
//     返回 value==path 的 key；未找到返回 0xffffffff（-1）。
//   （证据见 docs/class_func_reports/STScriptFileList/FindIndexByFullScan.md）

#include "NpcScript.h"
#include "LogManager.h"
#include "DNFLexWrapper.h"

// ORIG 0x0949b284 BSS：为 0 时重复索引报错返回 0；非 0 时重复项以负键覆盖插入。
extern char g_isAllowScriptDuplication;


#include "STScriptFileList/STScriptFileList.inc"


#include "STScriptFileList/Clear.inc"

#include "STScriptFileList/GetFileName.inc"


#include "STScriptFileList/FindIndexByFullScan.inc"


#include "STScriptFileList/GetIndex.inc"


#include "STScriptFileList/loadRDARScriptListFile.inc"

