// ============================================================================
// [SplitC 2026-09-11 batch3 · TU 迁移（ORIG GI 锚点）] 本文件由 ARAD_Database.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN4ARAD25Arad_DB_Charac_Dispatcher8dispatchEiiP6Stream
//   ORIG TU     ：Arad_Character.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "ARAD_Database.h"
#include "MySQL.h"
#include "Stream.h"
#include "SigTypes.h"
#include "GlobalData.h"          // GlobalData::s_db_mgr / s_systemTime_
#include "DBMgr.h"               // DBMgr::GetDBHandle / addUserDBInfo / stUserDBInfo_t
#include "CEnvironment.h"        // G_CEnvironment / STDBConnInfo
#include "CEventMgr.h"           // WongWork::CEventMgr（声明经 Main 收口）
#include "AdvanceAltarZip.h"     // compress_zip
#include "GameTypes.h"           // cMyTrace
#include "CSystemTime.h"
#include "GameWorld.h"           // G_GameWorld / GameWorld::GetChannelType
#include "LogManager.h"

#include "Arad_Character/dispatch.inc"
