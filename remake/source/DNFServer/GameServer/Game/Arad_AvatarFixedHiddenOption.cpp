// ============================================================================
// [SplitC 2026-09-11 batch3 · TU 迁移（ORIG GI 锚点）] 本文件由 AvatarFixedHiddenOptionServer.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN4ARAD6SCRIPT23AvatarFixedHiddenOptionC2Ev
//   ORIG TU     ：Arad_AvatarFixedHiddenOption.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "AvatarFixedHiddenOptionServer.h"
#include <cstdio>
#include <map>
#include "CEnvironment.h"    // less_TCHAR / SpinLock
#include "GameTypes.h"       // cMyTrace
#include "DNFLexWrapper.h"   // loadRDARScriptFile / ScanType / ScanInt / ScanFloat
#include "CUser.h"
#include "CUserCharacInfo.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "CDataManager.h"       // G_CDataManager / find_item
#include "LogManager.h"
#include "GlobalData.h"
#include "MoreTypes.h"          // arad::SigAvatarHiddenOptionChange / SigAradAvatarHiddenOptionSave
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "Arad_InternalDispatcher.h"   // DISPATCHER::make_internal_stream_jpn

#include "Arad_AvatarFixedHiddenOption/AvatarFixedHiddenOption.inc"
