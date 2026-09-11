// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CPowerManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <cstring>
#include <cstdio>
#include "CPowerManager.h"
#include "SigTypes.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CEnvironment.h"
#include "GlobalData.h"
#include "GameWorld.h"
#include "CGameManager.h"

#include "CGuildServerProxy.h"
#include "CServerProxyMgr.h"
#include "PacketGuard.h"
#include "CStreamGuard.h"
#include "MsgQueueMgr.h"
#include "CPrivateStoreMgr.h"
#include "CSharedServerMessageManager.h"
// [b28d-solo] SIG_LOAD_POWER_WAR_STATUE_INFO（0x80）载荷具名视图（P80/P99）：
// 偏移逐字段依 ORIG SetPowerWarUserStatueInfo 反汇编（0x0847ed4e 段）实证；
// +0x4d..+0x4f 为 ORIG 未访问空洞（显式 pad 表达布局，不参与写入）。
#pragma pack(push, 1)
struct PowerWarStatueView
{
    unsigned int m_characNo;   // +0x00
    char m_name[0x1e];         // +0x04
    char m_job;                // +0x22（与目标 m_rankings[].m_job 同型，P211 档③）
    char m_level;              // +0x23
    char m_grow;               // +0x24
    char m_guildName[0x28];    // +0x25
    char m_pad4d[3];           // +0x4d..+0x4f
    int m_guildId;             // +0x50
    int m_equipment[11];       // +0x54（0x2c）
};
#pragma pack(pop)
#include "CPowerManager/CPowerManager.inc"


#include "CPowerManager/IsPowerWarEventOn.inc"




#include "CPowerManager/GetPowerWarResponPenalty.inc"


#include "CPowerManager/GetPowerWarGhostTime.inc"



#include "CPowerManager/IncreasePowerUserCount.inc"


#include "CPowerManager/DecreasePowerUserCount.inc"


#include "CPowerManager/CheckPowerWarEnterLimit.inc"


#include "CPowerManager/GetWinnerSide.inc"


#include "CPowerManager/IsWinerSide.inc"


#include "CPowerManager/ProcessJoinPowerWar.inc"


#include "CPowerManager/Init.inc"


#include "CPowerManager/LoadPowerWarInfo.inc"


#include "CPowerManager/LoadRankerInfo.inc"

#include "CPowerManager/CheckCompleteDBLoadStatueInfo.inc"

#include "CPowerManager/SetPowerInfo.inc"


#include "CPowerManager/SetPowerWarStartTime.inc"


#include "CPowerManager/SetPowerWarUserRankingInfo.inc"




#include "CPowerManager/SetPowerWarUserStatueInfo.inc"


#include "CPowerManager/ClearMVPInfo.inc"


#include "CPowerManager/IsAbleToRevival.inc"


#include "CPowerManager/IncreaePowerWarPointInChannel.inc"


#include "CPowerManager/SendPowerWarUserStatueInfo.inc"


#include "CPowerManager/GetPowerUserCount.inc"


#include "CPowerManager/GetPowerWarPoint.inc"


#include "CPowerManager/GetPowerWarPointInChannel.inc"


#include "CPowerManager/GetRankingByCharacNo.inc"


#include "CPowerManager/SetPowerWarPoint.inc"

