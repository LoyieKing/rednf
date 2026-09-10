// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 ValueCtors/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// ============================================================================
// ValueCtors.cpp —— 小型值类型构造/析构/嵌套 functor 落地（OnlyOrigBatch1）。
//
// 内容：
//   1. PCK_AUCTION_* 4 个包类 ctor（ORIG 0x822e54e/0x822e9b4/0x822e6aa/0x822ea84，
//      成员布局见各自 .h；ORIG 为 W 头内联形态，Game 域以骨架头 + 本 TU 定义）。
//   2. TSearchByItemId_ ctor（ORIG W 0x822e694：仅 ROI_Category 成员构造）。
//   3. stSeriaBlessingScript::FindOneByLv C1/operator()（ORIG W 0x822aebe/0x822aecc）。
//   4. stLevelUpRewardItemScript::GetGrowUpRewardItemFunctor C1/operator()
//      （ORIG W 0x81b88e8/0x81b88fe）。
//   5. 头内联值类的显式实例化（生成 W 符号）：
//      BaseItemSelectionValue/OptionGroupChoiceValue/OptionGroupValue/
//      PartSelectonValue/stCeraShopBonusInfo_t/AradItemInfo 的 C1/C2/C1ERKS/C2ERKS。
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

// ORIG 0x822e54e（W 同址 C1/C2）
#include "ValueCtors/PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA.inc"
}

// ORIG 0x822e9b4（W 同址 C1/C2）
#include "ValueCtors/PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP.inc"
}

// ORIG 0x822e6aa（W 同址 C1/C2）
#include "ValueCtors/PCK_AUCTION_SEARCH_BY_ITEMKEY_GA.inc"
}

// ORIG 0x822ea84（W 同址 C1/C2）
#include "ValueCtors/PCK_AUCTION_SEARCH_BY_ITEMKEY_GP.inc"
}

// ORIG W 0x822e694：仅 ROI_Category 成员构造（+0x9）。
#include "ValueCtors/TSearchByItemId_.inc"
}

// ---------------------------------------------------------------------------
// stSeriaBlessingScript::FindOneByLv（ORIG W 0x822aebe / 0x822aecc）
// ---------------------------------------------------------------------------

#include "ValueCtors/FindOneByLv.inc"
}

#include "ValueCtors/operator.inc"
}

// ---------------------------------------------------------------------------
// stLevelUpRewardItemScript::GetGrowUpRewardItemFunctor
// （ORIG W 0x81b88e8 / 0x81b88fe）
// ---------------------------------------------------------------------------

#include "ValueCtors/GetGrowUpRewardItemFunctor.inc"
}

#include "ValueCtors/operator_1.inc"
}

// ---------------------------------------------------------------------------
// 头内联值类显式实例化 —— 生成 ORIG 同名 W 符号（C1/C2 同址别名）。
// ORIG 中这些类同为头内联（全 W），重建 .o 只需提供同名符号。
// ---------------------------------------------------------------------------
namespace
{

#include "ValueCtors/forceInstantiateRandomOptionValueCtors.inc"
}

}  // namespace
