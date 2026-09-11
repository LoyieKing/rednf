// ============================================================================
// [SplitC 2026-09-11 batch4 · TU 迁移（ORIG GI 锚点）] 本文件由 CSecretShop.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__Z22importSecretShopScriptP16SecretShopScriptPKc
//   ORIG TU     ：RDARScriptSecretShop.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include "Inven_Item.h"
#include "CDataManager.h"
#include "DNFLexWrapper.h"  // importSecretShopScript 的 Scan*/loadRDARScriptFile
#include "CInventory.h"
#include "CUser.h"
#include "CServerProxyMgr.h"
#include "CStatisticServerProxy.h"
#include "CCubeStatistic.h"
#include "GlobalData.h"
#include "SECRET_SHOP_DATA.h"
#include "CSecretShop.h"   // eBuyRule/CSecretShopStatistic/IBuyRule/CSecretShop 唯一声明点
#include "CParty.h"
#include "CDungeon.h"  // CheckLottery 遍历 m_dungeonMap
#include "PvPTypes.h"  // 权威全局 CMTRand（0x9c8，MT19937；实现见 CMTRand.cpp）
#include "Packet_Secret_Shop_Statistic.h"  // 权威包头（shared/packet/include）
#include "CGuildServerProxy.h"  // CEventBase/CEventManager 权威定义（LotteryItems 虚调 IsEventing）
#include <cstring>

#include "RDARScriptSecretShop/importSecretShopScript.inc"
