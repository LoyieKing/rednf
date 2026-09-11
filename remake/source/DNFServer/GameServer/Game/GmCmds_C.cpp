// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GmCmds_C/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// GmCmds_C.cpp — game_master 命令体系批次 C（df_game_r ORIG T 符号族还原）。
// 负责类（10）：
//   CCraneNeedMaterial        ctor 0x082a86aa / execute 0x084b0626
//   CCreateDisjointStoreCmd   ctor 0x082a8108 / execute 0x084a9720
//   CDeleteAccountCargo       ctor 0x082a84ce / execute 0x084abdb4
//   CDeleteGuild              ctor 0x082a83e0 / execute 0x084ab29c
//   DungeonDifficulty         ctor 0x082a8892 / parse 0x084b2110 / execute 0x084b2152
//   CEndEvent                 ctor 0x082a845e / parse 0x084ab914 / execute 0x084ab94c
//   CEnduranceCmd             ctor 0x082a83fc / parse 0x084aa73a / execute 0x084aa858
//   CGrowhWeaponEvent         ctor 0x082a88e6 / parse 0x084b28d6 / execute 0x084b293c
//   CGuildDebug               ctor 0x082a8592 / parse 0x084ac720 / execute 0x084ac76c
//   CGuildExpBookDelete       ctor 0x082a8506 / execute 0x084abf2c
// 对照 docs/class_func_reports/game_master__<Class>/{parse,execute}.md。
//
// 头文件占位字段失配说明：GameMasterCmd.h 对本批各类统一给出 m_str0(string@+0x08)
// 等占位名，但 ORIG 真实布局按各类不同：
//   CEndEvent          +0x08 int（事件代码）
//   CEnduranceCmd      +0x08 int（背包槽位）+0x0c unsigned short（耐久值）
//   CGrowhWeaponEvent  +0x08 byte（"gw" 重置标记）+0x0c int（时计数值）
//   DungeonDifficulty  +0x08 byte（难度 1..3）
//   CGuildDebug        +0x08 int +0x0c int +0x10 unsigned int（m_f1 占位即此字段）
//   （gcc4.4 32 位 std::string 仅 4 字节，故占位 string 与上述标量布局同尺寸。）
// 本 TU 一律按偏移访问真实标量字段，不向占位 std::string 写入。
#include "GameMasterCmd.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "GameWorld.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CInventory.h"
#include "CServerProxyMgr.h"
#include "Inven_Item.h"
#include "CDungeon.h"
#include "CDungeonClear.h"
#include "CDataManager.h"
#include "CExpertJob.h"
#include "CGuildServerProxy.h"
#include "CMonitorServerProxy.h"
#include "STItemScript.h"
#include "Packet_Monitor_Notice_Guild_Dismiss.h"
#include "Packet_Monitor_Event_End.h"
#include "Packet_Guild_Debug.h"
#include "CEnvironment.h"
#include "DB_EventGrowthWeaponReset.h"            // makeRequest（桥清零）
#include "DB_EventUpdateGrowthWeaponTimepiece.h"  // makeRequest（桥清零）

#include <stdlib.h>

namespace game_master
{

// ========================================================================
// CCraneNeedMaterial —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a86aa：基类 vptr → 自身 vptr。

#include "GmCmds_C/execute.inc"


// ========================================================================
// CCreateDisjointStoreCmd —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a8108。

#include "GmCmds_C/execute_1.inc"


// ========================================================================
// CDeleteAccountCargo —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a84ce。

#include "GmCmds_C/execute_2.inc"


// ========================================================================
// CDeleteGuild —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a83e0。

#include "GmCmds_C/execute_3.inc"


// ========================================================================
// DungeonDifficulty —— sizeof=0xc；+0x08 byte 难度（占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a8892。

#include "GmCmds_C/parse.inc"


#include "GmCmds_C/execute_4.inc"


// ========================================================================
// CEndEvent —— sizeof=0xc；+0x08 int 事件代码（占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a845e。

#include "GmCmds_C/parse_1.inc"


#include "GmCmds_C/execute_5.inc"


// ========================================================================
// CEnduranceCmd —— sizeof=0x10；+0x08 int 槽位、+0x0c unsigned short 耐久
// （占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a83fc。

#include "GmCmds_C/parse_2.inc"


#include "GmCmds_C/execute_6.inc"


// ========================================================================
// CGrowhWeaponEvent —— sizeof=0x10；+0x08 byte "gw" 重置标记、+0x0c int 时计值
// （占位 m_str0 失配，偏移访问）。
// ========================================================================

// ORIG ctor 0x082a88e6。

#include "GmCmds_C/parse_3.inc"


#include "GmCmds_C/execute_7.inc"


// ========================================================================
// CGuildDebug —— sizeof=0x14；+0x08/+0x0c int 参数、+0x10 unsigned int（m_f1 占位）。
// ========================================================================

// ORIG ctor 0x082a8592。

#include "GmCmds_C/parse_4.inc"


#include "GmCmds_C/execute_8.inc"


// ========================================================================
// CGuildExpBookDelete —— sizeof=0x8，无数据字段。
// ========================================================================

// ORIG ctor 0x082a8506。

#include "GmCmds_C/execute_9.inc"


} // namespace game_master