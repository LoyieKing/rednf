// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 W6_AssaultPlaceDepsB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// W6_AssaultPlaceDepsB.cpp —— Wave6 后半组 30 个 pvp_assault::CAssaultPlace
// ORIG-only 方法（/tmp/only_orig/pvp_assault.txt 54-83 行）。
// 全部语义取自 docs/class_func_reports/pvp_assault__CAssaultPlace/ 对应报告，
// 汇编为准；控制流 / 局部变量 / 调用序 / 常量逐字节转写。
#include "CAssaultMgr.h"

#include <string.h>

#include "CPowerManager.h"          // IsPowerWarEventOn / GetPowerWarLog /
                                    // IncreaePowerWarPointInChannel / ENUM_POWER_SIDE_TYPE
#include "CUser.h"                  // Send / GetParty / get_area / get_posX /
                                    // get_unique_id / get_acc_id / get_charac_no /
                                    // getHackAnalyzer / getPowerSide / cUserHistoryLog
#include "CUserCharacInfo.h"
#include "CParty.h"                 // send_to_party / get_user / get_member_count /
                                    // SetAssaultState / straight-victories 族
#include "PacketGuard.h"            // PacketGuard : InterfacePacketBuf
#include "GlobalData.h"             // s_power_manager / s_systemTime_ / s_guild_proxy_mgr
#include "GameWorld.h"              // G_GameWorld / send_to_vill / send_all / get_user_id_list
#include "CGuildServerProxy.h"      // SendPowerWarPoint / CServerProxyMgr 使用点
#include "CServerProxyMgr.h"        // GetServerProxy 完整类型
#include "CHackAnalyzer.h"          // WongWork::CHackAnalyzer::addServerHackCnt
#include "MSG_DIE_ASSAULT_PLAYER.h"
#include "LogManager.h"
#include "GameTypes.h"              // cMyTrace

#include "Packet_Guild_Change_Power_War_Point.h"   // 本批新建权威头（广播过）

// W7_BuildHarmonize 去重：pvp_assault::GetDirectionByTeam（ORIG T 0x82e564c，
// team 0→6、1→4、其他→0）唯一 owner 为 CAssaultMgr.cpp。
namespace pvp_assault { int GetDirectionByTeam(ENUM_ASSAULT_TEAM team); }

namespace {

}  // namespace

using pvp_assault::CAssaulter;

namespace pvp_assault
{

// ---------------------------------------------------------------------------
// 包构造 helper 组
// ---------------------------------------------------------------------------

#include "W6_AssaultPlaceDepsB/_MakeCompleteLoad.inc"


#include "W6_AssaultPlaceDepsB/_MakeStartAssault.inc"


#include "W6_AssaultPlaceDepsB/_MakeDieAssaultPlayer.inc"


#include "W6_AssaultPlaceDepsB/_MakeRevivalAssaultPlayer.inc"


#include "W6_AssaultPlaceDepsB/_MakeMoveAssaultPlace.inc"


// ---------------------------------------------------------------------------
// 发送组
// ---------------------------------------------------------------------------

#include "W6_AssaultPlaceDepsB/_SendPacketToVill.inc"


#include "W6_AssaultPlaceDepsB/_SendUserMiniInfo.inc"


#include "W6_AssaultPlaceDepsB/_SendUserUdpIpPort.inc"


#include "W6_AssaultPlaceDepsB/_MakeUserUdpIpPort.inc"


#include "W6_AssaultPlaceDepsB/_SendVictoryBonus.inc"


#include "W6_AssaultPlaceDepsB/_SendPowerWarAssaultResultToUser.inc"


#include "W6_AssaultPlaceDepsB/_MoveAssaultPlace.inc"


// ---------------------------------------------------------------------------
// 连接 P2P 统计组
// ---------------------------------------------------------------------------

#include "W6_AssaultPlaceDepsB/_IsCannotConnectP2P.inc"


#include "W6_AssaultPlaceDepsB/_GetConnectP2PPoint.inc"


#include "W6_AssaultPlaceDepsB/_GetConnectP2PMaxMinPoint.inc"


#include "W6_AssaultPlaceDepsB/_ClearConnectP2PAbsolute.inc"


#include "W6_AssaultPlaceDepsB/_ClearConnectP2PRelative.inc"


// ---------------------------------------------------------------------------
// PowerWar 组
// ---------------------------------------------------------------------------

#include "W6_AssaultPlaceDepsB/_PowerWarAssaultCheckIn.inc"


#include "W6_AssaultPlaceDepsB/_CheckPowerWarInPlayer.inc"


#include "W6_AssaultPlaceDepsB/_SendStraightVictories.inc"


#include "W6_AssaultPlaceDepsB/_SendPowerWarPointToGuildServer.inc"


// ORIG T 0x82ec558/0x82eceb2 直读补充：_SendStraightVictories 末尾 endp。
// （注：上面的 EndPvP reason 参数按 ORIG mangled 为字面 1（0x082ed187 前 mov $1）。）

// ---------------------------------------------------------------------------
// TimeoutCompleteLoad
// ---------------------------------------------------------------------------

#include "W6_AssaultPlaceDepsB/TimeoutCompleteLoad.inc"



// ---------------------------------------------------------------------------
// ChangeHp
// ---------------------------------------------------------------------------

#include "W6_AssaultPlaceDepsB/ChangeHp.inc"


// ---------------------------------------------------------------------------
// DieUser 双版本
// ---------------------------------------------------------------------------

#include "W6_AssaultPlaceDepsB/DieUser.inc"


#include "W6_AssaultPlaceDepsB/DieUser_1.inc"


// ---------------------------------------------------------------------------
// MakeTeam / 组队操作组
// ---------------------------------------------------------------------------

#include "W6_AssaultPlaceDepsB/MakeTeam.inc"


#include "W6_AssaultPlaceDepsB/_GetTeamCountAliveUser.inc"


#include "W6_AssaultPlaceDepsB/_AddUser.inc"


#include "W6_AssaultPlaceDepsB/_AddTeam.inc"


#include "W6_AssaultPlaceDepsB/_MoveUser.inc"


}  // namespace pvp_assault
