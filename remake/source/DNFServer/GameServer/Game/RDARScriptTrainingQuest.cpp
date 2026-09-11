// ============================================================================
// [SplitC 2026-09-11 batch4 · TU 迁移（ORIG GI 锚点）] 本文件由 STQuestScript.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__Z25importTrainingQuestScriptP19TrainingQuestScriptPKc
//   ORIG TU     ：RDARScriptTrainingQuest.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include "STQuestScript.h"
#include <algorithm>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Quest.h"
#include "MyRandom.h"          // suffleTrainingQuests 的 random_shuffle 随机数器
#include "DNFLexWrapper.h"
#include "CEnvironment.h"      // class SpinLock / less_TCHAR（importQuestScript [type] 静态表）
#include "SyncScriptDeps.h"    // checkValiedJobName（importQuestScript [target character]）

#include "RDARScriptTrainingQuest/importTrainingQuestScript.inc"
