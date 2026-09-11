// ============================================================================
// [SplitC 2026-09-11 batch4 · TU 迁移（ORIG GI 锚点）] 本文件由 ScriptDepsA.cpp 拆出。
//   ORIG 期望键：_GLOBAL__I__Z20importDisjointScriptP14DisjointScriptPKc
//   ORIG TU     ：RDARScriptDisjoint.cpp
//   .inc 为原样搬移（零文本改动）；头集 = 源 TU 头 include 逐行复制。
//   <iostream> 提供 file-scope 动态初始化 = GI 发射前提（P327）。
// ============================================================================
#include <iostream>
#include <cstdio>
#include <cstring>
#include <strings.h>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "DNFLexWrapper.h"         // loadRDARScriptFile/ScanType/ScanInt/ScanFloat/
#include "LogManager.h"            // LogManager::logFormat
#include "STCharacterScript.h"     // STCharacterScript 完整布局 + 子结构（本批次补全）
#include "STRDCharacterStatusClient.h"  // STRDCharacterStatusClient（双向 operator=）
#include "DisJoint.h"              // DisjointScript 完整布局
#include "CCompoundAvatar.h"       // tagCompoundAvatar/tagCompoundAvatarTable
#include "CItemAmplifier.h"        // stAmplifyItemScript_t/ENUM_RARITY/ENUM_AMPLIFY_TYPE
#include "STEquipmentScript.h"     // ENUM_EQUIPMENTTYPE
#include "CEnvironment.h"          // less_TCHAR / SpinLock（ImportEquipmentAniScript
#include "STStackableScript.h"     // ENUM_MODULE_TYPE
#include "Quest.h"                 // ENUM_CHARACTERJOB

struct LotteryInfo;
int getElement(const char* tag);                               // 定义 CreatureScript.cpp
void createLotteryInfo(LotteryInfo* info, std::vector<int>* counts);   // 定义 CStackableItem.cpp
#include "RDARScriptDisjoint/importDisjointScript.inc"
