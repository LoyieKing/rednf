// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFairMatchRooms/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CFairMatch.h"

#include <algorithm>    // std::swap
#include <cstddef>

#include "PvP_Room.h"              // PvP_Room 全接口/CGameManager
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "CMissionList_Charac.h"
#include "MissionTypes.h"
#include "CFairPvPScore.h"
#include "CSwitchLog.h"
#include "GameTypes.h"             // cMyTrace
#include "LogManager.h"
#include "CDataManager.h"
#include "ServerParameterScript.h" // m_schoolMatchSeal / m_schoolMatchDailyPlayLimit
#include "GameWorld.h"
#include "TimerQueue.h"
#include "TimerDispatcher.h"
#include "CRelayBattleMgr.h"

using fair_pvp::CFairMatch;
using fair_pvp::TeamInfoThird;
using fair_pvp::PlayerInfoThird;

// CEloRating / PvPGlobalEnvironmentVariable 权威声明在 OnlinePreliminary.h
// （OnlinePreliminary 批维护；calc 定义在其 OnlinePreliminaryMatch.cpp）
#include "OnlinePreliminary.h"
#include "CGameManager.h"

// SPS 训练印章配置块视图（m_serverParameter 位于 CDataManager+0x68；
// ORIG 读 dm+0x49d8..0x49e0 = SPS+0x4970..0x497a；宏内联——P49）
#define FM_SPS() (G_CDataManager()->m_serverParameter)
// [dedup] moved to CFairMatch.cpp
// [dedup] moved to CFairMatch.cpp
int CFairMatch::nextAuthValue_ = 0;                       // B 0x0943e150
// [dedup] moved to CFairMatch.cpp

// TeamInfoThird 首字节视图（ORIG 以 *(char*)(this+team*0x48+0x1c) 判在座；
// 宏内联——P49：-O0 下 static inline helper 产真实 call 符号）
#define FM_TeamByte0(t) (*(reinterpret_cast<unsigned char*>(&(t))))


// ===================================================================== //
// 小函数组（ORIG 逐指令转写）
// ===================================================================== //

#include "CFairMatchRooms/_CheckSameTeam.inc"


#include "CFairMatchRooms/CheckUnusableEquipmentInUser.inc"


#include "CFairMatchRooms/OnGiveup.inc"


#include "CFairMatchRooms/OnWaitRematch.inc"


#include "CFairMatchRooms/GetEndPvPResultTimeout.inc"


#include "CFairMatchRooms/GetNextMatch.inc"

#include "CFairMatchRooms/SetNextMatch.inc"

#include "CFairMatchRooms/_SetEnterCharacNo.inc"


#include "CFairMatchRooms/_GetRating.inc"


// ===================================================================== //
// 检查组
// ===================================================================== //

#include "CFairMatchRooms/_CheckObserver.inc"


#include "CFairMatchRooms/CheckJoinable.inc"


#include "CFairMatchRooms/OnReady.inc"


#include "CFairMatchRooms/_CheckMatchable.inc"


#include "CFairMatchRooms/_CheckRematchable.inc"


#include "CFairMatchRooms/CheckMatchable.inc"


#include "CFairMatchRooms/GetGap.inc"


#include "CFairMatchRooms/GetUser.inc"


#include "CFairMatchRooms/GetPlayerCount.inc"


#include "CFairMatchRooms/_FindTeam.inc"


// ===================================================================== //
// 房间生命周期组
// ===================================================================== //

#include "CFairMatchRooms/OnCreateRoom.inc"


#include "CFairMatchRooms/OnDestroyRoom.inc"


#include "CFairMatchRooms/OnJoinRoom.inc"


#include "CFairMatchRooms/_SendMatchStatePacket.inc"


#include "CFairMatchRooms/_BackupRoomInfo.inc"


#include "CFairMatchRooms/_RegisterWaitingRoom.inc"


#include "CFairMatchRooms/_ChangeTeamLeader.inc"


#include "CFairMatchRooms/OnLeaveRoom.inc"


#include "CFairMatchRooms/OnUnregisterMatch.inc"


#include "CFairMatchRooms/OnRegisterMatch.inc"


// ===================================================================== //
// 对局进行 / 换房组
// ===================================================================== //

#include "CFairMatchRooms/_CheckMovableRoom.inc"


#include "CFairMatchRooms/_PullOutRoom.inc"


#include "CFairMatchRooms/_PutInRoom.inc"


#include "CFairMatchRooms/_MakeRoom.inc"


#include "CFairMatchRooms/_MoveRoom.inc"


#include "CFairMatchRooms/OnMatchRoom.inc"


// ===================================================================== //
// 结算 / 重赛 / 奖励组
// ===================================================================== //

#include "CFairMatchRooms/_ReturnTeam.inc"


#include "CFairMatchRooms/_PullOutRoomTeam.inc"


#include "CFairMatchRooms/OnWalkOut.inc"


#include "CFairMatchRooms/CheckRematch.inc"


#include "CFairMatchRooms/OnEndFight.inc"


#include "CFairMatchRooms/OnReward.inc"


#include "CFairMatchRooms/UpdateScore.inc"


#include "CFairMatchRooms/GiveTrainingSeal.inc"


#include "CFairMatchRooms/OnRematchTimeout.inc"


#include "CFairMatchRooms/OnAskRematch.inc"


#include "CFairMatchRooms/OnEndPvPResult.inc"


// ===================================================================== //
// 载入 / P2P 组
// ===================================================================== //

#include "CFairMatchRooms/OnCompleteLoad.inc"


#include "CFairMatchRooms/OnConnectP2PTimeout.inc"


#include "CFairMatchRooms/OnConnectP2P.inc"



#include "CFairMatchRooms/_UpdateRating.inc"

