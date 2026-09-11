// ============================================================================
// [SplitC 2026-09-11 batch3 · TU 迁移（ORIG GI 锚点）] 本文件由 CraneMinigameManager.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__ZN20CraneMinigameManagerC2Ev
//   ORIG TU     ：CraneMinigameManager.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制（inline/宏环境一致）。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）；ioinit 为 weak 不夺键。
// ============================================================================
#include <iostream>
#include "CraneMinigameManager.h"
#include <string>
#include "NeedMaterialDataManager.h"
#include "PacketGuard.h"
#include "CUser.h"             // Send(PacketGuard&)
#include "GameTypes.h"          // cMyTrace
#include "DNFLexWrapper.h"      // loadRDARScriptFile/ScanType/ScanInt/ScanFloat
#include <map>
#include "CEnvironment.h"      // class SpinLock / struct less_TCHAR（本函数惰性注册表）

#include "CraneMinigameManagerOrig/CraneMinigameManager.inc"
