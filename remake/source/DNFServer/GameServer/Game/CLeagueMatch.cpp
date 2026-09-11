// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CLeagueMatch/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CLeagueMatch.h"
#include <algorithm>
#include <string.h>

#include "CUser.h"
#include "CUserCharacInfo.h"
#include "PvP_Room.h"
#include "CGameManager.h"
#include "GameWorld.h"
#include "GlobalData.h"
#include "InterfacePacketBuf.h"
#include "PacketGuard.h"
#include "LogManager.h"
#include "GameTypes.h"                  // cMyTrace
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "CSchoolServerProxy.h"
#include "SchoolMatchEnums.h"           // ENUM_ELO_RATING_SYSTEM
#include "OnlinePreliminary.h"          // CEloRating / PvPGlobalEnvironmentVariable
#include "Packet_PvPResult.h"           // 结果包 0x80 载荷具名布局（shared/packet/include）
#include "RDARScriptStringManager.h"
#include "TimerDispatcher.h"            // TimerStartMatching
#include "CInventory.h"                 // OS_API::snprintf
// [SweepLeagueMatch4·判据⑪/㉔] ORIG MatchingSystem.cpp 归属：本 TU 定义
// g_schoolMgr / g_guildAgitMgr（static_init 构造点实证；定义由 CGameManager.cpp 迁入）。
#include "CSchoolMgr.h"                 // g_schoolMgr（ORIG B 0x0943e080 / C1 0x0856f206）
#include "CGuildAgitManager.h"          // g_guildAgitMgr（ORIG B 0x0943e0e0 / C1 0x0856f2c6）

