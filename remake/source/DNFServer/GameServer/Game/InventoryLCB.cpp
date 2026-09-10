// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 InventoryLCB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// InventoryLCB.cpp —— LinkClosureB.BattleItem 独占 TU。
// 忠实转写 CInventory ORIG 符号：
//   CheckEquipmentItemCondition(INVEN_TYPE, short, stEquipConditionForChronicle&)
//     const
//   T _ZNK10CInventory27CheckEquipmentItemConditionE10INVEN_TYPEsR28stEquipCondi
//     tionForChronicle @0x085083d0 (0x165)
// 报告：docs/class_func_reports/CInventory/CheckEquipmentItemCondition_085083d0.md
//
// 返回错误码 int：0=成功；0x11=槽空/找不到脚本道具；0x13=强化选项未鉴定；
// 0xd8=耐久不符（ORIG eax 直接值；bool 声明会截断错误码，已改 int）。
// stEquipConditionForChronicle 写入布局证据见 CInventory.h 注释与该报告。
// ============================================================================

#include "CBattle_Field.h"
#include "CParty.h"          // batch-15 dep-downgrade: was via CBattle_Field.h
#include "CEquipItem.h"      // CEquipItem::get_endurance（ORIG 非虚 thiscall）
#include "CDataManager.h"    // G_CDataManager / CDataManager::find_item
#include "CInventory.h"
#include "CItemAmplifier.h"  // stAmplifyOption_t::getAbilityType/isIdentified/getAbilityValue
#include "Inven_Item.h"

#include "InventoryLCB/CheckEquipmentItemCondition.inc"
}

// ============================================================================
// CInventory::CheckEquipmentItemCondition(vector<pair<int,int>>, stEquipCondition&,
//                                         Inven_Item&) const
// ORIG T 0x08507f1a _ZNK10CInventory27CheckEquipmentItemConditionERKSt6vectorISt4pair
//   IiiESaIS2_EER16stEquipConditionR10Inven_Item（0x4b6 字节）。
// 语义（逐指令对照）：
//   - items 空 → 0；遍历 items：pair.first 经 GetItemType 非 1(EQUIP) 跳过；
//     pair.second > 1 → 1；StartEndPos(type,&start,&end)；
//   - 段1 slot=start..end、段2 slot=3..8 扫 m_pEquipSlot(+0x650, 步进 0x3d)：
//     m_addInfo 匹配计数 >1 → 0x12；整槽 0x3d 字节拷贝到 outItem；
//     m_pParent->CheckItemLock(slot,1) 锁定 → 0xd5；
//   - 循环后 outItem.m_addInfo==0 → 0；find_item 空 → 0x11；
//     m_amp.getAbilityType()==0x80 → 0x13；m_fieldb != get_endurance → 0xd8；
//   - 输出：cond.m_upgrade=1、m_tableIdx=GetUpgrade()；abilityType==0 →
//     amplifySelect/pad3/field4 清零，否则 1/type/value；
//     m_flag6 = GetAttachType()==0 ? 1 : (m_field0 ? 1 : 0)；m_reSealCount。
// 返回错误码 int（0x11/0x12/0x13/0xd5/0xd8，bool 声明会截断，与 CInventory.h 一致）。
// ============================================================================
// b28d-solo-InventoryLCB verdict=AE（342/342 条逐条全等；.o 层仅 call/jmp 重定位目标
// 显示差，偏移量与 ORIG 逐值相同 ⇒ relink 后应 IDENTICAL）；p167 调用点差集 22=22 CLEAR。
// 关键修法：P197④ size()==0（empty 无 sete 段）；P283+P225 goto 进入 do-while 复现底部
// 条件 setle/test/jne；P135 pair 右值赋值 aSEOS0_；P184 type==0 死块照抄；P211/P248 档③
// abilityType uchar 同型化（char 下 `==0x80` 被 4.4 常量折叠删整段）；P214②/P222 正向包裹
// 共享尾 return-0 块；P28 后置 it++（ppEi 三临时槽 -0x40/-0x3c/-0x38 全对齐）。
// 真语义 bug 修复：CheckItemLock(slot,1) → CheckItemLock(1,slot)（ORIG 0x850812b/0x8508133
// 实参序 (1,slot)，与 AvatarRechargeServer 等调用点惯例一致）。
// 残差：_Z41__static_initialization_and_destruction_0ii = N123 按名撞名噪音；3 个仅NEW
//（_GLOBAL__I_ 键名 / pair C2Ev / iterator C2ERKS4_）= P93/P108 结构性别名，非发明功能符号。
// ============================================================================
#include "InventoryLCB/CheckEquipmentItemCondition_1.inc"
}
