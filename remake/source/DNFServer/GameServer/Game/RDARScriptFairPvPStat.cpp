// ============================================================================
// [SplitC 2026-09-11 batch4 · TU 迁移（ORIG GI 锚点）] 本文件由 InitScriptProducers_2.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__Z23importPairPvPStatScriptR17FairPvPStatScriptPKc
//   ORIG TU     ：RDARScriptFairPvPStat.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "CDataManager.h"             // 脚本类型权威定义（须先于 deps 头）
#include "sync_script.h"              // STEquipmentPartSetPiece 完整类型
#include "STEquipmentScript.h"        // STEquipmentScript/m_paramMap678/ImportEquipmentScript
#include "EquipmentParameterInfo.h"   // EquipmentParameterInfo copy/operator=
#include "DataManagerInitDepends.h"   // 本批声明唯一汇总点
#include "DNFLexWrapper.h"            // loadRDARScriptFile/ScanType/ScanInt/ScanStr/ScanFloat
#include "LogManager.h"               // LogManager::logFormat

#include "RDARScriptFairPvPStat/importPairPvPStatScript.inc"
