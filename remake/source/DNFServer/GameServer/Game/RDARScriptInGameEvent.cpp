// ============================================================================
// [SplitC 2026-09-11 batch2 · TU 迁移（ORIG GI 锚点）] 本文件由 CEventScriptMng.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN19InGameEventInfoList15insertEventInfoEiR15InGameEventInfo
//   ORIG TU     ：RDARScriptInGameEvent.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "CEventScriptMng.h"
#include "CUser.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "PacketGuard.h"
#include "DB_SelectLevelUpReward.h"   // process_level_up_reward 的 DB 确认请求
#include "GameWorld.h"
#include "CDataManager.h"
#include "DB_UpdateInGameEventHistory.h"
#include "InGameEventScripts.h"
#include <ctime>
#include <cstring>
#include "SyncScriptDeps.h"          // toTString

#include "RDARScriptInGameEvent/insertEventInfo.inc"
