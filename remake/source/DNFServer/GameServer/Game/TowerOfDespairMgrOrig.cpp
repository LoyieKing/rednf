// ============================================================================
// [SplitC 2026-09-11 batch3 · TU 迁移（ORIG GI 锚点）] 本文件由 CtorProducers_1.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN17TowerOfDespairMgrC2Ev
//   ORIG TU     ：TowerOfDespairMgr.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "CDataManager.h"
#include "CCharacList.h"
#include "CWorldMap.h"
#include "WorldMapScript.h"
#include "NPCScript.h"
#include "STCharacterScript.h"   // STCharacterScript 权威布局（CCharacList::init 真实 C1/D1）
#include "NPCDynamicInfo.h"      // CNPCDynamicInfo 权威布局/ctor（CNPCDynamicInfoManager::add 真实 C1）
#include "CChattingEmoticonList.h"
#include "CGuildServerProxy.h"   // DeliveryMsg 权威声明（迁入定义 NL6）
#include "sync_script.h"             // STAICharacterCommonScript 占位/STEquipmentPartSetPiece（须先于 deps 头）
#include "DataManagerInitDepends.h"  // TowerOfDespair/CLevelRewardTable 权威类（Ctor_Group1）
#include "EquipmentParameterInfo.h"
#include "STEquipmentScript.h"       // ImportEquipmentScript 声明（5 参）
#include "DNFLexWrapper.h"           // loadRDARScriptFile/ScanType/ScanInt/loadRDARScriptListFile
#include "LogManager.h"
#include "GlobalData.h"
#include "CEnvironment.h"
#include "RDARScriptStringManager.h" // g_scriptStringManager_/findString
#include "sync_script.h"             // STAICharacterCommonScript 占位/STEquipmentPartSetPiece（须先于 deps 头）
#include "DataManagerInitDepends.h"  // TowerOfDespair/CLevelRewardTable 权威类（Ctor_Group1）
#include "CDungeon.h"                // get_index/get_min_level/get_standard_level/getDungeonKind
#include "GameTypes.h"               // cMyTrace
#include "TimerDispatcher.h"         // TimerNPCMoodChange::registNextTimer（权威声明）

#include "TowerOfDespairMgrOrig/TowerOfDespairMgr.inc"
