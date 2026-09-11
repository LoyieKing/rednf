// ============================================================================
// [SplitC 2026-09-11 batch2 · TU 迁移（ORIG GI 锚点）] 本文件由 CHackAnalyzer.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN19Secu_AccountHacking27checkAccountHackingHackTypeEN8WongWork13ENUM_HACKTYPEE
//   ORIG TU     ：Secu_AccountHacking.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "CHackAnalyzer.h"
#include <algorithm>
#include <string.h>
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "CItem.h"
#include "CStreamGuard.h"
#include "GlobalData.h"
#include "MsgQueueMgr.h"
#include "StreamPool.h"
#include "GameRand.h"
#include "CParty.h"
#include "CDungeon.h"
#include "CBattle_Field.h"
#include "CSystemTime.h"
#include "PacketBuf.h"
#include "DB_SelectCharacMid.h"
#include "DB_ReqSaveHackUserPunish.h"
#include "W6_HackPunishDeps.h"

#include "Secu_AccountHacking/checkAccountHackingHackType.inc"
