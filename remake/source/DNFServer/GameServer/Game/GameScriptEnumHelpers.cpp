// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GameScriptEnumHelpers/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GameScriptEnumHelpers.cpp — 字符串→枚举映射自由函数。
// 逐函数依据 ORIG df_game_r objdump 反汇编还原（地址见各函数注释）；
// getChangeStatusType / getNpcRoleByString 按 ORIG 结构还原为「函数级静态
// map<char const*,int,less_TCHAR> + SpinLock 双检 + 贯穿式 insert 建表链 +
// switch(码) 派发」原形（键/码/返回值逐条转写自反汇编）。
// 注意：getCharacterJobToEnumValue(0x89bbc0f) 由 CDataManager.cpp 提供，不在此重复。

#include "GameScriptEnumHelpers.h"
#include "DNFLexWrapper.h"   // ScanStr(std::string*)
#include "CEnvironment.h"    // less_TCHAR / SpinLock（ORIG __lookup 静态表依赖）

#include <map>
#include <cstring>

// getChangeStatusType / getNpcRoleByString 的函数级静态查找表类型：
// ORIG 实证 std::map<char const*, int, less_TCHAR>（键按串内容比较）。
typedef std::map<const char*, int, less_TCHAR> EnumLookupMap;

#include "GameScriptEnumHelpers/setCodePageByLocal.inc"


#include "GameScriptEnumHelpers/getStateToEnumValue.inc"

#include "GameScriptEnumHelpers/getActiveStatusToEnumValue.inc"


#include "GameScriptEnumHelpers/getTargetToEnumValue.inc"


#include "GameScriptEnumHelpers/getAttackConditionToEnumValue.inc"


#include "GameScriptEnumHelpers/getCategoryToEnumValue.inc"


#include "GameScriptEnumHelpers/getElementToEnumValue.inc"


#include "GameScriptEnumHelpers/getGradeToEnumValue.inc"


#include "GameScriptEnumHelpers/getObjectTypeToEnumValue.inc"


#include "GameScriptEnumHelpers/getChangeStatusType.inc"


#include "GameScriptEnumHelpers/getNpcRoleByString.inc"


#include "GameScriptEnumHelpers/getNpcMoodFromString.inc"


#include "GameScriptEnumHelpers/getModuleType.inc"


#include "GameScriptEnumHelpers/scanModuleType.inc"


// ---------------------------------------------------------------------------
// setCodePage — ORIG @ 0x8adea1a：全局 codepage 变量置换，返回旧值。
// （Library/Include/Core/UnicodeConvert.cpp 有同名语义实现，但 game 目标未编入，
//   故按 ORIG 在此提供唯一定义。）
// ---------------------------------------------------------------------------
// [b28d-Main R3 迁移·检查点13] gCodePage/getCodePage/setCodePage 已迁入 StringConvert.cpp
// （ORIG 0x09501e90/0x08adea10/0x08adea1a 归属 TU）⇒ 消本 TU 副本，修复码页语义分叉。

// setCodePage 定义已迁入 StringConvert.cpp（声明见 GameScriptEnumHelpers.h）。
