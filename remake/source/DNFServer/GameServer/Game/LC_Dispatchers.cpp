// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 LC_Dispatchers/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 还原 —— LinkClosureC 批次：Dispatcher_MouseRegister 三方法 +
// Dispatcher_RegisterToBlackList::process。
// ORIG 依据（docs/class_func_reports/）：
//   Dispatcher_MouseRegister/process.md  T 0x08263224
//   Dispatcher_MouseRegister/read.md     T 0x08263210
//   Dispatcher_MouseRegister/send.md     T 0x082632b4
//   Dispatcher_RegisterToBlackList/process.md T 0x081cdc9a
// 附加依赖 producer：Packet_Register_To_BlackList C1 W 0x0822bb2e
//   （objdump 逐指令：PacketHeader(0x5dc,0x2c) + memset(this+0xe,0,0x1e)）。
#include "Dispatchers.h"

#include <cstring>

#include "CEnvironment.h"       // G_CEnvironment / get_server_group
#include "CGameManager.h"
#include "CUser.h"              // get_acc_id / GetServerGroup
#include "CompoundEmblemDeps.h" // LineFunc 权威声明
#include "GlobalData.h"         // s_monitor_proxy_mgr / s_guild_proxy_mgr
#include "CMonitorServerProxy.h"
#include "PacketHeader.h"      // 权威 PacketHeader（0xa packed）
#include "CServerProxyMgr.h"

// ---------------------------------------------------------------------------
// Packet_Register_To_BlackList（0x2c；ctor W 0x0822bb2e；本 TU 唯一定义点）。
// +0x00 PacketHeader(0xa packed) / +0x0a accId / +0x0e char name[0x1e]。
class Packet_Register_To_BlackList
{
public:
    Packet_Register_To_BlackList()
        : m_header(0x5dc, 0x2c)
    {
        memset(m_name, 0, sizeof(m_name));
    }

    PacketHeader m_header;    // +0x00
    unsigned int m_accId;     // +0x0a
    char m_name[0x1e];        // +0x0e
} __attribute__((packed));

#include "LC_Dispatchers/process.inc"


#include "LC_Dispatchers/read.inc"


#include "LC_Dispatchers/send.inc"


#include "LC_Dispatchers/process_1.inc"

