// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 WongWorkSkillGM_A/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>

#include "SkillSlot.h"          // CSkillChanger 类定义 / CUser / NumberToString / G_CDataManager
// [Lane37CUser 2026-09-10 解环] SkillSlot.h 已把 `#include "CUser.h"` 改为前向声明
//   （解 CUser.h ⇄ SkillSlot.h 环）⇒ 依赖传递获得 CUser 完整定义的 TU 需自行直补。
#include "CUser.h"              // CUser 完整定义（本 TU 解引用 CUser 成员）
#include "Quest.h"              // Quest（+0xb8 字段）/ RewardItemInfo 完整型
#include "QuestClear.h"         // WongWork::CQuestClear::enumQuestClear 回调链
#include "CDataManager.h"       // find_quest / get_level_exp
#include "CMyFileLog.h"         // AppendGM_Sys 日志
#include "CGuildServerProxy.h"  // WongWork::CGMAccounts / stGMInfo_t 权威声明

namespace WongWork
{

#include "WongWorkSkillGM_A/loadGMAccounts.inc"


#include "WongWorkSkillGM_A/CheckCondition.inc"

#include "WongWorkSkillGM_A/CountElixer.inc"


#include "WongWorkSkillGM_A/GetElixerCount.inc"


#include "WongWorkSkillGM_A/GetNeedElixerCount.inc"


#include "WongWorkSkillGM_A/GetNeedElixerAbility.inc"


#include "WongWorkSkillGM_A/_addSPRelatedItem.inc"


#include "WongWorkSkillGM_A/_addSFPRelatedItem.inc"


#include "WongWorkSkillGM_A/_fnEnumQuestClear.inc"


#include "WongWorkSkillGM_A/_CountQuestSPBook.inc"


// ============================================================================
// CGMAccounts 三缺失成员
// ============================================================================

#include "WongWorkSkillGM_A/appendGM.inc"


#include "WongWorkSkillGM_A/AppendGM_Sys.inc"


}  // namespace WongWork
