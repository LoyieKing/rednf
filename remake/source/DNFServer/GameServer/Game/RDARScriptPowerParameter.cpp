// ============================================================================
// [SplitC 2026-09-11 batch4 · TU 迁移（ORIG GI 锚点）] 本文件由 InitScriptProducers_3.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__Z26importPowerParameterScriptP20PowerParameterScriptPKc
//   ORIG TU     ：RDARScriptPowerParameter.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include "CDataManager.h"
#include "sync_script.h"         // CSyncScript/STEquipmentPartSetPiece（须先于 deps 头，与 CDataManager.cpp 口径一致）
#include "DataManagerInitDepends.h"
#include "DNFLexWrapper.h"       // loadRDARScriptFile/ScanType/ScanInt/ScanFloat/ScanStr/ScanTypeOrStr
#include "LogManager.h"          // LogManager::logFormat
#include <map>
#include <string>
#include <string.h>              // memcpy（ORIG importQuestParameterScript [difficulty] 段）
#include <utility>
#include <vector>

#include "RDARScriptPowerParameter/importPowerParameterScript.inc"
