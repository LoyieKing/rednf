// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CUserStatB/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "CUser.h"
#include "CDataManager.h"
#include "CInventory.h"
#include "CItem.h"
#include "Inven_Item.h"
#include "GlobalData.h"
#include "CSystemTime.h"
#include "CQuestShop.h"

// ---------------------------------------------------------------------------
// CUser::RefillItemByScript —— ORIG 0x086890be，报告逐段转写。
// 遍历 CDataManager::GetRefillItemInfo()（= m_limitItemUsage.m_refill18.m_list，
// ORIG this+0x4d10）。对到期秒晚于当前秒的每个条目：
//   count==0：向调用方 vector 压入 pair(itemId, 0)（供下游 deleteSpecificItem）；
//   count!=0：find_item 查定义，按背包有无分流：
//     无（GetInvenData<0）→ vtable 槽 2 make_item 填默认 Inven_Item 后，
//        强制写 pack(1) +0x02 物品索引=entry.m_itemId、+0x07 数量=entry.m_count
//        （ORIG 栈槽 -0x67/-0x62 即这两字段），再插入背包
//        insertItemIntoInventory(item, reason 0xd, bLog=true, b=false)；
//     有 → 先 ItemAdd(INVEN_TYPE 1, 现有数量, 物品索引, item, 0xd) 记日志，
//        再把 +0x07 数量改为 entry.m_count 后 update_item 回写；
//   插入/更新成功（slot>=0）→ SendUpdateItemList(eSendTarget_1, 空间 0, slot)。
// ---------------------------------------------------------------------------
#include "CUserStatB/RefillItemByScript.inc"

// ---------------------------------------------------------------------------
// CUser::sendCharacQuestPiece —— ORIG 0x0868af2c。
// 取角色扩展槽 0xb（CQuestShop），非空则转发 sendCharacQuestPiece(this)。
// ---------------------------------------------------------------------------
#include "CUserStatB/sendCharacQuestPiece.inc"
