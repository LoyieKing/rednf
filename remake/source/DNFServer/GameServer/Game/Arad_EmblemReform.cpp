// ============================================================================
// [SplitC 2026-09-11 batch3 · TU 迁移（ORIG GI 锚点）] 本文件由 AvatarRechargeServer.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN4ARAD6SCRIPT14AvatarRechargeC2Ev
//   ORIG TU     ：Arad_EmblemReform.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "AvatarRechargeServer.h"
#include "Arad_InternalDispatcher.h"  // ARAD::DISPATCHER::make_internal_stream_jpn
#include <cstdio>
#include <string>
#include <string.h>
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "CStreamGuard.h"
#include "GameTypes.h"        // cMyTrace
#include "MoreTypes.h"        // arad::SigArad*
#include "DNFLexWrapper.h"    // loadRDARScriptFile / unloadRDARScriptFile / Scan*
#include "CUser.h"            // CUser（含 eSendTarget/get_state/SendPacket 等）
#include "CInventory.h"       // CInventory::GetInvenRef/delete_item/GetAvatarItemMgrR
#include "CAvatarItemMgr.h"   // WongWork::CAvatarItemMgr::getJewelSocketData
#include "CParty.h"           // CParty::IsUseFatigueDungeon/CheckMapClear
#include "Inven_Item.h"       // Inven_Item::isEmpty/m_addInfo/m_addInfo2/m_fieldSplit
#include "GamePODTypes.h"     // stAvatarEmblemInfo_t
#include "PacketGuard.h"      // PacketGuard（InterfacePacketBuf put_*/finalize）
#include "LogManager.h"       // LogManager::logFormat

#include "Arad_EmblemReform/AvatarRecharge.inc"
