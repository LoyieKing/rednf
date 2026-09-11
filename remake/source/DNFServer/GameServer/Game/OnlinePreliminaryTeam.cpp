// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 OnlinePreliminaryTeam/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// OnlinePreliminaryTeam.cpp —— online_preliminary 数据域方法实现。
// 权威头：OnlinePreliminary.h（勿在此 include CUserCharacInfo.h 旧类块）。
// 报告目录：docs/class_func_reports/online_preliminary__*/；无报告小函数
// 按 objdump 反汇编逐条转写并标注【推断：objdump】。
//
// 布局注记：ORIG 中 COnlinePreliminaryTeam::GetTeamId() const @0x8697892 读
// *(int*)(this+0x4)，而 GetCurrentUserCount 族操作 *(int*)(this+0x0)
// （0x8588d7e..0x8588dbd）；loadDataPreliminaryTeam/saveData 亦以 +0x4 为
// teamId 序列化键。故 GetTeamId 按 ORIG 修正为返回 m_field04
// 【推断修正：objdump 0x8697892】，GetTeamName(+0x14) 与原定义一致保持。

#include "OnlinePreliminary.h"

#include <cstring>

#include "CStreamGuard.h"
#include "StreamPool.h"
#include "MsgQueueMgr.h"
#include "GlobalData.h"
#include "Stream.h"
#include "SigTypes.h"
#include "CUser.h"
#include "GameTypes.h"      // cMyTrace

#include "OnlinePreliminaryTeam/COnlinePreliminaryTeam.inc"
{
}

#include "OnlinePreliminaryTeam/COnlinePreliminaryMember.inc"
{
}

namespace online_preliminary
{

// ============================================================================
// CharacterInServer
// ============================================================================

// ORIG W 0x08588d7e? no——0x085868d4 _ZNK18online_preliminary17CharacterInServerltERKS0_
// 报告 docs/class_func_reports/online_preliminary__COnlinePreliminaryMember/
//（operator< 为内联辅助，无独立报告）
// 【推断：objdump 0x85868d4】先按 serverId 低 16 位（movzwl）比较；
// 相等时再比较 uniqueId（int@+0x04）。
bool CharacterInServer::operator<(const CharacterInServer& rhs) const
{
    // [b28c-16] P0/P35：ORIG 无 s1/s2 槽（4 次 movzwl 表达式直读）；
    // 字段 unsigned short 化后 4.4 复现 `movzwl (%eax),%edx` 单段直落
    // （short+cast 恒拆两段，见 OnlinePreliminary.h 注释）。
    if (m_serverId < rhs.m_serverId)
        return true;
    if (m_serverId == rhs.m_serverId && m_uniqueId < rhs.m_uniqueId)
        return true;
    return false;
}

// ============================================================================
// COnlinePreliminaryTeam（sizeof 0x40）
// ============================================================================

#include "OnlinePreliminaryTeam/dtor_COnlinePreliminaryTeam.inc"


#include "OnlinePreliminaryTeam/GetTeamId.inc"


#include "OnlinePreliminaryTeam/GetTeamName.inc"


#include "OnlinePreliminaryTeam/ResetDailyMidnight.inc"


#include "OnlinePreliminaryTeam/Reset.inc"


#include "OnlinePreliminaryTeam/IncreaseWinCount.inc"


#include "OnlinePreliminaryTeam/IncreaseLoseCount.inc"


#include "OnlinePreliminaryTeam/SetRating.inc"


#include "OnlinePreliminaryTeam/GetRating.inc"


// ---- CurrentUserCount 族：全部操作 *(int*)(this+0x0)，无标志写入 ----

#include "OnlinePreliminaryTeam/IncreaseCurrentUserCount.inc"


#include "OnlinePreliminaryTeam/DecreaseCurrentUserCount.inc"


#include "OnlinePreliminaryTeam/SetCurrentUserCount.inc"


#include "OnlinePreliminaryTeam/GetCurrentUserCount.inc"


#include "OnlinePreliminaryTeam/IncreaseEachOtherTeamPlayCount.inc"


#include "OnlinePreliminaryTeam/GetEachOtherTeamPlayCount.inc"


#include "OnlinePreliminaryTeam/saveData.inc"


#include "OnlinePreliminaryTeam/_saveTeam.inc"


#include "OnlinePreliminaryTeam/_saveTeamMatchList.inc"


#include "OnlinePreliminaryTeam/loadDataPreliminaryTeam.inc"


#include "OnlinePreliminaryTeam/loadDataPreliminaryTeamMatchList.inc"


// ============================================================================
// COnlinePreliminaryMember（CData 派生，sizeof 0x14）
// ============================================================================

// ORIG T 0x08585f34 C1：charac_expand::CData ctor + 置本类虚表（编译器生成）。
// ORIG 不初始化 m_rating/m_winCount/m_loseCount（构造后为垃圾值）。
// 定义已前置到 includes 之后（GI 键名对齐），此处不再重复定义。

#include "OnlinePreliminaryTeam/dtor_COnlinePreliminaryMember.inc"


#include "OnlinePreliminaryTeam/loadData.inc"


#include "OnlinePreliminaryTeam/_saveData.inc"


#include "OnlinePreliminaryTeam/_reset.inc"


#include "OnlinePreliminaryTeam/IncreaseWinCount_1.inc"


#include "OnlinePreliminaryTeam/IncreaseLoseCount_1.inc"


#include "OnlinePreliminaryTeam/SetRating_1.inc"


// ============================================================================
// COnlinePreliminaryMemberMatchList（CData 派生，sizeof 0x20）
// ============================================================================

#include "OnlinePreliminaryTeam/COnlinePreliminaryMemberMatchList.inc"


#include "OnlinePreliminaryTeam/dtor_COnlinePreliminaryMemberMatchList.inc"


#include "OnlinePreliminaryTeam/loadData_1.inc"


#include "OnlinePreliminaryTeam/_saveData_1.inc"


#include "OnlinePreliminaryTeam/_reset_1.inc"


#include "OnlinePreliminaryTeam/ResetDailyMidnight_1.inc"


#include "OnlinePreliminaryTeam/IncreasePeerPlayCount.inc"


#include "OnlinePreliminaryTeam/GetPeerPlayCount.inc"


// ============================================================================
// CTeamFactory：StaticPool<COnlinePreliminaryTeam,600> 薄包装
//（ctor/dtor 转调基类 StaticPool 生命周期，定义在 GlobalData.cpp）
// ============================================================================

#include "OnlinePreliminaryTeam/Acquire.inc"


#include "OnlinePreliminaryTeam/Free.inc"


}  // namespace online_preliminary
