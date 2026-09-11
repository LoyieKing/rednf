// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCargo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstdlib>
#include <cstring>

#include "CUserCharacInfo.h"
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "CInventory.h"

// ============================================================================
// 构造 / 析构（ORIG 0x86949da / 0x86949f8，weak）
// ============================================================================
extern unsigned int GetIntegratedPvPItemAttr(const Inven_Item& item);
extern Inven_Item g_emptySlot;
#include "CCargo/checkStackableLimit.inc"
#include "CCargo/CCargo.inc"


#include "CCargo/dtor_CCargo.inc"


#include "CCargo/set_parent.inc"


#include "CCargo/SetCapacity.inc"


#include "CCargo/SetInventoryMemory.inc"


#include "CCargo/reset.inc"


#include "CCargo/set_cargo.inc"


#include "CCargo/get_cargo.inc"


#include "CCargo/GetCapacity.inc"


#include "CCargo/GetItemCount.inc"


#include "CCargo/get_cargo_start.inc"


#include "CCargo/get_empty_slot.inc"


#include "CCargo/IsOperatorable.inc"


#include "CCargo/GetCargoRef.inc"


#include "CCargo/GetCargoRef_1.inc"



#include "CCargo/get_cargo_slot.inc"


#include "CCargo/GetCargoData.inc"


#include "CCargo/insert_item.inc"


#include "CCargo/_AddStackItem.inc"


#include "CCargo/_tryAddStackItem.inc"


#include "CCargo/tryInsertItemSpecialSlot.inc"


#include "CCargo/insert_item_special_slot.inc"


#include "CCargo/delete_item.inc"


#include "CCargo/_ResetSlot.inc"


#include "CCargo/move_item.inc"


#include "CCargo/update_item.inc"


#include "CCargo/_AddItem.inc"

#include "CCargo/check_slot_empty.inc"


#include "CCargo/check_item_exist.inc"


#include "CCargo/_GetItemCount.inc"


#include "CCargo/calcCargoItemCount.inc"


#include "CCargo/_IsExistEquipItem.inc"


#include "CCargo/FindItemLock.inc"


#include "CCargo/UpgradeCargo.inc"


#include "CCargo/isUpgradable.inc"


#include "CCargo/_CompareSlot.inc"


#include "CCargo/sort.inc"


#include "CCargo/SendItemLockList.inc"


#include "CCargo/MakeItemList.inc"


#include "CCargo/MakeItemPacket.inc"

