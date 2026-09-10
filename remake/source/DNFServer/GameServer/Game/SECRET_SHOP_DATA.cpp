// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是聚合器：
//   每个顶层函数已移入 SECRET_SHOP_DATA/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件**逐行保留**原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图），只把函数定义块换成一行 #include。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 每个 `#include` 的**位置即原函数定义位置**，改动顺序会改 codegen。
// ============================================================================
// df_game_r secretshop::SECRET_SHOP_DATA 还原（G2-5 批次）。
// CParty +0x2a4，总尺寸 0x88。符号（nm -C）：
//   C1 0x082a5422 / D1 0x082a549e / clear 0x08ab14b8 /
//   IsCompleteBuy 0x08ab155a / IsOpen 0x08ab150c / SetBuying 0x08ab1532；
//   附带 SECRET_SHOP_INFO：C1 0x082a5396 / D1 0x082a53dc /
//   clear 0x08ab1642 / GetSaleInfo 0x085fbe8a。
// 布局（由 ctor/clear/IsOpen/SetBuying 反汇编推导）：
//   SECRET_SHOP_DATA：+0x00 int m_openCode、+0x04 SECRET_SHOP_INFO[4]
//   （0x1c 每项）、+0x74 char m_buying[4]、+0x78..+0x87 保留区。
//   SECRET_SHOP_INFO：+0x00 RETAILER（vector<SALE_INFO>）、+0x0c SHOPPER
//   （vector<BUY_INFO>）、+0x18 char m_bClear。
// RETAILER/SHOPPER 类声明与实现见 SECRET_SHOP_DATA.h / CSecretShop.cpp
// （本 TU 不再定义，直接以真实 C++ 成员调用）。

#include "SECRET_SHOP_DATA.h"

namespace secretshop
{

#include "SECRET_SHOP_DATA/SECRET_SHOP_INFO.inc"
}

#include "SECRET_SHOP_DATA/dtor_SECRET_SHOP_INFO.inc"
}

#include "SECRET_SHOP_DATA/clear.inc"
}

#include "SECRET_SHOP_DATA/GetSaleInfo.inc"
}

#include "SECRET_SHOP_DATA/SECRET_SHOP_DATA.inc"
}

#include "SECRET_SHOP_DATA/dtor_SECRET_SHOP_DATA.inc"
}

#include "SECRET_SHOP_DATA/clear_1.inc"
}

#include "SECRET_SHOP_DATA/IsCompleteBuy.inc"
}

#include "SECRET_SHOP_DATA/IsOpen.inc"
}

#include "SECRET_SHOP_DATA/SetBuying.inc"
}

}  // namespace secretshop
