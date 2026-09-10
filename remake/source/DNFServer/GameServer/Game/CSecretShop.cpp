// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 CSecretShop/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// df_game_r 还原 —— secretshop 域（G4 商城/交易批次）
// 参照 docs/class_func_reports/secretshop__*.md + ORIG 反汇编。
// 覆盖：SALE_INFO/BUY_INFO、RETAILER/SHOPPER、IBuyRule、
// CBuyItembyGold/CBuyItembyRecipe、CSecretShopStatistic、CSecretShop。
// SECRET_SHOP_DATA/SECRET_SHOP_INFO 由 SECRET_SHOP_DATA.cpp 提供。
// Dispatcher_SecretShop* / Timer_SecretShopStatistic 属分发域（G7），
// 本批不实现（GameStubs 桩保留）。
// ============================================================================

#include <map>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <ctime>

#include "Inven_Item.h"
#include "CDataManager.h"
#include "DNFLexWrapper.h"  // importSecretShopScript 的 Scan*/loadRDARScriptFile
#include "CInventory.h"
#include "CUser.h"
#include "CServerProxyMgr.h"
#include "CStatisticServerProxy.h"
#include "CCubeStatistic.h"
#include "GlobalData.h"
#include "SECRET_SHOP_DATA.h"
#include "CSecretShop.h"   // eBuyRule/CSecretShopStatistic/IBuyRule/CSecretShop 唯一声明点
#include "CParty.h"
#include "CDungeon.h"  // CheckLottery 遍历 m_dungeonMap
#include "PvPTypes.h"  // 权威全局 CMTRand（0x9c8，MT19937；实现见 CMTRand.cpp）
#include "Packet_Secret_Shop_Statistic.h"  // 权威包头（shared/packet/include）
#include "CGuildServerProxy.h"  // CEventBase/CEventManager 权威定义（LotteryItems 虚调 IsEventing）

#include <cstring>
// eBuyRule / CSecretShopStatistic / IBuyRule / CSecretShop 类声明已提取至
// CSecretShop.h（跨 TU 唯一声明点，供 CParty::dungeon_start 等调用方）。
// ---- CSecretShop::CSecretShop（GI 键名对齐：ORIG 该 ctor 所在 TU（SecretShop.cpp）
//      的首个强外部实体即本 ctor（_ZN10secretshop11CSecretShopC2Ev），故将定义
//      前置于本 TU 首位；定义体逐字未改，仅移动。）----

#include "CSecretShop/CSecretShop.inc"
}

// ---- importSecretShopScript（ORIG 0x8a81cf0，_Z22importSecretShopScriptP16SecretShopScriptPKc）----

// 按 docs/class_func_reports/_lt_global_gt_/importSecretShopScript.md 忠实转写：
//   - 所有取值扫描均为 int ScanInt(bool* ok) 形式；占位扫描传 (bool*)0
//     （ORIG movl $0x0 压参），成功标志仅用于条目循环终止判断。
//   - [dungeon npc]/[level npc]：每条 7 次扫描（key/npcIdx/rate 带 flag +
//     4 次 flag=0 占位，ORIG +0x11c 循环 i=0..3），循环至 flag 失败后
//     verifyNpc 对应 multimap。
//   - [npc]：npcKey 存入局部 stNpcSales::m_field0；内层 token 循环处理
//     [dungeon]/[level]（先 dungeonKey/maxCount 两次占位扫描，随后 itemIdx
//     带 flag 的商品循环）、[/npc]（整表插入 m_dungeonNpcMap）；
//     [dungeon cash item] 为无操作继续。
//   - [event item] 在外层：首个占位扫描写 m_eventDungeonSales.m_maxCount
//     （ORIG script+0x64），其后 itemIdx 带 flag 的商品循环插入 m_sales。
//   - stDungeonNpc 各字段与 key/npcIdx/rate 的对应为 [推断]（ORIG 经局部
//     辅助构造函数写入，未逐指令核对）。
#include "CSecretShop/importSecretShopScript.inc"
}

namespace secretshop
{

// ---- SALE_INFO / BUY_INFO（ORIG W 0x08a832aa / 0x08ab1606 逐字段清零；
//      operator== ORIG W 0x08a832ee / 0x08ab161e，sete 直返 bool，
//      供 std::find 实例（0x08a839dd / 0x08ab16ec）比较）----
#include "CSecretShop/SALE_INFO.inc"
}

bool SALE_INFO::operator==(int itemIdx)
{
    return m_itemIdx == itemIdx;
}

#include "CSecretShop/BUY_INFO.inc"
}

bool BUY_INFO::operator==(int itemIdx)
{
    return m_itemIdx == itemIdx;
}

#include "CSecretShop/RETAILER.inc"
}

