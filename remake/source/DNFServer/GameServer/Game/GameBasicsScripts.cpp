// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 GameBasicsScripts/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include <cstring>
// [T2-GI · P338/P327] 补 <iostream> 恢复 file-scope 动态初始化 __ioinit（ORIG 实有符号），
// 使本 TU 发射 _GLOBAL__I_（键名见下 ClearData 前移注释）。
#include <iostream>
#include "CDataManager.h"
#include "GamePODTypes.h"

namespace APSystem
{
#include "GameBasicsScripts/ClearData.inc"


#include "GameBasicsScripts/CActionPointEtcParameter.inc"


#include "GameBasicsScripts/dtor_CActionPointEtcParameter.inc"

#include "GameBasicsScripts/IsValidTodayRewardItem.inc"

}


#include "GameBasicsScripts/init.inc"


#include "GameBasicsScripts/init_1.inc"



#include "GameBasicsScripts/getPvPSkillPoint.inc"


#include "GameBasicsScripts/getGiveSkill.inc"


// 全局脚本重复加载开关（ORIG 0x0949b284 B，char）
char g_isAllowScriptDuplication = 0;

#include "GameBasicsScripts/getPvPSkillTree.inc"

