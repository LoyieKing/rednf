// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmRegister/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmRegister.cpp — game_master::CGameMasterMgr 构造注册链完整还原。
// 对照 ORIG CGameMasterMgr::CGameMasterMgr() @0x082a8a28（size 0x18fb）：
//   senior = new CSeniorGameMaster(0)；junior = new CJuniorGameMaster(senior)；
//   随后按 ORIG 顺序注册：数字段 0x01..0x80（79 条 add(int,CCommand*)）结束后，
//   交错区实证为 "bongfail"(CSetMultiboxFailCnt) → 0x82(CBingoGmTest) → "lv"…
//   即 0x82 的 add 夹在名字注册中间，共 80 条 int + 7 条 string。
#include "GameMasterCmd.h"
#include <string>

namespace game_master
{

#include "GmRegister/CGameMasterMgr.inc"


} // namespace game_master
