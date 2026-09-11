// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ItemVendingMachine_Auction/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include "ItemVendingMachine.h"

#include "CStreamGuard.h"
#include "GlobalData.h"
#include "StreamPool.h"
#include "CUser.h"
#include "CInventory.h"  // OS_API::snprintf（ORIG 0x858c81c）
#include "CDataManager.h"  // G_CDataManager/FindGoods/getItemList（GetFinalProbability）
#include "CItemList.h"     // CCeraShopGoods 完整定义（_GetItemType/_GetInvenSlotInfo 等）
#include <cstdlib>  // atoi（ORIG atoi@plt）
#include <cstdlib>  // atoi（ORIG atoi@plt）
#include "CDataManager.h"  // G_CDataManager/FindGoods/getItemList（GetFinalProbability）
#include "CItemList.h"     // CCeraShopGoods 完整定义（_GetItemType/_GetInvenSlotInfo 等）
#include <cstdlib>  // atoi（ORIG atoi@plt）
#include <cstdlib>  // atoi（ORIG atoi@plt）

namespace
{

// [AnonLane round8 2026-09-11] 原 6 个 `const double kRate*/kFeeDivisor`（匿名 namespace ⇒
// 我方发射 `_ZN12_GLOBAL__N_1L*kRate*E`/`L11kFeeDivisorE` 6 条 only-NEW，ORIG 全库无同名）
// 已删：ORIG 侧是**匿名 .rodata 常量**（0x08b871a8/b0/b8/c0/c8/d0，各 8 字节 double，
// 无符号）⇒ 使用点直接内联字面量（0.5/1.5/1.6/1.7/1.8/100.0）；4.4 为各值发射匿名
// `.LC` 双精度常量，与 ORIG `fldl <常量>` 形态一致。

}  // namespace

#include "ItemVendingMachine_Auction/BuyAuctionItem.inc"


// ORIG 布局断言：AuctionNeedInfo 0x28（put_binary 长度；trans1.m_tranNo 落在
// +0x14、trans2.m_tranNo 落在 +0x20，与 BillingSuccess/Fail 读参偏移一致）。
typedef char (*AuctionNeedInfoSizeAssert)[sizeof(ItemVendingMachine::AuctionNeedInfo) == 0x28 ? 1 : -1];
AuctionNeedInfoSizeAssert g_auctionNeedInfoSizeAssert = 0;

// ============================================================================
// W8_LongTail 批次 5 符号（ItemVendingMachine.h 声明；ORIG objdump 逐指令核对，
// 报告 docs/class_func_reports/ItemVendingMachine/{_GetItemType,_GetInvenSlotInfo,
// _CheckEnableInventoryCondition,LoadPurcahseBonusScript,GetFinalProbability}.md）
// ============================================================================

#include "ItemVendingMachine_Auction/_GetItemType.inc"


#include "ItemVendingMachine_Auction/_GetInvenSlotInfo.inc"


#include "ItemVendingMachine_Auction/_CheckEnableInventoryCondition.inc"


#include "ItemVendingMachine_Auction/LoadPurcahseBonusScript.inc"
{
    if (!m_bonus.LoadScript()) return false;
    return true;
}

// ---- GetFinalProbability —— ORIG 0x0854c470 ----
// FindGoods(goodsIdx) 视作 CCeraShopGoods* 取 GetBonusWeight()（float）；
// G_CDataManager()+0x0c 的 CItemList*（ORIG 内联读，非 getItemList W call）的
// GetVendingBonusRate()（W 0x0854c596，读 +0x4c）。fistpl 复用 rate 槽
// （ORIG fistpl -0x1c）。栈槽序依 ORIG：weight(-0x24)/rate(-0x1c)/goods(-0xc)，
// 4 个未用 dummy 撑出中间空隙（P31 帧差对齐）。+0x0c 读用 P1 组件视图折叠。
struct VendingItemListView { char m_pad[0xc]; CItemList* m_list; };  // CDataManager+0x0c 视图
#include "ItemVendingMachine_Auction/GetFinalProbability.inc"


