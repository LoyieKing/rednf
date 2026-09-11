// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MsgQueueMgr/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r MsgQueueMgr 实现（ORIG 0x8570e40-0x8571257，AE 口径还原）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN11MsgQueueMgrC2Ev`（ORIG 0x8571298）；ctor 已是首强符号。
#include <iostream>

#include "MsgQueueMgr.h"

#include "MsgQueueMgr/MsgQueueMgr.inc"


#include "MsgQueueMgr/dtor_MsgQueueMgr.inc"


#include "MsgQueueMgr/put.inc"


#include "MsgQueueMgr/put_1.inc"


#include "MsgQueueMgr/get.inc"


#include "MsgQueueMgr/empty.inc"


#include "MsgQueueMgr/remain.inc"


#include "MsgQueueMgr/get_queue_name.inc"

