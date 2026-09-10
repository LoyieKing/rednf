// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 MakePVPRoom/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r MakePVPRoom 全局函数还原（2026-08-28）。
// ORIG T _Z11MakePVPRoomP5CUserR17MSG_MAKE_PVP_ROOM14ENUM_CMDPACKET15PVP_BATTLE_MODE
//   @0x081f793c（sz 0x701）。
// 返回类型：ORIG __PRETTY_FUNCTION__ @0x08bcff20 实证为 "bool MakePVPRoom(...)"
//   （mangled 不含返回类型，与 PVP_BATTLE_MODE 返回同符号；此处忠实写 bool）。
// 逐指令转写自 objdump 0x081f793c..0x081f803d；日志串原样：
//   0x8bc22a4 "pvp@log %s,%s, room(%d),%d,%d,%d"
//   0x8bc22c8 "create_room fail: PVP슬롯반환 ID[%d]\n"（CP949 字节）
//   0x8bc1464 "PacketDispatcher_Impl_1.cpp"（logFormat file 参）
// 调用方：DisPatcherCore.cpp::DisPatcher_MakePVP::dispatch_sig（cmd=0x35/0x9f）。
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

// ---------------------------------------------------------------------------
// b28d-solo-MakePVPRoom: verdict=AE（check_tu_routed.sh / compare_tu clean 口径；
//   本件无 class_func_reports 报告 ⇒ game_func_report --no-compile 的 DIFF 属 N124 假报）
//   修法：①真语义 bug —— create_room 第 6 实参 msg.m_field2e != 1 → == 1（ORIG 0x081f7ccf
//   `cmp $0x1,%al; sete %al`；旧写 setne 极性反，门类型 1 的房间会被当失败路径）；
//   ②P0 槽-局部律去 pvpScore/giveItem/giveLimit 具名局部（ORIG 无槽：GetGiveItem→%esi、
//   get_pvp_item_give 结果留 %eax 直比 setle）改全内联右值；
//   ③P227 全表达式临时链 `CSwitchLog(...)(...)` 复现 ORIG ctor 前右起求 varargs 并
//   get_index→%edi / getCurCharacName→%esi / get_acc_name→%ebx 跨 ctor 保活（两语句具名对象
//   会退化为 4.4 ctor 后求值）；④P139 签名串改 __PRETTY_FUNCTION__（双侧
//   `_ZZ11MakePVPRoom…E19__PRETTY_FUNCTION__` r 0x4e 全等，P187 硬闭合）。
//   读数：ndiff 393→0、insns 448/448、帧 0x8c 全等；p167 调用点 90/90 CLEAR（0 missing/0 extra）。
//   TU 残余 `_Z41__static_initialization_and_destruction_0ii` = N123 同名副本撞名假 DIFF。
// ---------------------------------------------------------------------------
#include "MakePVPRoom/MakePVPRoom.inc"
}
