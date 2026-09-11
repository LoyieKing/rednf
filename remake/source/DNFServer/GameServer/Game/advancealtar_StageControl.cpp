#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 advancealtar_StageControl/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r Game/ advancealtar::StageControl 专属 TU（从 GameStubs.cpp 拆出）。
// 声明唯一归口 advancealtar.h（具名布局：m_index@+0x00 / ProcStage 槽@+0x04 /
// CharacData 槽@+0x98，sizeof 0xc0）；本 TU 全部函数依据
// docs/class_func_reports/advancealtar__StageControl/ 报告翻译。
#include "CGameManager.h"
#include "CInventory.h"  // PacketGuard 完整定义 / CInventory::SendItemLockListInven
#include "CMap.h"  // CMap::get_index/get_monster_list、_mapMonster（sendMapInfo）
#include "GameWorld.h"
#include <cstring>        // std::memset / std::memcpy
#include <new>            // placement new（StageControl 子对象构造）
#include "advancealtar_ProcStage.h"  // ProcStage/CharacData 完整类型（getProcStage/selectStage）

namespace advancealtar
{
#include "advancealtar_StageControl/StageControl.inc"


#include "advancealtar_StageControl/dtor_StageControl.inc"


#include "advancealtar_StageControl/getIndex.inc"


#include "advancealtar_StageControl/setIndex.inc"


#include "advancealtar_StageControl/leaveUser.inc"


#include "advancealtar_StageControl/onTimerStageTick.inc"


#include "advancealtar_StageControl/reset.inc"


#include "advancealtar_StageControl/destroy.inc"


#include "advancealtar_StageControl/getProcStage.inc"


#include "advancealtar_StageControl/getCharacData.inc"


#include "advancealtar_StageControl/sendStageInfo.inc"


#include "advancealtar_StageControl/selectStage.inc"


#include "advancealtar_StageControl/sendEplp.inc"


#include "advancealtar_StageControl/onEplp.inc"


#include "advancealtar_StageControl/onEplpReturnVillage.inc"


#include "advancealtar_StageControl/onClearReward.inc"


#include "advancealtar_StageControl/onTimer.inc"


#include "advancealtar_StageControl/gotoStage.inc"



#include "advancealtar_StageControl/getCenterCount.inc"


#include "advancealtar_StageControl/sendMapInfo.inc"


#include "advancealtar_StageControl/Phase.inc"


#include "advancealtar_StageControl/reset_1.inc"


#include "advancealtar_StageControl/TimeLineSummary.inc"


#include "advancealtar_StageControl/dtor_TimeLineSummary.inc"


#include "advancealtar_StageControl/TimeLineSummary_1.inc"


TimeLineSummary& TimeLineSummary::operator=(const TimeLineSummary& other)
{
    m_field0 = other.m_field0;
    m_phaseSummarys = other.m_phaseSummarys;
    return *this;
}

#include "advancealtar_StageControl/reset_2.inc"


#include "advancealtar_StageControl/TimeLine.inc"


#include "advancealtar_StageControl/dtor_TimeLine.inc"


#include "advancealtar_StageControl/TimeLine_1.inc"


TimeLine& TimeLine::operator=(const TimeLine& other)
{
    m_phases = other.m_phases;
    m_summary = other.m_summary;
    return *this;
}

#include "advancealtar_StageControl/reset_3.inc"


}  // namespace advancealtar