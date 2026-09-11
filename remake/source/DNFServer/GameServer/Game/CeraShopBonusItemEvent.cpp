// ============================================================================
// [SplitC 2026-09-11 batch2 · TU 迁移（ORIG GI 锚点）] 本文件由 CCeraShopAddRestrict.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN23CCeraShopBonusItemEventC2Ev
//   ORIG TU     ：CeraShopBonusItemEvent.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include <ctime>
#include <cstring>
#include <map>
#include <string>
#include "RDARScriptStringManager.h"
#include "CGameManager.h"
#include "CInventory.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "InterfacePacketBuf.h"
#include "Arad_DataManager.h"
#include "CItemLimitEdition.h"
#include "CEnvironment.h"   // G_CEnvironment()（GetCalcDay 读 env+0x37c）
#include "LogManager.h"
#include "CCeraShopBonusItemEvent.h"   // 权威声明（P104 ODR 收敛，2026-08-29 EventManagerWave）

#include "CeraShopBonusItemEvent/CCeraShopBonusItemEvent.inc"
