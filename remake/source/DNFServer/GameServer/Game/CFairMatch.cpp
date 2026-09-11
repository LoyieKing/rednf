// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CFairMatch/<Func>.inc（不参与编译、内部不含 #include）。
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

#include "CFairMatch.h"

#include "CDataManager.h"    // G_CDataManager / find_item / m_serverParameter
#include "CUser.h"           // CUser::GetRating / Send / CUserCharacInfo getters
#include "CEquipItem.h"      // CEquipItem::GetUsableEquipmentType
#include "Inven_Item.h"

namespace fair_pvp
{

// ---------------------------------------------------------------------------
// 静态数据成员
// ---------------------------------------------------------------------------

/* ORIG B 0x943e148 */
CMultiMatchRoomList* CFairMatch::pWaitingRoomList_ = 0;
/* ORIG B 0x943e14c */
int CFairMatch::nWaitingRoomListRefCount_ = 0;
/* ORIG D 0x93abb76（.data 初值 01） */
bool CFairMatch::bMatchable_ = true;

// ---------------------------------------------------------------------------
// fair_pvp::PlayerInfoThird
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// fair_pvp::TeamInfoThird
// ---------------------------------------------------------------------------

#include "CFairMatch/TeamInfoThird.inc"


#include "CFairMatch/reset.inc"


#include "CFairMatch/reset_1.inc"


#include "CFairMatch/add.inc"


#include "CFairMatch/del.inc"


#include "CFairMatch/refresh.inc"


#include "CFairMatch/size.inc"


#include "CFairMatch/SendToTeam.inc"


#include "CFairMatch/GetTeamRating.inc"


// ---------------------------------------------------------------------------
// fair_pvp::CFairMatch 生命周期
// ---------------------------------------------------------------------------

#include "CFairMatch/CFairMatch.inc"


#include "CFairMatch/dtor_CFairMatch.inc"


// ---------------------------------------------------------------------------
// IMatch 覆盖（本批符号）
// ---------------------------------------------------------------------------

#include "CFairMatch/GetUserList.inc"


#include "CFairMatch/RequestMap.inc"


#include "CFairMatch/OnStartFight.inc"


#include "CFairMatch/OnEndPvP.inc"


#include "CFairMatch/CheckSeat.inc"


// ---------------------------------------------------------------------------
// 非 IMatch worklist 方法（本批符号）
// ---------------------------------------------------------------------------

#include "CFairMatch/SetMatchAble.inc"


#include "CFairMatch/SetPlayCountPerOneTeam.inc"


#include "CFairMatch/CheckUnusableEquipment.inc"


#include "CFairMatch/_GetOppositeTeam.inc"


#include "CFairMatch/_GetOtherTeam.inc"


#include "CFairMatch/GetGiveItem.inc"


}  // namespace fair_pvp
