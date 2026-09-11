#define _GLIBCXX_IOSTREAM 1
// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CreatureOnlyMgrRest/<Func>.inc（不参与编译、内部不含 #include）。
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

#include <map>

#include "CDataManager.h"
#include "CInventory.h"
#include "Inven_Item.h"
#include "LogManager.h"
#include "GlobalData.h"
#include "CGameManager.h"
#include "CUser.h"
#include "CPacketRespondent.h"
#include "SigTypes.h"
#include "CSystemTime.h"
// ============================================================================
// CreatureOnlyMgrRest.cpp —— user_creature::CCreatureMgr worklist 收尾
// （OnlyOrig_CreatureJob 批；SIG_CREATURE_ITEM_DETAIL_INFO 权威定义已提升至
//   CInventory.h 顶部，本 TU 不再携带局部副本。）
// ============================================================================

// ORIG _ZN13user_creature12CCreatureMgr10SetStomachEi T 0x08339ee0（SetStomach.md）：
// m_equippedCreature 非空 → CCreature::SetStomach(value) 后经 CPacketRespondent
// 广播 MakeNotipacketCreatureState(uid, GetStomach())；返回恒 1。
// ============================================================================

namespace user_creature
{
#include "CreatureOnlyMgrRest/SetStomach.inc"



#include "CreatureOnlyMgrRest/ChangeOwner.inc"


#include "CreatureOnlyMgrRest/ItemEvolute.inc"


#include "CreatureOnlyMgrRest/ChangeSlotNo.inc"


#include "CreatureOnlyMgrRest/LoadArtifact.inc"


#include "CreatureOnlyMgrRest/AuctionRegist.inc"


#include "CreatureOnlyMgrRest/DenySwapArtifact.inc"


#include "CreatureOnlyMgrRest/GetCreatureIndex.inc"


#include "CreatureOnlyMgrRest/LoadCreatureItem.inc"


#include "CreatureOnlyMgrRest/SaveCreatureItem.inc"


#include "CreatureOnlyMgrRest/SetOverSkillTime.inc"


#include "CreatureOnlyMgrRest/IsSaveCreatureItem.inc"


#include "CreatureOnlyMgrRest/UnRegistTempPeriod.inc"


#include "CreatureOnlyMgrRest/EventEvoluteCreature.inc"


#include "CreatureOnlyMgrRest/SendCreatureItemInfo.inc"


#include "CreatureOnlyMgrRest/LoadCreatureTempPeriod.inc"


#include "CreatureOnlyMgrRest/SetCreatureItemAddInfo.inc"


#include "CreatureOnlyMgrRest/InsertCreatureItemResult.inc"


#include "CreatureOnlyMgrRest/UpdateCreatureItemResult.inc"


#include "CreatureOnlyMgrRest/Evolute.inc"


#include "CreatureOnlyMgrRest/CCreatureMgr.inc"


// ORIG _ZN13user_creature12CCreatureMgrC1ERKS0_ W 0x86992cc【objdump】：
// map 拷贝 → +0x18/+0x1c → m_artifacts[3] 内联 3×mov → +0x2c → map30 →
// +0x48 bool；无 CArtifact::C1 调用 ⇒ 原码是**编译器隐式合成拷贝构造**。
// [b28d-solo-CreatureOnlyMgrRest] verdict=AE（原 DIFF 57）：CInventory.h 删用户
// 拷贝构造声明（Main 代落）⇒ 编译器隐式合成 W C1ERKS0_/C2ERKS0_；Main 续删
// m_pad49 后与 ORIG 62 条逐条一致（map 拷贝 → +0x18/+0x1c → 数组 3×mov 循环
// → +0x2c → map30 → +0x48 bool，尾部无 pad 拷贝、无 CArtifact::C1）。
// 发射点：CInventory.cpp 的 CInventory 拷贝构造成员初始化列表（Main 代落）。
// 本 TU 原 P147 force 使用点已按 Main 裁决删除（其连带发射的 map 拷贝模板
// W 实例随发射面迁至 CInventory.o，链接期 COMDAT 合并，全局符号集不变）。
// 真 bug 修正：旧用户定义形 `i<2` 漏拷 m_artifacts[2]（ORIG 循环 3 次）。

// ============================================================================
// const 侧查询
// ============================================================================

#include "CreatureOnlyMgrRest/getExpireDate.inc"


#include "CreatureOnlyMgrRest/GetCreatureItemSize.inc"


#include "CreatureOnlyMgrRest/IsExistedCreatureItem.inc"


#include "CreatureOnlyMgrRest/checkMaxCreatureLevel.inc"


#include "CreatureOnlyMgrRest/isGrowCreatureMaxLevel.inc"
{
    return m_isMaxLevelGrowCreature;
}

}  // namespace user_creature

namespace user_creature
{

#include "CreatureOnlyMgrRest/IsCreatureScript.inc"


}  // namespace user_creature


