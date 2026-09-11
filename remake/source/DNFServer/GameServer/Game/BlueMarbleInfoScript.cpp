#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BlueMarbleInfoScript/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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


#include "BlueMarbleInfoScript/dtor_BlueMarbleInfoScript.inc"


#include "BlueMarbleInfoScript/clear.inc"


#include "BlueMarbleInfoScript/getRandomDungeonIndex.inc"


#include "BlueMarbleInfoScript/getBossDungeonIndex.inc"


#include "BlueMarbleInfoScript/getUniqueDungeonIndex.inc"


#include "BlueMarbleInfoScript/getDungeonDifficulty.inc"


#include "BlueMarbleInfoScript/importScript.inc"

