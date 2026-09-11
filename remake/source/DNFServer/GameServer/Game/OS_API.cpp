// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OS_API/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "OS_API.h"

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

#include "CMyFileLog.h"
char* NumberToString(unsigned int value, int radix);   // 定义 LinkFinalDeps.cpp/ServerCommon

// 5 个 ORIG 符号的实现（df_game_r 0x0858c53a..0x0858c789）。
// 其余 5 符号（GetDateTimeTick/snprintf/Sleep/WriteLog/uSleep）由既有文件以
// namespace OS_API 定义，声明见 OS_API.h 与 CInventory.h。
// 全部为自由函数（ORIG mangled 与 namespace 函数一致，均无 this 参与）。

namespace OS_API
{

#include "OS_API/Exit.inc"


#include "OS_API/WriteLastBookmark.inc"


#include "OS_API/WriteLastSig.inc"


#include "OS_API/WriteLastSig_1.inc"


#include "OS_API/WriteLoginInfo.inc"


}  // namespace OS_API
