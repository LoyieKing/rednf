// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CAccountCargo/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <cstring>

#include "CUser.h"
#include "Inven_Item.h"
#include "InterfacePacketBuf.h"
#include "CDataManager.h"
#include "GameWorld.h"
#include "CInventory.h"
#include "CTitleBook.h"
#include "DB_AccountCargoSync.h"
#include "ServiceRestrictManager.h"
#include "Arad_DataManager.h"

extern unsigned int GetIntegratedPvPItemAttr(const Inven_Item& item);
extern Inven_Item g_emptySlot;  // ORIG 全局（0x943ddc0），定义于 Inven_Item.cpp（ORIG Inven_Item TU）

#include "CAccountCargo/CAccountCargo.inc"


#include "CAccountCargo/SetDBData.inc"


#include "CAccountCargo/Clear.inc"


#include "CAccountCargo/ResetSlot.inc"


#include "CAccountCargo/GetCapacity.inc"


#include "CAccountCargo/SetCapacity.inc"


#include "CAccountCargo/GetMoney.inc"


#include "CAccountCargo/SetStable.inc"


#include "CAccountCargo/IsAlter.inc"


#include "CAccountCargo/CheckValidSlot.inc"


#include "CAccountCargo/GetSlot.inc"


#include "CAccountCargo/GetSlotRef.inc"


#include "CAccountCargo/CheckInsertCondition.inc"


#include "CAccountCargo/InsertItem.inc"


#include "CAccountCargo/DeleteItem.inc"


#include "CAccountCargo/MoveItem.inc"


#include "CAccountCargo/GetEmptySlot.inc"


#include "CAccountCargo/CheckSlotEmpty.inc"


#include "CAccountCargo/GetSpecificItemSlot.inc"


#include "CAccountCargo/CheckStackLimit.inc"


#include "CAccountCargo/GetItemCount.inc"


#include "CAccountCargo/CheckMoneyLimit.inc"


#include "CAccountCargo/AddMoney.inc"


#include "CAccountCargo/SubMoney.inc"


#include "CAccountCargo/DepositMoney.inc"


#include "CAccountCargo/WithdrawMoney.inc"


#include "CAccountCargo/SendNotifyMoney.inc"


#include "CAccountCargo/SendItemList.inc"


#include "CAccountCargo/MakeItemPacket.inc"


#include "CAccountCargo/CreateAccountCargo.inc"


#include "CAccountCargo/UpgradeAccountCargo.inc"


#include "CAccountCargo/SendNotifyRecipe.inc"


#include "CAccountCargo/DeleteAccountCargo.inc"

