// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ExpertJobOnlyEnchanter/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// ExpertJobOnlyEnchanter.cpp —— expert_job::CEnchanter 附魔师 15 符号独占 TU。
#include "InstanceRentalSystem.h"   // [EJEnchanter] 先行引入带 CheckRentableItem(int,uint) 的版本
//（该成员声明为 decl 追加项；ORIG T 0x082702dc）。
// 来源：docs/class_func_reports/expert_job__CEnchanter/ 全部报告逐份转写；
// IsUseOtherUserMonsterCard 无独立报告，按 objdump 反汇编转写并标注【objdump】。
// 布局依据 CExpertJob.h 现有 CEnchanter 字段：
//   +0x08/+0x18 ExpertJobMaterial m_materials[2]（步长 0x10）
//   +0x28 m_user +0x2c m_peer +0x30 uid +0x34 peerUid
//   +0x38 m_field38（附魔费用）+0x3c m_field3c（已登记怪物卡物品，
//   = m_materials[1].m_item）+0x40 m_field40（附魔配方/目标物品）+0x44 m_peerRequestType
// 头部声明追加需求见 /tmp/df_batches/CreatureJob_decl_EJEnchanter.txt。
// ============================================================================
#include "CUser.h"                 // ENUM_PEER_REQUEST_TYPE（先于本头）
#include "CExpertJob.h"
#include "PacketGuard.h"        // PacketGuard : InterfacePacketBuf（put_header/put_byte…）
#include "CItem.h"              // get_index / is_stackable / GetItemType / getUsableItemType
#include "CUser.h"              // Send/SendCmdErrorPacket/get_unique_id/get_acc_name/m_historyLog
#include "CInventory.h"         // GetInvenRef/GetInvenData/MakeItemPacket/gain_money/use_money/delete_item/get_money + GetInvenTypeFromItemSpace
#include "Inven_Item.h"         // Inven_Item（ctor/m_addInfo/m_fieldd）
#include "CDataManager.h"       // G_CDataManager() / find_item(int)
#include "GlobalData.h"         // GlobalData::s_rentalSystem
#include "InstanceRentalSystem.h"
#include "CUserProc.h"          // APSystem::CUserProc::ClearActionAndSendtoUser

namespace expert_job
{

#include "ExpertJobOnlyEnchanter/IsUseOtherUserMonsterCard.inc"


#include "ExpertJobOnlyEnchanter/IsRightMaterial.inc"


#include "ExpertJobOnlyEnchanter/IsRecipeRight.inc"


#include "ExpertJobOnlyEnchanter/IsEqualMonsterCad.inc"


#include "ExpertJobOnlyEnchanter/_DrawEnchant.inc"


#include "ExpertJobOnlyEnchanter/_TryEnchantMine.inc"


#include "ExpertJobOnlyEnchanter/_TryEnchantOther.inc"


#include "ExpertJobOnlyEnchanter/_PutItemForEnchant.inc"


#include "ExpertJobOnlyEnchanter/OnPutItemForEnchant.inc"


#include "ExpertJobOnlyEnchanter/OnTryEnchant.inc"


#include "ExpertJobOnlyEnchanter/_AgreeEnchant.inc"


#include "ExpertJobOnlyEnchanter/OnAgreeEnchant.inc"


#include "ExpertJobOnlyEnchanter/SetPeerRequestType.inc"


#include "ExpertJobOnlyEnchanter/_DoEnchant.inc"


}  // namespace expert_job
