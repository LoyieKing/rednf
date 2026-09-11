// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CSecretShop/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
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


// ---- importSecretShopScript（ORIG 0x8a81cf0，_Z22importSecretShopScriptP16SecretShopScriptPKc）----

#include "CSecretShop/importSecretShopScript.inc"


namespace secretshop
{

#include "CSecretShop/SALE_INFO.inc"


bool SALE_INFO::operator==(int itemIdx)
{
    return m_itemIdx == itemIdx;
}

#include "CSecretShop/BUY_INFO.inc"


bool BUY_INFO::operator==(int itemIdx)
{
    return m_itemIdx == itemIdx;
}

#include "CSecretShop/RETAILER.inc"


#include "CSecretShop/dtor_RETAILER.inc"


#include "CSecretShop/clear.inc"


// [SplitC 2026-09-11 batch2 · TU 迁移] GetSaleInfo 已迁出至 SecretShopData.cpp（ORIG GI 键对齐）。



#include "CSecretShop/SHOPPER.inc"


#include "CSecretShop/dtor_SHOPPER.inc"


#include "CSecretShop/clear_1.inc"


#include "CSecretShop/GetBuyInfo.inc"


#include "CSecretShop/BuyItem.inc"



#include "CSecretShop/CSecretShopStatistic.inc"


#include "CSecretShop/dtor_CSecretShopStatistic.inc"


// [SplitC 2026-09-11 batch2 · TU 迁移] Clear 已迁出至 SecretShopStatistic.cpp（ORIG GI 键对齐）。



#include "CSecretShop/GetNpcPos.inc"


#include "CSecretShop/GetNpcIndex.inc"


#include "CSecretShop/GetDungeonData.inc"


#include "CSecretShop/RecordShow.inc"


#include "CSecretShop/RecordBuy.inc"


#include "CSecretShop/RecordPrice.inc"



#include "CSecretShop/SendSecretShopStatistic.inc"





#include "CSecretShop/CheckLimit.inc"


#include "CSecretShop/InsertItemIntoInventory.inc"


#include "CSecretShop/SendSecretShopBuyItem.inc"


#include "CSecretShop/LogCubeStatistic.inc"


#include "CSecretShop/LogValueStatistic.inc"


// ---- CBuyItembyGold（金币购买规则） ----
// 无 virtual 析构；ctor 委托 IBuyRule(CSecretShopStatistic&) 存 +0x04，vptr 置
// &_ZTVN10secretshop14CBuyItembyGoldE+0x8（BuyItem 于 vtable[0]，ORIG 确认）。

#include "CSecretShop/CBuyItembyGold.inc"


#include "CSecretShop/BuyItem_1.inc"


// ---- CBuyItembyRecipe（配方购买规则） ----
// 无 virtual 析构；ctor 委托 IBuyRule(CSecretShopStatistic&)，vptr 置
// &_ZTVN10secretshop16CBuyItembyRecipeE+0x8（BuyItem 于 vtable[0]，ORIG 确认）。

#include "CSecretShop/CBuyItembyRecipe.inc"


#include "CSecretShop/BuyItem_2.inc"



// ---- CSecretShop（ORIG 布局：map<eBuyRule,IBuyRule*> +0x00、
//      CMTRand* +0x18、CSecretShopStatistic +0x1c） ----

// ---- SecretShopScript 跨类方法（CDataManager +0xa700，ORIG 真实符号；
//      声明见 CDataManager.h，本 TU 直接真实调用） ----

#include "CSecretShop/dtor_CSecretShop.inc"


#include "CSecretShop/GetRule.inc"


#include "CSecretShop/LotteryNpc.inc"


#include "CSecretShop/LotteryItems.inc"


#include "CSecretShop/CheckLottery.inc"


#include "CSecretShop/BuyItem_3.inc"


#include "CSecretShop/SendSecretShopNpc.inc"


#include "CSecretShop/SendSecretShopItemList.inc"


#include "CSecretShop/SendSecretShopStatistic_1.inc"


}  // namespace secretshop
#include "CSecretShop/GetItemByDungeonIdx.inc"


#include "CSecretShop/GetItemByDungeonLev.inc"


#include "CSecretShop/GetItemByEvent.inc"


#include "CSecretShop/GetItems.inc"


#include "CSecretShop/IsExistItem.inc"


#include "CSecretShop/GetDomainRate.inc"


#include "CSecretShop/GetLevelIdx.inc"


#include "CSecretShop/GetRandItemPrice.inc"


#include "CSecretShop/CopyItem.inc"


#include "CSecretShop/clear_2.inc"


#include "CSecretShop/verifyNpc.inc"


#include "CSecretShop/GetSecretShopData.inc"

