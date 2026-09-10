// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 OnlyOrigBatch6_Force/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// OnlyOrigBatch6_Force.cpp —— 本批仅 ORIG 符号的落地（构造/析构/copy 族）。
//
// 全部目标符号为 ORIG 头内联弱符号（nm W），重建头内联定义已补全（SigTypes
// C/D/E/B、SIG_SAVE_EXPAND_EQUIPSLOT、STGuildSkill、BreakAwayPreventSystem、
// CDungeon、CBattle_Field_shared、CUser、EquipmentParameterInfo、CMap、
// STComputeDamageVariable、STStackableScript、SpecialItemHandler、
// W8LT_PassiveObjectScript、TODLayer、CTournamentDungeonReward、CStackableItem、
// QuestClear）；本 TU 通过显式构造/拷贝/析构（P147/P149 force 发射）使编译器
// 按 ODR 使用点发射同名 W 符号。STGrowthPowerData 两 getter 与 ScanAll ×2 为
// T 强符号，已落地于 CDataManagerScripts.cpp / DNFLexWrapper.cpp。
// ============================================================================

#include "SigTypes.h"                  // SIG_* 全部（C/D/E/B 分片）
#include "STGuildSkill.h"
#include "BreakAwayPreventSystem.h"    // STRewardData
#include "CDungeon.h"                  // STTowerDialog/TournamentRewardRate/UseCoinDungeonDiff
#include "CBattle_Field_shared.h"      // STKillMonsterData
#include "CUser.h"                     // STPremiumItemData
#include "EquipmentParameterInfo.h"    // STResultItemCnt
#include "CMap.h"                      // STAssignBloodSystem 等
#include "STComputeDamageVariable.h"
#include "STStackableScript.h"         // STStackablePackageData/STEnchantSystemMapData/Key
#include "SpecialItemHandler.h"
#include "W8LT_PassiveObjectScript.h"  // STPassiveObjectScript/SpeedCtrlPassiveObjInfo
#include "TODLayer.h"
#include "CTournamentDungeonReward.h"  // TournamentRewardType
#include "CStackableItem.h"            // StatChangeInfo

namespace
{

// P147 force 发射：每个 ODR 使用点生成对应 W 符号（C1/C2 同址别名、D1/D2 同址）。
#include "OnlyOrigBatch6_Force/emitBatch6WeakSymbols.inc"
}

}  // namespace
