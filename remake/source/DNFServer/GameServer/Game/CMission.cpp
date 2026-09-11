// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CMission/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// df_game_r 任务类族实现（G3-1 批次，2026-08-17）。
// 语义依据 docs/class_func_reports/CMission*.md + ORIG 弱访问器反汇编。
#include <bitset>

#include "CMission.h"
#include "CMissionList_Charac.h"
#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "GlobalData.h"
#include "RefPvpGrade.h"   // RefPvpGrade 权威定义（GetPvpPoint/GetPvpNextRankPoint）

// cMyTraceNoop（禁用追踪，ORIG 0x822ad5a/0x822ad60，属日志族 TU）
class cMyTraceNoop
{
public:
    cMyTraceNoop();
    void operator()(const char* fmt, ...);
};

// ============================================================================
// CMission
// ============================================================================

#include "CMission/CMission.inc"


#include "CMission/dtor_CMission.inc"

#include "CMission/_makeTotalMissionInfo.inc"


#include "CMission/_canUpdate.inc"


#include "CMission/_isAcceptCondition.inc"


#include "CMission/makeMissionInfo.inc"


#include "CMission/canUpdate.inc"


#include "CMission/isAcceptCondition.inc"


#include "CMission/Reward.inc"


#include "CMission/_Reward.inc"


bool CMission::operator<(const CMission& other)
{
    // ORIG W 0x085e30ae：!= 早退形（cmp;setne;test;je 续判，落 setl 返回），
    // 相等分支内联在 fall-through（P222 正向包裹；== 形产生镜像块序）。
    if (m_kind != other.GetMissionKind())
        return m_kind < other.GetMissionKind();
    if (m_missionIndex != other.GetMissionIndex())
        return m_missionIndex < other.GetMissionIndex();
    return false;
}

#include "CMission/_isAcceptCondition_Series.inc"


// ============================================================================
// CMission_rank（抽象，无 ctor；verifyMission 保持纯虚）
// ============================================================================

#include "CMission/dtor_CMission_rank.inc"

#include "CMission/isClearable.inc"


#include "CMission/isSeriesMission.inc"


#include "CMission/Update.inc"

#include "CMission/getClearCondition.inc"


#include "CMission/isAcceptCondition_1.inc"


// ============================================================================
// CMission_winning_count
// ============================================================================

#include "CMission/CMission_winning_count.inc"


#include "CMission/dtor_CMission_winning_count.inc"

#include "CMission/_makeTotalMissionInfo_1.inc"


#include "CMission/_canUpdate_1.inc"


#include "CMission/_isAcceptCondition_1.inc"


#include "CMission/isClearable_1.inc"


#include "CMission/verifyMission.inc"


#include "CMission/isSeriesMission_1.inc"


#include "CMission/Update_1.inc"


#include "CMission/getClearCondition_1.inc"


// ============================================================================
// CMission_winning_streak
// ============================================================================

#include "CMission/CMission_winning_streak.inc"


#include "CMission/dtor_CMission_winning_streak.inc"

#include "CMission/_makeTotalMissionInfo_2.inc"


#include "CMission/_canUpdate_2.inc"


#include "CMission/isClearable_2.inc"


#include "CMission/verifyMission_1.inc"


#include "CMission/isSeriesMission_2.inc"


#include "CMission/Update_2.inc"


#include "CMission/getClearCondition_2.inc"


// ============================================================================
// CMission_join
// ============================================================================

#include "CMission/CMission_join.inc"


#include "CMission/dtor_CMission_join.inc"

#include "CMission/_makeTotalMissionInfo_3.inc"


#include "CMission/_isAcceptCondition_2.inc"


#include "CMission/isClearable_3.inc"


#include "CMission/verifyMission_2.inc"


#include "CMission/isSeriesMission_3.inc"


#include "CMission/Update_3.inc"


#include "CMission/getClearCondition_3.inc"


// ============================================================================
// CMission_kill_count
// ============================================================================

#include "CMission/CMission_kill_count.inc"


#include "CMission/dtor_CMission_kill_count.inc"

#include "CMission/_makeTotalMissionInfo_4.inc"


#include "CMission/_isAcceptCondition_3.inc"


#include "CMission/isClearable_4.inc"


#include "CMission/verifyMission_3.inc"


#include "CMission/isSeriesMission_4.inc"


#include "CMission/Update_4.inc"


#include "CMission/getClearCondition_4.inc"


// ============================================================================
// CMission_hereafter_join
// ============================================================================

#include "CMission/CMission_hereafter_join.inc"


#include "CMission/dtor_CMission_hereafter_join.inc"

#include "CMission/isClearable_5.inc"


#include "CMission/verifyMission_4.inc"


#include "CMission/isSeriesMission_5.inc"


#include "CMission/Update_5.inc"


