// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 Arad_IPGProcess/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "Arad_IPGProcess.h"
#include "Arad_MileageProcess.h"

#include "CDataManager.h"    // G_CDataManager / find_item / FindGoods / getItemLimitEditionMgr
#include "CUser.h"           // CUser / cUserHistoryLog（+0x79700）/ CeraShopPaymentType
#include "CInventory.h"      // AddAvatarItem / use_money / MakeItemPacket / Inven_Item
#include "CItem.h"           // isCerashopPackageItem / getUsablePeriod / getExpirationDate
#include "CItemList.h"       // CCeraShopGoods::GetSubGroupIndex / get_rand_int
#include <math.h>     // floor（GiveMileage）
#include <stdlib.h>   // atoi
#include "CMailBoxHelper.h"  // ReqDBSendNewAvatarMailCashShop
#include "RDARScriptStringManager.h"  // findString / g_scriptStringManager_
#include "LogManager.h"
#include "ItemVendingMachine.h"  // BillingRecord
#include "Arad_DataManager.h"    // ARAD::Singleton<Arad_DataManager> / Arad_Mileage_Data
#include "CUserCharacInfo.h"     // getCurCharacNo（经 CUser 继承可见）

#include <stdlib.h>   // atoi
#include <string.h>   // strlen

#include "Arad_IPGProcess/CashShopModeAvatar.inc"


#include "Arad_IPGProcess/Arad_IPGProcess.inc"

#include "Arad_IPGProcess/dtor_Arad_IPGProcess.inc"

#include "Arad_IPGProcess/CashShopModeCoin.inc"


#include "Arad_IPGProcess/CashShopModeItem.inc"


#include "Arad_IPGProcess/CashShopModeCreature.inc"


#include "Arad_IPGProcess/CashShopModeVendingItem.inc"


#include "Arad_IPGProcess/Arad_MileageProcess.inc"

#include "Arad_IPGProcess/dtor_Arad_MileageProcess.inc"

#include "Arad_IPGProcess/BuySuccess.inc"


#include "Arad_IPGProcess/GiveMileage.inc"


#include "Arad_IPGProcess/BuyFail.inc"


#include "Arad_IPGProcess/BuyMileageItem.inc"

