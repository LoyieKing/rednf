// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCommunityServerProxy/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— CCommunityServerProxy（独占 OrigCommPx 批）。
// 依据 docs/class_func_reports/CCommunityServerProxy/*.md 与 objdump 转写
// （OnTcpRecv/SendTcpPacket 报告缺失，按 §3.2 nm 定址后 objdump 区间转写）。
#include <errno.h>
#include <string.h>   // memcpy / strerror

#include "GameTypes.h"   // cMyTrace
#include "CCommunityServerProxy.h"
#include "CEnvironment.h"   // G_CEnvironment / get_channel_no
#include "CUser.h"          // CUser::get_acc_id/GetUID/GetServerGroup/getSex
#include "Packet_Community_Login.h"
#include "Packet_Community_Logout.h"
#include "Packet_Request_Add_PvP_Buddy.h"
#include "Packet_Response_Add_PvP_Buddy.h"
#include "Packet_Request_Remove_PvP_Buddy.h"

#include "CCommunityServerProxy/CCommunityServerProxy.inc"


#include "CCommunityServerProxy/Init.inc"


#include "CCommunityServerProxy/InitTcp.inc"


#include "CCommunityServerProxy/Disconnect.inc"


#include "CCommunityServerProxy/SendTcpPacket.inc"


#include "CCommunityServerProxy/OnTcpRecv.inc"


#include "CCommunityServerProxy/OnTcpSend.inc"


#include "CCommunityServerProxy/OnTcpError.inc"


#include "CCommunityServerProxy/Parsing.inc"


#include "CCommunityServerProxy/SendLogin.inc"


#include "CCommunityServerProxy/SendLogout.inc"


// ---- StubS3A_deps 批：Send*PvPBuddy（ORIG 逐指令转写）----
// 通用形态：栈上构造对应包（ctor 预填 header+len+清零），填
// +0x0a accId / +0x0e characNo / +0x12 flag，memcpy(name, nameLen) 到 +0x13，
// SendTcpPacket(pkt, *(ushort*)(pkt+2)=packetSize)。

#include "CCommunityServerProxy/SendReqAddPvPBuddy.inc"


#include "CCommunityServerProxy/SendResAddPvPBuddy.inc"


#include "CCommunityServerProxy/SendRemovePvPBuddy.inc"