// GetGap 排位增量表：ORIG 各 TU 本地 _ZL13g_RankingIncr（16 字节，
// df_game_r nm 显示 15 份内容一致）＝ {1.0f, 1.0f, 1.0f, 1.0f}。
// [SweepLeagueMatch2·P204③] 文件级 static（非匿名 namespace）：后者产
// `_ZN12_GLOBAL__N_1L13g_RankingIncrE`，与 ORIG `_ZL13g_RankingIncr` 不符。
static const float g_RankingIncr[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

// [SweepLeagueMatch4·判据⑪ P327 GI 键名 + 判据㉔ 跨 TU 定义迁移]
// ORIG 本 TU（orig_tu_names.tsv:354 标注 MatchingSystem.cpp）的 GI 键名 =
// `_GLOBAL__I__ZN28PvPGlobalEnvironmentVariable25school_match_daily_point_E`
// @0x08568636（size 0x1c）⇒ 该 TU 内**第一个**「强符号·外部链接·本 TU 定义」
// 实体即 school_match_daily_point_。ORIG .data 0x093abb60..0x093abb73 连续
// 五成员，初值读实 1/2/5/3/3（objdump -s -j .data 实证）。
// 迁移前这 5 个定义在 OnlinePreliminaryMatch.cpp，本 TU GI 键名误为
// `_GLOBAL__I__ZN12CLeagueMatch17pWaitingRoomList_E`（ORIG 无 ⇒ 计入 missing）。
// 顺序铁律：本组必须置于 pWaitingRoomList_ **之前**（P327：仅定义行序决定键名）。
int PvPGlobalEnvironmentVariable::school_match_daily_point_ = 1;
int PvPGlobalEnvironmentVariable::school_match_daily_point_weekends_ = 2;
int PvPGlobalEnvironmentVariable::school_match_fight_count_for_gain_point_ = 5;
int PvPGlobalEnvironmentVariable::school_match_elo_k_ = 3;
int PvPGlobalEnvironmentVariable::school_match_elo_k2_ = 3;

// [SweepLeagueMatch4] ORIG static_init @0x085685a0（size 0x96）构造序实证：
//   `_ZStL8__ioinit`(0x943e172, call 0x85685c0) →
//   `g_schoolMgr`(0x943e080, call 0x85685e2 = CSchoolMgr::C1 0x0856f206) →
//   `g_guildAgitMgr`(0x943e0e0, call 0x8568612 = CGuildAgitManager::C1 0x0856f2c6)，
// 各带 __cxa_atexit。两对象定义由 CGameManager.cpp 迁回本 TU（声明序 = 构造序）。
CSchoolMgr g_schoolMgr;
CGuildAgitManager g_guildAgitMgr;

CLeagueMatchRoomList* CLeagueMatch::pWaitingRoomList_ = 0;   // B 0x0943e064
int CLeagueMatch::nWaitingRoomListRefCount_ = 0;             // B 0x0943e068
int CLeagueMatch::nextAuthValue_ = 0;                        // B 0x0943e06c
bool CLeagueMatch::bMatchable_ = false;                      // B 0x0943e060
unsigned char CLeagueMatch::nBattleMode_ = 3;                // D 0x093abb75 初值 3
unsigned char CLeagueMatch::nPlayerCountPerOneTeam_ = 2;     // D 0x093abb74 初值 2

// ---------------------------------------------------------------------------
// PlayerInfo / TeamInfo（报告 TeamInfo/*.md + ORIG objdump 转写）
// ---------------------------------------------------------------------------

#include "CLeagueMatch/reset.inc"


// [SweepCLeagueMatch-2·判据㉓] PlayerInfo::reset（ORIG W 0x085687d4）已改类内
// inline 定义（CLeagueMatch.h），由使用点发射 W；本 TU 因 TeamInfo 族调用仍发射
// 该 W 副本。函数体逐条不变。

#include "CLeagueMatch/CompareRating.inc"


#include "CLeagueMatch/refresh.inc"


#include "CLeagueMatch/add.inc"


#include "CLeagueMatch/del.inc"


#include "CLeagueMatch/size.inc"


// ---------------------------------------------------------------------------
// 生命周期
// ---------------------------------------------------------------------------

#include "CLeagueMatch/CLeagueMatch.inc"


#include "CLeagueMatch/dtor_CLeagueMatch.inc"


// ---------------------------------------------------------------------------
// IMatch 覆盖
// ---------------------------------------------------------------------------

#include "CLeagueMatch/GetGap.inc"


#include "CLeagueMatch/GetUser.inc"


#include "CLeagueMatch/GetUserList.inc"


#include "CLeagueMatch/RequestMap.inc"


#include "CLeagueMatch/GetExtraWinPoint.inc"


#include "CLeagueMatch/OnCreateRoom.inc"


#include "CLeagueMatch/OnDestroyRoom.inc"


#include "CLeagueMatch/OnJoinRoom.inc"


#include "CLeagueMatch/OnLeaveRoom.inc"


#include "CLeagueMatch/OnRegisterMatch.inc"


#include "CLeagueMatch/OnMatchRoom.inc"


#include "CLeagueMatch/OnStartFight.inc"


#include "CLeagueMatch/OnEndFight.inc"


#include "CLeagueMatch/OnWalkOut.inc"


#include "CLeagueMatch/OnReady.inc"


#include "CLeagueMatch/OnReward.inc"


#include "CLeagueMatch/OnCompleteLoad.inc"


#include "CLeagueMatch/CheckJoinable.inc"


#include "CLeagueMatch/CheckSeat.inc"


#include "CLeagueMatch/CheckMatchable.inc"


// ---------------------------------------------------------------------------
// 非 IMatch 方法
// ---------------------------------------------------------------------------

#include "CLeagueMatch/SetMatchAble.inc"


#include "CLeagueMatch/SetPlayCountPerOneTeam.inc"


#include "CLeagueMatch/GetSchoolNo.inc"


// [SweepCLeagueMatch-2·判据㉓] GetIndentification(0x08568834 W) / GetBattleMode
// (0x084ed387 W) / SetBattleMode(0x082743a0 W) / GetPlayCountPerOneTeam
// (0x084ed37b W) 四个 ORIG 弱符号已改类内 inline 定义（CLeagueMatch.h），由
// 使用点 TU 发射 W 副本（P233/P204③）；本 TU 不再外联定义（原 T 绑定与 ORIG
// 的 W 不符）。函数体逐条不变，见头内注释。

#include "CLeagueMatch/_MakeRoom.inc"


#include "CLeagueMatch/_RegisterWaitingRoom.inc"


#include "CLeagueMatch/_BackupRoomInfo.inc"


#include "CLeagueMatch/_CheckMatchable.inc"


#include "CLeagueMatch/_PullOutRoom.inc"


#include "CLeagueMatch/_PutInRoom.inc"


#include "CLeagueMatch/_ReturnTeam.inc"


#include "CLeagueMatch/_SendMatchStatePacket.inc"


#include "CLeagueMatch/_getSameSchool.inc"


#include "CLeagueMatch/_GetOppositeTeam.inc"


#include "CLeagueMatch/_CheckSchool.inc"


#include "CLeagueMatch/addBadge.inc"

