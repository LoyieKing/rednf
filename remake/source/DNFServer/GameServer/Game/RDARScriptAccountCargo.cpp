// ============================================================================
// [SplitC 2026-09-11 batch3 · TU 迁移（ORIG GI 锚点）] 本文件由 InitScriptProducers_1.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__Z24importAccountCargoScriptP18AccountCargoScriptPKc
//   ORIG TU     ：RDARScriptAccountCargo.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include "CDataManager.h"
#include "sync_script.h"   // STEquipmentPartSetPiece 完整类型（DataManagerInitDepends.h 前置依赖）
#include "DataManagerInitDepends.h"
#include "DNFLexWrapper.h"
#include "LogManager.h"
#include <cstdio>
#include <cstring>

#include "RDARScriptAccountCargo/importAccountCargoScript.inc"
