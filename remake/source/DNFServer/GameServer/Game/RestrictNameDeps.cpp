// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 RestrictNameDeps/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// RestrictNameDeps.cpp —— restrict_inputting_name 域 + checkValidCharacName 真实定义。
// 逐符号证据：
//   - importUnicodeRangeData：W _ZN23restrict_inputting_name25RestrictInputtingNameData
//     22importUnicodeRangeDataEPKcPSt6vectorISt4pairIiiESaIS5_EE @0x08ac2eca sz 0x29e
//     （报告 global_constructors_keyed_to_…/unicodeRangeDataList_.md 全文已读）。
//   - C1 @0x08ac3168、isInRange @0x08ac31a0、IsNoneUnicodeRange op() @0x08ac321c、
//     IsNoneUnicodeRange C1 @0x08ac3240、any_of/find_if/none_of 实例族
//     （W @0x08ac3253/0x08ac3278/0x08ac32a0）、isWideStrInUnicodeRange 报告全文已读。
//   - isUtf8StrInUnicodeRange 报告全文已读（WideString + convertFromUtf8 链）。
//   - checkValidCharacName @0x0820582b（报告缺失，objdump 全文转写）。

#include "RestrictNameDeps.h"
#include "StringTypes.h"          // WideString
#include "StringResource.h"       // convertFromUtf8
#include "DNFLexWrapper.h"        // loadRDARScriptFile/ScanType/ScanInt 权威声明
#include "LogManager.h"
#include <string.h>               // strlen
#include <wchar.h>                // wcslen
#include <algorithm>              // std::any_of（ORIG W _ZSt6any_ofIPKw…EEbT_S4_T0_ @0x08ac3253）
#include "CDataManager.h"        // G_CDataManager()/hasSpecialCharacter 族

namespace restrict_inputting_name
{

std::vector<std::pair<int, int> > RestrictInputtingNameData::unicodeRangeDataList_;
bool RestrictInputtingNameData::isLoaded_ = false;

#include "RestrictNameDeps/importUnicodeRangeData.inc"


#include "RestrictNameDeps/RestrictInputtingNameData.inc"


#include "RestrictNameDeps/isInRange.inc"


#include "RestrictNameDeps/operator.inc"


#include "RestrictNameDeps/isWideStrInUnicodeRange.inc"


#include "RestrictNameDeps/isUtf8StrInUnicodeRange.inc"


}  // namespace restrict_inputting_name

#include "RestrictNameDeps/checkValidCharacName.inc"


// ORIG 函数无正常退出路径（外层 for(;;) 永不离开；正常返回仅经 catch→return false）。
// 此行不可达，仅为抑制编译器 non-void 返回警告。
