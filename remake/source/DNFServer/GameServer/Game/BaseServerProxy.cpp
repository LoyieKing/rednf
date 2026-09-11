// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 BaseServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "LogManager.h"
// df_game_r 还原 —— BaseServerProxy（E1 批次）。
// 参照 docs/class_func_reports/BaseServerProxy.md。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN15BaseServerProxyC2EPciS0_`（ORIG 0x82fdd4f）；ctor 已是首强符号。
#include <iostream>

#include "BaseServerProxy.h"

#include <errno.h>
#include <string.h>

#include "GameBase.h"

// ============================================================================
// [b28d-55] 本 TU 判词表（verdict = game_func_report --no-compile，fresh .o）：
//   InitTcp 0x082fd47c  DIFF(171/170，改前 171/167) P167 CLEAR(25/25)；本 TU 经
//     dnf_game_cxx.sh 路由 C6446R（4.4.6 后端，N7 敏感族翻正）。
//     [N4 2026-09-11] 手法：ORIG 0x082fd537 `mov %eax,-0x54(%ebp)` 后
//     0x082fd53a 才 `mov -0x1c(%ebp),%edi` ⇒ P0「ORIG 有槽往返必具名局部」；
//     两分支 `*__errno_location()` 第 3 实参改 `errnoVal = *__errno_location()`，
//     并在 `int ret;` 前声明 `int errnoVal;` ⇒ our 167→170，gap +4→+1
//     （四数不劣：AE=11/IDENT=1/DIFF=2 不变）。
//     残余 = ORIG 独有 `push %edi` + `mov -0x1c(%ebp),%edi`（ret 跨调用保活）
//     与帧 0x6c（我方 0x60）、槽 -0x4c/-0x1c/-0x54（我方 -0x3c/-0xc/-0x10）
//     ⇒ N7 寄存器分配/帧跨版本形，源码不可控，封顶。
// ============================================================================

#include "BaseServerProxy/BaseServerProxy.inc"


#include "BaseServerProxy/dtor_BaseServerProxy.inc"


#include "BaseServerProxy/OnDisconnect.inc"


#include "BaseServerProxy/Init.inc"


#include "BaseServerProxy/InitTcp.inc"


#include "BaseServerProxy/Disconnect.inc"


#include "BaseServerProxy/SendTcpPacket.inc"


#include "BaseServerProxy/OnTcpRecv.inc"


#include "BaseServerProxy/OnTcpSend.inc"


#include "BaseServerProxy/OnTcpError.inc"

