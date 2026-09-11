// ============================================================================
// [SplitC 2026-09-11 batch2 · TU 迁移（ORIG GI 锚点）] 本文件由 CDataManagerScripts.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN23DeleteInvalidItemScriptC2Ev
//   ORIG TU     ：global_ScriptDeleteInvalidItem.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "CBoosterGage.h"  // BoosterGageData 权威布局（须先于 CDataManager.h）
#include "CDataManager.h"
#include "cMyTraceNoop.h"  // add_lottery_needMoney 命中 trace（ORIG 0x83650f5）
#include "GameWorld.h"
#include "CItemGeneratorMgr.h"
#include "CStackableItem.h"
#include "CItem.h"
#include "GlobalData.h"
#include "CWorldMap.h"
#include "NPCScript.h"   // CNPCScript 完整类型（_destroy 中 delete）
#include "STExpertJobScript.h"  // 权威 STExpertJobScript（唯一声明点）
#include "DNFLexWrapper.h"   // ScanType/ScanInt/ScanStr/ScanFloat/loadRDARScriptFile/
#include "Statuepredicate.h"  // pieceQuestRewardStatus 合并谓词（getRewardStatus）
#include "CDungeon.h"        // advancealtar::{StageDifficulty,RewardItemType}
#include <cstring>
#include <algorithm>     // std::find（CWorldMap::hasDungeon）

#include "global_ScriptDeleteInvalidItem/DeleteInvalidItemScript.inc"
