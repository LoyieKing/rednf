// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUdpHandler/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CUdpHandler（UDP 句柄，ORIG 0x8483518 族，8 函数）。
// 参照 docs/class_func_reports/CUdpHandler.md；getErrno 同 TU（ORIG 0x8483e75）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN11CUdpHandlerC2Ev`（ORIG 0x8483e59）；键名符号需前移（首定义现为 getErrno）。
#include <iostream>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "CUdpHandler.h"
#include "GameTypes.h"

#include "CUdpHandler/CUdpHandler.inc"


#include "CUdpHandler/getErrno.inc"


#include "CUdpHandler/InetAddr.inc"


#include "CUdpHandler/InitClientSocket.inc"


#include "CUdpHandler/InitServerSocket.inc"


#include "CUdpHandler/RecvFromClient.inc"


#include "CUdpHandler/RecvFromServer.inc"


#include "CUdpHandler/SendToClient.inc"


#include "CUdpHandler/SendToServer.inc"

