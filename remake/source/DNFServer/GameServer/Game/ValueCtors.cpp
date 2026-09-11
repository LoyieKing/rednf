// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 ValueCtors/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <string.h>   // memset
#include <utility>

#include "StringTypes.h"   // CharString（AradItemInfo 依赖链）

#include "RandomOptionScript.h"   // PartSelectonValue/BaseItemSelectionValue/
                                  // OptionGroupChoiceValue/OptionGroupValue
#include "CItemList.h"            // stCeraShopBonusInfo_t
#include "AradItemInfo.h"
#include "CDataManager.h"         // stSeriaBlessingScript/stLevelUpRewardItemScript

#include "PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA.h"
#include "PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP.h"
#include "PCK_AUCTION_SEARCH_BY_ITEMKEY_GA.h"
#include "PCK_AUCTION_SEARCH_BY_ITEMKEY_GP.h"

// ---------------------------------------------------------------------------
// PCK_AUCTION 包类 ctor（ORIG 序列：成员 ctor → memset → setCategory →
// setPacketID → setSize；成员构造由编译器在函数体前自动发射）
// ---------------------------------------------------------------------------

#include "ValueCtors/PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA.inc"


#include "ValueCtors/PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP.inc"


#include "ValueCtors/PCK_AUCTION_SEARCH_BY_ITEMKEY_GA.inc"


#include "ValueCtors/PCK_AUCTION_SEARCH_BY_ITEMKEY_GP.inc"


#include "ValueCtors/TSearchByItemId_.inc"


// ---------------------------------------------------------------------------
// stSeriaBlessingScript::FindOneByLv（ORIG W 0x822aebe / 0x822aecc）
// ---------------------------------------------------------------------------

#include "ValueCtors/FindOneByLv.inc"


#include "ValueCtors/operator.inc"


// ---------------------------------------------------------------------------
// stLevelUpRewardItemScript::GetGrowUpRewardItemFunctor
// （ORIG W 0x81b88e8 / 0x81b88fe）
// ---------------------------------------------------------------------------

#include "ValueCtors/GetGrowUpRewardItemFunctor.inc"


#include "ValueCtors/operator_1.inc"


// ---------------------------------------------------------------------------
// [AnonLane2 2026-09-11 试形-回滚] 曾删本垫片：orphan-W=0/519（目标 W 符号全库
// 均有其它定义者，链接后符号不丢），但本 TU 四数 88/218/0/0/1 → 24/36/0/0/0
// （identical −64、ae −182，符号归属转移）⇒ 按 TU 四数口径回滚，判词保留。
// 头内联值类显式实例化 —— 生成 ORIG 同名 W 符号（C1/C2 同址别名）。
// ORIG 中这些类同为头内联（全 W），重建 .o 只需提供同名符号。
// ---------------------------------------------------------------------------
namespace
{

#include "ValueCtors/forceInstantiateRandomOptionValueCtors.inc"


}  // namespace
