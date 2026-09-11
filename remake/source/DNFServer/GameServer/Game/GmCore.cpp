// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmCore/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmCore.cpp — game_master 命令体系核心框架（第1批）。
// 还原：IGameMaster/CJunior/CSenior 责任链、CCommand 基类、CParser::parse、
//       CCommandMap 双索引注册表、CGameMasterMgr 分发入口。
// 全部函数体对照 df_game_r ORIG 汇编（docs/class_func_reports/game_master__* 与 objdump）。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "PacketDispatcher.h"

extern void trim(std::string& s);   // ORIG _Z4trimRSs @0x0829f068

namespace game_master
{

// ---- IGameMaster ----


#include "GmCore/HandleRequest.inc"




#include "GmCore/HandleRequest_1.inc"




#include "GmCore/HandleRequest_2.inc"


// ---- CCommand ----


#include "GmCore/SetParser.inc"


#include "GmCore/GetUser.inc"


#include "GmCore/executeTemplate.inc"


#include "GmCore/CParser.inc"


#include "GmCore/dtor_CParser.inc"


#include "GmCore/parse.inc"


#include "GmCore/CCommandMap.inc"


#include "GmCore/dtor_CCommandMap.inc"


#include "GmCore/add.inc"


#include "GmCore/add_1.inc"


#include "GmCore/get.inc"


#include "GmCore/get_1.inc"


#include "GmCore/GetDispatcher.inc"


#include "GmCore/HandleRequest_3.inc"


#include "GmCore/GetCommand.inc"


#include "GmCore/GetCommand_1.inc"


} // namespace game_master
