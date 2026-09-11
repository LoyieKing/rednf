// ============================================================================
// [SplitC 2026-09-11 batch4 · TU 迁移（ORIG GI 锚点）] 本文件由 InitScriptProducers_7.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__Z34importInGameAdvertisementEtcScriptP25InGameAdvertisementScriptPKc
//   ORIG TU     ：RDARScriptInGameAdvertisement.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include "CDataManager.h"        // DataManagerInitDepends.h 形参类型宿主，必须先包含
#include "DataManagerInitDepends.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <map>
#include <set>
#include <string>
#include <vector>
#include "DNFLexWrapper.h"
#include "EventAvatarListScript.h"
#include "CEventScriptMng.h"   // InGameEventInfo / InGameEventInfoList

#include "RDARScriptInGameAdvertisement/importInGameAdvertisementEtcScript.inc"
