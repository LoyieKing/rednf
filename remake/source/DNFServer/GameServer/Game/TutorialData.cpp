// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 TutorialData/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// TutorialData — df_game_r ORIG 转写（docs/class_func_reports/TutorialData/*）
// 新 TU：需在 source/cmake/game/CMakeLists.txt 登记（宿主收口）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN12TutorialDataC2Ev`（ORIG 0x8ab97c5）。
#include <iostream>

#include "TutorialData.h"

#include "GlobalScriptTutorial.h"   // GlobalScriptTutorial::getData（ORIG 同 TU 域 0x08ab5xxx-0x08ab9xxx；被调前置定义使 4.4 帧收窄为 sub $0x4，与 ORIG 0x08ab971e 一致）

#include <cstddef>

static_assert(sizeof(TutorialData) == 0x148,
              "TutorialData layout mismatch vs ORIG (0x148)");
static_assert(sizeof(TutorialData::DialogData) == 8,
              "DialogData layout mismatch vs ORIG (8)");
static_assert(sizeof(TutorialData::SkillData) == 12,
              "SkillData layout mismatch vs ORIG (0xc)");
static_assert(sizeof(TutorialData::TutorialNotice) == 0x10,
              "TutorialNotice layout mismatch vs ORIG (0x10)");
static_assert(sizeof(TutorialData::ItemData) == 8,
              "ItemData layout mismatch vs ORIG (8)");

// ---- 子对象生命周期（ORIG W） ----------------------------------------

#include "TutorialData/TutorialData.inc"


#include "TutorialData/DialogData.inc"


#include "TutorialData/TutorialNotice.inc"


// ---- 主类生命周期 -----------------------------------------------------

#include "TutorialData/dtor_TutorialData.inc"


// ---- dungeonIndex ------------------------------------------------------

#include "TutorialData/setDungeonIndex.inc"


#include "TutorialData/getDungeonIndex.inc"


#include "TutorialData/getData.inc"


// ---- 对话数据 ----------------------------------------------------------

#include "TutorialData/addDialogData.inc"


#include "TutorialData/getDialogSize.inc"


#include "TutorialData/getDialogStr.inc"


#include "TutorialData/getFaceIndex.inc"


#include "TutorialData/isCurrentEpisode.inc"


// ---- 头像索引 ----------------------------------------------------------

#include "TutorialData/addAvatarIndex.inc"


#include "TutorialData/getAvatarIndex.inc"


// ---- 装备索引 ----------------------------------------------------------

#include "TutorialData/addEquipmentIndex.inc"


#include "TutorialData/getEquipmentIndex.inc"


// ---- 技能索引 ----------------------------------------------------------

#include "TutorialData/getConvertHotKey.inc"


#include "TutorialData/addSkillIndex.inc"


#include "TutorialData/getSkillData.inc"


#include "TutorialData/addPassiveSkillData.inc"


#include "TutorialData/getPassiveSkillData.inc"


// ---- 物品 -------------------------------------------------------------

#include "TutorialData/addItem.inc"


#include "TutorialData/getItem.inc"


// ---- 教程公告 ---------------------------------------------------------

#include "TutorialData/addTutorialNotice.inc"


#include "TutorialData/getNoticeAniPath.inc"

