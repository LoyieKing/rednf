// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmMacro/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmMacro.cpp — game_master::CMacro / stMacroData 完整还原（第2批）。
// 对照 docs/class_func_reports/game_master__CMacro/ 10 函数报告 + stMacroData ctor/dtor。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include <string.h>

namespace game_master
{

#include "GmMacro/Reset.inc"


#include "GmMacro/stMacroData.inc"


// b28d-solo-GmMacro：stMacroData 无用户析构（隐式 trivial）。
// ORIG nm 0x084b5db4 W 0x5 = 编译器隐式 trivial D1；ORIG Recording 0x084a8802 正常
// 路径无 dtor call、vector 清理走 _Destroy_aux<true>（trivial 分支）。我方旧用户空
// dtor 使 4.4 判 non-trivial：正常路径多 1 次 dtor call + push_back 处 EH landing
// （~stMacroData ×2 + _Unwind_Resume + push esi/ebx），Recording 53 vs ORIG 35。
// m_data 释放仍由 CMacro::Reset 循环负责（浅拷贝 push_back，析构释放会 double-free）。

// ---- CMacro ----

#include "GmMacro/StartMacro.inc"


#include "GmMacro/StopMacro.inc"


#include "GmMacro/RudMacro.inc"


#include "GmMacro/NextCmd.inc"


#include "GmMacro/GetCurCmd.inc"


#include "GmMacro/Recording.inc"


#include "GmMacro/OnRecv.inc"


#include "GmMacro/OnSend.inc"


#include "GmMacro/SendPacket.inc"


} // namespace game_master
