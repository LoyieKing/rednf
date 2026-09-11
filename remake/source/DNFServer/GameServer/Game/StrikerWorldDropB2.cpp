// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 StrikerWorldDropB2/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string>
#include <utility>
#include <vector>

#include "CDataManager.h"             // StageMapList 权威声明
#include "DataManagerInitDepends.h"   // AvatarRoulette 权威布局（须在 CDataManager.h 之后）
#include "StrikerScript.h"            // StrikerScript / StrikerSkill / 子类型
#include "EquipmentParameterInfo.h"   // importSkillDataUp + EquipmentSkillDataUp
#include "DNFLexWrapper.h"            // ScanType / ScanInt / ScanFloat / ScanStr
#include "CItemGenerator.h"           // WongWork::CWorldDrop

#include "StrikerWorldDropB2/TagSkillPenalty.inc"


// ============================================================================
// StrikerScript 脚本导入叶子（报告全文转写）
// ============================================================================

#include "StrikerWorldDropB2/importSkillDataUpVector.inc"


#include "StrikerWorldDropB2/importSkillAttackPenalty.inc"


#include "StrikerWorldDropB2/importSkillCoolTimePenalty.inc"


#include "StrikerWorldDropB2/importParentLevelPenlaty.inc"


#include "StrikerWorldDropB2/readSkillPenalty.inc"


// ============================================================================
// StrikerSkill 生命周期（无独立函数报告【标记】，objdump 逐指令转写）
// ============================================================================

#include "StrikerWorldDropB2/StrikerSkill.inc"

// ============================================================================
// WongWork::CWorldDrop（无独立函数报告两处【标记】，objdump 转写；
// 双参 loadScript 为报告全文转写）
// ============================================================================

#include "StrikerWorldDropB2/CWorldDrop.inc"

#include "StrikerWorldDropB2/loadScript.inc"


#include "StrikerWorldDropB2/loadScript_1.inc"


#include "StrikerWorldDropB2/makeStageMapUniqueId.inc"

// ============================================================================
// AvatarRoulette::swap（报告全文转写；生命周期其余部分在 InitScriptProducers_6）
// ============================================================================

#include "StrikerWorldDropB2/swap.inc"

