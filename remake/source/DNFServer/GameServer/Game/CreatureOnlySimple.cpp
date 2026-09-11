// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CreatureOnlySimple/<Func>.inc（不参与编译、内部不含 #include）。
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

#include "GlobalData.h"    // s_systemTime_ / s_*_stream_pool
#include "Inven_Item.h"    // Inven_Item / Inven_Item_Compatible
#include "SigTypes.h"      // SIG_CREATURE_ITEM
#include "StreamPool.h"    // Stream(m_poolType) / StreamPool/BigStreamPool/SmallStreamPool
#include "CInventory.h"    // user_creature::CCreatureItem / CCreatureMgr 完整布局
#include "CGameManager.h"  // user_creature::COverSkillTime/CStomach/CEgg/CDBMsgSender/
                           // CCreatureItemFactory/GetInstanceCreatureItemFactory
#include "CUser.h"

namespace user_creature
{

// ============================================================================
// COverSkillTime（4B，内嵌于 CCreature +0x50）
// ============================================================================

#include "CreatureOnlySimple/COverSkillTime.inc"


#include "CreatureOnlySimple/SetTime.inc"


#include "CreatureOnlySimple/GetTime.inc"


// ============================================================================
// CStomach（8B，内嵌于 CCreature +0x34：m_lastTime@+0 / m_value@+4）
// ============================================================================

#include "CreatureOnlySimple/CStomach.inc"


// ============================================================================
// CCreatureItem（权威布局见 CInventory.h；本批提供外联 ctor / 类型写入器）
// ============================================================================

#include "CreatureOnlySimple/CCreatureItem.inc"


#include "CreatureOnlySimple/SetCreatureItemType.inc"


// CCreatureItem::GetItemId 定义后置至 CEgg::SaveCreatureItem 之后（P210：4.4 对
// TU 内已见定义的 callee 精确继承 outgoing，SaveCreatureItem 帧塌缩 sub $0x4；
// 未前置时保守大帧=sub $0x18 与 ORIG 4.1 一致）。

#include "CreatureOnlySimple/GetUid.inc"


#include "CreatureOnlySimple/GetNoCharge.inc"


#include "CreatureOnlySimple/GetStat.inc"


// ============================================================================
// CEgg（继承 CCreatureItem；布局替换 decl 见 txt）
// ============================================================================

#include "CreatureOnlySimple/CEgg.inc"


#include "CreatureOnlySimple/Reset.inc"


#include "CreatureOnlySimple/IsSave.inc"


#include "CreatureOnlySimple/LoadCreatureItem.inc"


#include "CreatureOnlySimple/SaveCreatureItem.inc"


#include "CreatureOnlySimple/GetItemId.inc"


// ============================================================================
// CDBMsgSender（Init/C1/dtor/Send 归 CCreatureMgr.cpp；此处补余下消息填充族）
// ============================================================================

#include "CreatureOnlySimple/Cancel.inc"


// ============================================================================
// 共同骨架（ChangeOwner/ChangeSlotNo/AuctionRegist 同构，objdump 逐条核对）：
//   m_stream 空 → 0；buf = GetInBuffer<SIG_CREATURE_ITEM>()；
//   memset(buf, 0, 0x74)；compat(buf+0x04).setCopy(*item)；
//   buf->m_addInfo2(+0x30) ← item+0x07（低半 addInfo2）；随后各自差异字段。
// ============================================================================

#include "CreatureOnlySimple/ChangeOwner.inc"


#include "CreatureOnlySimple/ChangeSlotNo.inc"


#include "CreatureOnlySimple/AuctionRegist.inc"


// ============================================================================
// 全局池尺寸包装器（均转发 CCreatureItemFactory 单例查询）
// ============================================================================

#include "CreatureOnlySimple/GetCreatureEggPoolSize.inc"


#include "CreatureOnlySimple/GetCreaturePoolSize.inc"


#include "CreatureOnlySimple/GetCreatureEggPoolRemainSize.inc"


#include "CreatureOnlySimple/GetCreaturePoolRemainSize.inc"


}  // namespace user_creature
