#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 MakePVPRoom/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CDataManager.h"        // G_CDataManager / GetMaxGradePvPChannel / m_serverParameter
#include "CGameManager.h"        // G_CGameManager / GetPvp / CheckOutParty / PutPvp
#include "GameWorld.h"           // G_GameWorld / GetChannelType / IsPVPChannel / send_all / goto_pvp
#include "PvP_Room.h"            // create_room / get_index / make_room_info
#include "CParty.h"              // CParty::send_invite_pvp
#include "CFairPvPScore.h"       // fair_pvp::CFairPvPScore::GetGiveItem
#include "PacketGuard.h"         // PacketGuard : InterfacePacketBuf（put_header/put_byte/put_short/finalize/clear）
#include "CSwitchLog.h"          // CSwitchLog（ORIG 854fd3c/854fd64）
#include "LogManager.h"          // LogManager::logFormat
#include "GameEnums.h"           // ENUM_CMDPACKET / PVP_BATTLE_MODE / ENUM_CHARAC_EXPAND_TYPE

// MSG_MAKE_PVP_ROOM：权威定义当前在 DisPatcherCore.cpp wave1 声明区（sizeof 0x3d，
// dispatch_sig memset 0x3d 实证；DisPatcherCore.h 仅前置声明）。此处按同一布局
#pragma pack(push, 1)
struct MSG_MAKE_PVP_ROOM
{
    char m_pad0[0xd];        // +0x00..0x0c
    char m_field0d;          // +0x0d 门类型（<0 失败）
    char m_name[0x1e];       // +0x0e..0x2b 房名
    short m_field2c;         // +0x2c
    char m_field2e;          // +0x2e 模式门
    int m_field2f;           // +0x2f 密码长（<=8）
    char m_field33[9];       // +0x33..0x3b 密码/校验缓冲
    char m_battleMode;       // +0x3c battleMode
};
#pragma pack(pop)

#include "MakePVPRoom/MakePVPRoom.inc"

