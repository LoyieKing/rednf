// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CBossPlay/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
#include "CBossPlay.h"

#include <cmath>

#include "CParty.h"
#include "CUser.h"
#include "CDungeon.h"
#include "CDataManager.h"
#include "PacketGuard.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "GameWorld.h"
#include "CGuildServerProxy.h"   // CEventBase / CEventManager（GetRepeatEvent）
#include "CServerEvent.h"
#include "CEnvironment.h"
#include "CBurningFatigueEvent.h"
#include "CExpDoubleEvent.h"
#include "CPCRoomBurningEvent.h"
#include "LogManager.h"
#include "CItemGeneratorMgr.h"
#include "CBattle_Field_shared.h"
#include "GameRand.h"   // get_rand_int

// stClearRewardExpParameter_t / stClearRewardBonusExp_t 权威定义已迁至
// CParty.h（FixBossRewardExpDeps 域），此处不再本地定义。


namespace WongWork
{

namespace
{

// [AnonLane2 2026-09-11 匿名轴] 旧 `kPercentDivisor = 100.0f`（ORIG 侧为匿名
// .rodata `_DAT_08b7507c`，无符号）已删，3 处使用点内联字面量 `100.0f`。
// CItemGenerator 虚调用已由权威头 CItemGenerator.h 提供（generateRandomItem/
// generateSpecificItem 虚槽 +0x08/+0x0c），不再使用原始虚表跳转。
// CEventBase 判活改经真实虚函数 IsEventing(CUser*)（vtable+0x34）；
// CPCRoomBurningEvent::GetExpFactor 改真实类调用（权威头 CPCRoomBurningEvent.h）。
}  // namespace

// ============================================================================
// 生命周期
// ============================================================================

#include "CBossPlay/reset.inc"

#include "CBossPlay/stRewardItem_t.inc"

#include "CBossPlay/CBossPlay.inc"

#include "CBossPlay/dtor_CBossPlay.inc"

#include "CBossPlay/InitPlayUser.inc"

#include "CBossPlay/_checkMemberDie.inc"

#include "CBossPlay/checkAllMemberReady.inc"

#include "CBossPlay/getLiveMemberCount.inc"

#include "CBossPlay/handleDieUser.inc"

#include "CBossPlay/handleLeaveUser.inc"

#include "CBossPlay/generateRewardExp.inc"

#include "CBossPlay/generateRewardItem.inc"

}  // namespace WongWork