#include "CMission/getClearCondition_5.inc"


// ============================================================================
// CMission_hereafter_winning_count
// ============================================================================

#include "CMission/CMission_hereafter_winning_count.inc"


#include "CMission/dtor_CMission_hereafter_winning_count.inc"

#include "CMission/_canUpdate_3.inc"


#include "CMission/isClearable_6.inc"


#include "CMission/verifyMission_5.inc"


#include "CMission/isSeriesMission_6.inc"


#include "CMission/Update_6.inc"


#include "CMission/getClearCondition_6.inc"


// ============================================================================
// CMission_remain_hp
// ============================================================================

#include "CMission/CMission_remain_hp.inc"


#include "CMission/dtor_CMission_remain_hp.inc"

#include "CMission/isClearable_7.inc"


#include "CMission/verifyMission_6.inc"


#include "CMission/isSeriesMission_7.inc"


#include "CMission/Update_7.inc"


#include "CMission/getClearCondition_7.inc"


// ============================================================================
// CMission_revenge
// ============================================================================

#include "CMission/CMission_revenge.inc"


#include "CMission/dtor_CMission_revenge.inc"

#include "CMission/isClearable_8.inc"


#include "CMission/verifyMission_7.inc"


#include "CMission/isSeriesMission_8.inc"


#include "CMission/Update_8.inc"


#include "CMission/getClearCondition_8.inc"


// ============================================================================
// CMission_hereafter_kill_count
// ============================================================================

#include "CMission/CMission_hereafter_kill_count.inc"


#include "CMission/dtor_CMission_hereafter_kill_count.inc"

#include "CMission/isClearable_9.inc"


#include "CMission/verifyMission_8.inc"


#include "CMission/isSeriesMission_9.inc"


#include "CMission/Update_9.inc"


#include "CMission/getClearCondition_9.inc"


// ============================================================================
// CMission_kill
// ============================================================================

#include "CMission/CMission_kill.inc"


#include "CMission/dtor_CMission_kill.inc"

#include "CMission/_canUpdate_4.inc"


#include "CMission/isClearable_10.inc"


#include "CMission/verifyMission_9.inc"


#include "CMission/isSeriesMission_10.inc"


#include "CMission/Update_10.inc"


#include "CMission/getClearCondition_10.inc"


// ============================================================================
// CMission_within
// ============================================================================

#include "CMission/CMission_within.inc"


#include "CMission/dtor_CMission_within.inc"

#include "CMission/_isAcceptCondition_4.inc"


#include "CMission/isClearable_11.inc"


#include "CMission/verifyMission_10.inc"


#include "CMission/isSeriesMission_11.inc"


#include "CMission/Update_11.inc"


#include "CMission/getClearCondition_11.inc"


// ============================================================================
// CMission_remains_hp
// ============================================================================

#include "CMission/CMission_remains_hp.inc"


#include "CMission/dtor_CMission_remains_hp.inc"

#include "CMission/_isAcceptCondition_5.inc"


#include "CMission/isClearable_12.inc"


#include "CMission/verifyMission_11.inc"


#include "CMission/isSeriesMission_12.inc"


#include "CMission/Update_12.inc"


#include "CMission/getClearCondition_12.inc"


// ============================================================================
// CMission_move_channel
// ============================================================================

#include "CMission/CMission_move_channel.inc"


#include "CMission/dtor_CMission_move_channel.inc"

#include "CMission/_isAcceptCondition_6.inc"


#include "CMission/isClearable_13.inc"


#include "CMission/verifyMission_12.inc"


#include "CMission/isSeriesMission_13.inc"


#include "CMission/Update_13.inc"


#include "CMission/getClearCondition_13.inc"


// ============================================================================
// CMission_move_channel_total
// ============================================================================

#include "CMission/CMission_move_channel_total.inc"


#include "CMission/dtor_CMission_move_channel_total.inc"

#include "CMission/_isAcceptCondition_7.inc"


#include "CMission/isClearable_14.inc"


#include "CMission/verifyMission_13.inc"


#include "CMission/isSeriesMission_14.inc"


#include "CMission/Update_14.inc"


#include "CMission/getClearCondition_14.inc"


// ============================================================================
// CMission_combo_clear
// ============================================================================

#include "CMission/CMission_combo_clear.inc"


#include "CMission/dtor_CMission_combo_clear.inc"

#include "CMission/_makeTotalMissionInfo_5.inc"


#include "CMission/_isAcceptCondition_8.inc"


#include "CMission/isClearable_15.inc"


#include "CMission/verifyMission_14.inc"


#include "CMission/isSeriesMission_15.inc"


#include "CMission/Update_15.inc"


#include "CMission/getClearCondition_15.inc"


#include "CMission/_Reward_1.inc"


#include "CMission/GetRankRange.inc"

