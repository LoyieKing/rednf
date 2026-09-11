// ============================================================================
// [SplitC 2026-09-11 batch2 · TU 迁移（ORIG GI 锚点）] 本文件由 Arad_InternalDispatcher.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN4ARAD10DISPATCHER23Arad_DatabaseDispatcherC2Ev
//   ORIG TU     ：Arad_DatabaseDispatcher.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "Arad_InternalDispatcher.h"
#include "CStreamGuard.h"
#include "PacketGuard.h"
#include "GameEnums.h"
#include <algorithm>
#include "GlobalData.h"
#include "ServiceRestrictManager.h"
#include "AvatarConvertServer.h"
#include "EmblemCompoundServer.h"
#include "AvatarRechargeServer.h"
#include "AvatarFixedHiddenOptionServer.h"
#include "Arad_DataManager.h"       // ARAD::Singleton<T>::Get
#include "MySQL.h"
#include "AvatarCoin.h"
#include "DBMgr.h"
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "LogManager.h"
#include "Stream.h"
#include "MoreTypes.h"      // arad::Sig* 线格式结构
#include "StreamPool.h"    // GlobalData::s_stream_pool->Acquire（LoadRewardEventItem/LoadServerState）
#include "MsgQueueMgr.h"   // MsgQueueMgr::DISPATCH_Q / put（LoadRewardEventItem/LoadServerState）
#include "ServiceRestrictManager.h"
#include "AvatarConvertServer.h"
#include "EmblemCompoundServer.h"
#include "AvatarRechargeServer.h"
#include "AvatarFixedHiddenOptionServer.h"
#include "Arad_DataManager.h"       // ARAD::Singleton<T>::Get

namespace ARAD
{
namespace DISPATCHER
{
#include "Arad_DatabaseDispatcher/Arad_DatabaseDispatcher.inc"
}  // namespace DISPATCHER
}  // namespace ARAD
