// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFieldLengthChecker/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// CFieldLengthChecker.cpp — ORIG 转写（df_game_r elf32-i386）
// AddField 双分支：静态 fieldLength_ 空 → 解析字段定义（'(' ')' ';' 分隔，填充
// 静态 fieldNames_/fieldLength_）；实例成员空 → 解析 "values(" 数据行，按列
// 更新静态 fieldLength_ 为实测最大长度（'...' 引号值去转义、trim）。
#include <iostream>
#include "CFieldLengthChecker.h"
#include "CEnvironment.h"

#include "CFieldLengthChecker/GetSocketTypeCharacter.inc"


// fieldNames_/fieldLength_ 的权威定义在本 TU（判据 P186/P108 键名 + 地址邻接）：
// ORIG static_init @0x0862690c 依次构造 ioinit@0x94734d6、fieldNames_@0x94734a0、
// fieldLength_@0x94734ac，且与 `CFieldLengthChecker::AddField`@0x086272e8、
// `sync_script::DisplayMaxLength`@0x0861ca94、`GetSocketTypeCharacter`@0x0861ca1b
// 同属一个 TU（GI 键名 `_GLOBAL__I__Z22GetSocketTypeCharactert`）。
// 反向证据：CItemList TU 的 ORIG static_init @0x0851398b 只含 ioinit（26 条），
// 故两个静态定义不得留在 CItemList.cpp（否则本 TU GI 缺 2 对象、CItemList GI 多 2 对象，
// 两侧同时 DIFF）。
std::vector<std::string> CFieldLengthChecker::fieldNames_;
std::vector<unsigned int> CFieldLengthChecker::fieldLength_;

#include "CFieldLengthChecker/CFieldLengthChecker.inc"


#include "CFieldLengthChecker/dtor_CFieldLengthChecker.inc"


#include "CFieldLengthChecker/AddField.inc"

