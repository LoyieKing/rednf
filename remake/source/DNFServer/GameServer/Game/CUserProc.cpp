#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUserProc/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <stdio.h>
#include <string.h>

#include "CUserProc.h"   // APSystem::CUserProc / _ActionGroupIndex（唯一声明点）
#include "CUser.h"       // CUser 权威定义（m_actionPoint @+0x8d264）
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"

// [b28c-4] ORIG 0x081224ac 载荷为 5 字节紧凑槽（flag@-0x19、itemIdx@-0x18）
// ⇒ pack(1) 结构体（探针 /tmp/l4/reward.cpp g2 与 ORIG 逐条全等）。
struct UserProcRewardPayload
{
    char flag;
    int itemIdx;
} __attribute__((packed));

#include "CUserProc/ClearActionAndSendtoUser.inc"


#include "CUserProc/SendActionClear.inc"


#include "CUserProc/SendActionListState.inc"


#include "CUserProc/SendActionListState_1.inc"


#include "CUserProc/SendGetRewardItem.inc"


#include "CUserProc/SetTodayActionAndCheckMedalReward.inc"


#include "CUserProc/UpdateActionPoint.inc"


#include "CUserProc/GetTodayRewardAndSendPostal.inc"


#include "CUserProc/CheckAndSendToMessageAllUser.inc"


#include "CUserProc/SendToMessageAllUser.inc"


#include "CUserProc/GM_Reset.inc"


#include "CUserProc/GM_Set.inc"

