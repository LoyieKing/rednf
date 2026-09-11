// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_EventPeriodDataManager/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ ARAD::Arad_EventPeriodDataManager 实现
//（批3：从 GameStubs.cpp 迁移；批4：补 Arad_InterfaceGameScript 基类生命周期）。
//
// ORIG ctor 0x81953fa：Arad_InterfaceGameScript("Arad_EventPeriodDataManager") 基类 +
// vector<SEventPeriodData>(+0x0c) / vector(+0x18) / Mutex(+0x24) / list(+0x3c)，+0x08=0。
// Load() 0x8195c9a：清理 + 装载事件周期脚本（loadScriptData）。
// isApplied 0x81956b0：在 +0x0c vector 按事件码找周期项，判定 [m_beginTime, m_endTime)。
#include "Arad_EventPeriodDataManager.h"

#include <algorithm>

#include "CUser.h"       // get_acc_id / get_charac_no
#include "CGuildServerProxy.h"   // WongWork::CGMAccounts::isGM 权威声明
#include "GlobalData.h"  // s_pGMAccounts_
#include "LogManager.h"
#include "MoreTypes.h"   // arad::SigLoadRewardEventItem 完整定义

namespace ARAD
{

#include "Arad_EventPeriodDataManager/Arad_EventPeriodDataManager.inc"


#include "Arad_EventPeriodDataManager/dtor_Arad_EventPeriodDataManager.inc"


#include "Arad_EventPeriodDataManager/loadScriptData.inc"


#include "Arad_EventPeriodDataManager/clearScriptData.inc"


#include "Arad_EventPeriodDataManager/isApplied.inc"


#include "Arad_EventPeriodDataManager/isRunning.inc"


#include "Arad_EventPeriodDataManager/addUserRewardData.inc"


#include "Arad_EventPeriodDataManager/addUserRewardData_1.inc"


#include "Arad_EventPeriodDataManager/isRewarded.inc"


#include "Arad_EventPeriodDataManager/checkCreateTime.inc"


#include "Arad_EventPeriodDataManager/delUserRewardData.inc"


#include "Arad_EventPeriodDataManager/setCreatePeriod_GM.inc"


}  // namespace ARAD
