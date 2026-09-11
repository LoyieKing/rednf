// ============================================================================
// [SplitC 2026-09-11 batch2 · TU 迁移（ORIG GI 锚点）] 本文件由 Arad_ItemBonus.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN4ARAD26CeraShopPurcahseCountBonus10LoadScriptEv
//   ORIG TU     ：CeraShopPurcahseCountBonus.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include <math.h>
#include <string.h>
#include <algorithm>
#include <utility>
#include "Arad_ItemBonus.h"
#include "Arad_DataManager.h"          // Singleton<Arad_DataManager>/getPurchaseLimitItem
#include "CMailBoxHelper.h"            // WongWork::CMailBoxHelper::ReqDBSendNewMail
#include "CDataManager.h"
#include "CCeraShop.h"
#include "CInventory.h"
#include "CItemList.h"
#include "CUserCharacInfo.h"
#include "GameTypes.h"                 // cMyTrace / Inven_Item 完整定义链
#include "GlobalData.h"
#include "Inven_Item.h"
#include "MsgQueueMgr.h"
#include "PacketGuard.h"
#include "RDARScriptStringManager.h"
#include "GameWorld.h"                 // G_GameWorld / send_to_area
#include "DNFLexWrapper.h"             // ScanType / ScanInt
#include "NpcScript.h"                 // loadRDARScriptFile
#include "CUser.h"
#include "CSystemTime.h"

namespace ARAD
{
#include "CeraShopPurcahseCountBonus/LoadScript.inc"
}  // namespace ARAD