#include "CSecretShop/dtor_RETAILER.inc"
}

#include "CSecretShop/clear.inc"
}

#include "CSecretShop/GetSaleInfo.inc"
}

#include "CSecretShop/SHOPPER.inc"
}

#include "CSecretShop/dtor_SHOPPER.inc"
}

#include "CSecretShop/clear_1.inc"
}

#include "CSecretShop/GetBuyInfo.inc"
}

#include "CSecretShop/BuyItem.inc"
}


#include "CSecretShop/CSecretShopStatistic.inc"
}

#include "CSecretShop/dtor_CSecretShopStatistic.inc"
}

#include "CSecretShop/Clear.inc"
}

#include "CSecretShop/GetNpcPos.inc"
}

#include "CSecretShop/GetNpcIndex.inc"
}

#include "CSecretShop/GetDungeonData.inc"
}

#include "CSecretShop/RecordShow.inc"
}

#include "CSecretShop/RecordBuy.inc"
}

#include "CSecretShop/RecordPrice.inc"
}


#include "CSecretShop/SendSecretShopStatistic.inc"
}




// [b28d-solo-CSecretShop] 返回类型 int→bool（判词见 CSecretShop.h 声明处）。
#include "CSecretShop/CheckLimit.inc"
}

#include "CSecretShop/InsertItemIntoInventory.inc"
}

#include "CSecretShop/SendSecretShopBuyItem.inc"
}

#include "CSecretShop/LogCubeStatistic.inc"
}

#include "CSecretShop/LogValueStatistic.inc"
}

// ---- CBuyItembyGold（金币购买规则） ----
// 无 virtual 析构；ctor 委托 IBuyRule(CSecretShopStatistic&) 存 +0x04，vptr 置
// &_ZTVN10secretshop14CBuyItembyGoldE+0x8（BuyItem 于 vtable[0]，ORIG 确认）。

#include "CSecretShop/CBuyItembyGold.inc"
}

#include "CSecretShop/BuyItem_1.inc"
}

// ---- CBuyItembyRecipe（配方购买规则） ----
// 无 virtual 析构；ctor 委托 IBuyRule(CSecretShopStatistic&)，vptr 置
// &_ZTVN10secretshop16CBuyItembyRecipeE+0x8（BuyItem 于 vtable[0]，ORIG 确认）。

#include "CSecretShop/CBuyItembyRecipe.inc"
}

#include "CSecretShop/BuyItem_2.inc"
}


// ---- CSecretShop（ORIG 布局：map<eBuyRule,IBuyRule*> +0x00、
//      CMTRand* +0x18、CSecretShopStatistic +0x1c） ----

// ---- SecretShopScript 跨类方法（CDataManager +0xa700，ORIG 真实符号；
//      声明见 CDataManager.h，本 TU 直接真实调用） ----

#include "CSecretShop/dtor_CSecretShop.inc"
}

#include "CSecretShop/GetRule.inc"
}

#include "CSecretShop/LotteryNpc.inc"
}

#include "CSecretShop/LotteryItems.inc"
}

#include "CSecretShop/CheckLottery.inc"
}

#include "CSecretShop/BuyItem_3.inc"
}

#include "CSecretShop/SendSecretShopNpc.inc"
}

#include "CSecretShop/SendSecretShopItemList.inc"
}

#include "CSecretShop/SendSecretShopStatistic_1.inc"
}

}  // namespace secretshop
// ============================================================================
// SecretShopScript::GetItemByDungeonIdx / GetItemByDungeonLev（ORIG 0x08a82a0e /
// 0x08a82b34）。依赖链：GetItems -> CopyItem/IsExistItem/GetDomainRate，GetLevelIdx，
// GetItemByEvent -> GetItems。全部按 class_func_reports/SecretShopScript 忠实转写。
// ============================================================================
#include "CSecretShop/GetItemByDungeonIdx.inc"
}

#include "CSecretShop/GetItemByDungeonLev.inc"
}

#include "CSecretShop/GetItemByEvent.inc"
}

#include "CSecretShop/GetItems.inc"
}

#include "CSecretShop/IsExistItem.inc"
}

#include "CSecretShop/GetDomainRate.inc"
}

#include "CSecretShop/GetLevelIdx.inc"
}

#include "CSecretShop/GetRandItemPrice.inc"
}

#include "CSecretShop/CopyItem.inc"
}

// ---- SecretShopScript::clear（ORIG 0x8372020 W）----
#include "CSecretShop/clear_2.inc"
}

// ---- SecretShopScript::verifyNpc（ORIG 0x8a82804）----
#include "CSecretShop/verifyNpc.inc"
}

#include "CSecretShop/GetSecretShopData.inc"
}
