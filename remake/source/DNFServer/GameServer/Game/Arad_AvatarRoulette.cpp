// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_AvatarRoulette/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// Arad_AvatarRoulette — 原始工程 localjapan/Arad_AvatarRoulette.cpp 的复刻 TU。
// 收录 AvatarCoin 的「使用侧」方法（ORIG 参照 docs/class_func_reports/AvatarCoin/）：
//   Use（前批次）/ SaveToDB / SendSyncPacket / GetPackage / HistoryLog::AddLog
//   以及 CUser::_GetSaveInvenBase（SaveToDB 的内联依赖，ORIG 独立 T 符号）。
// 另含 AvatarRouletteServer::isAvatarPottery（0x817fb0a）。
// 注意：Use 调用 IsUsable 必须跨 TU（PLT 调用 → ORIG 帧 sub $0x18），
// 与 AvatarCoin.cpp 保持分 TU。

#include "AvatarCoin.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"               // m_avatarCoin 真实成员访问（_GetSaveInvenBase）
#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "SigTypes.h"
#include "LogManager.h"
#include "PacketGuard.h"
#include "CDataManager.h"             // DataManagerInitDepends.h 前置依赖
#include "DataManagerInitDepends.h"   // AvatarRoulette 权威布局（成员声明）
#include <cstring>

// isAvatarPottery 为 AvatarRouletteServer 成员（ORIG T 0x0817fb0a，定义
// CtorProducers_1.cpp；本 TU 经 DataManagerInitDepends.h 类声明调用）。

#include "Arad_AvatarRoulette/getAvatarList.inc"


#include "Arad_AvatarRoulette/getStikerList.inc"


#include "Arad_AvatarRoulette/Use.inc"


// ---- AvatarCoin 对象位于角色背包基址 +0x658（ORIG 全局约定） ----
// [b28c-lane17] 删除 coinOf static inline 助手（P49：-O0 下仍产真实 call 符号；
// ORIG 各调用点均为内联 `getCurCharacInvenW(); add $0x658` 直落 receiver）。

#include "Arad_AvatarRoulette/_GetSaveInvenBase.inc"


#include "Arad_AvatarRoulette/SaveToDB.inc"


#include "Arad_AvatarRoulette/SendSyncPacket.inc"


#include "Arad_AvatarRoulette/AddLog.inc"


#include "Arad_AvatarRoulette/GetPackage.inc"


#include "Arad_AvatarRoulette/BuyProcess.inc"


