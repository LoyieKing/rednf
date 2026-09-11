// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CCeraShopGoods/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <map>
#include <string>
#include <vector>

#include "CItemList.h"
#include "CDataManager.h"

// ============================================================================
// STCeraShopItem
// ============================================================================

#include "CCeraShopGoods/STCeraShopItem.inc"

#include "CCeraShopGoods/clear.inc"

// ---- operator== @ 0x891a89e ----
bool STCeraShopItem::operator==(const STCeraShopItem& other)
{
    // ORIG 0x0891a8a1：先载 other（0xc(%ebp)）后载 this，cmp %eax,%edx。
    if (other.m_field8 == m_field8)
        return true;
    return false;
}

#include "CCeraShopGoods/getCeraPrice.inc"

#include "CCeraShopGoods/getGoldPrice.inc"

#include "CCeraShopGoods/isLimitSaleItem.inc"

// ============================================================================
// CCeraShopGoods
// ============================================================================

#include "CCeraShopGoods/SetItem.inc"

#include "CCeraShopGoods/CCeraShopGoods.inc"

#include "CCeraShopGoods/GetItemIdx.inc"

#include "CCeraShopGoods/GetStackCount.inc"

#include "CCeraShopGoods/GetGoldPrice.inc"

#include "CCeraShopGoods/GetWinPoint.inc"

#include "CCeraShopGoods/GetCeraPrice.inc"

#include "CCeraShopGoods/GetMileagePrice.inc"

#include "CCeraShopGoods/GetSubGroupIndex.inc"

#include "CCeraShopGoods/GetGroupIdx.inc"

#include "CCeraShopGoods/GetFeaturedIdx.inc"

#include "CCeraShopGoods/GetSticker.inc"

#include "CCeraShopGoods/GetExpiryDate.inc"

#include "CCeraShopGoods/GetBonusWeight.inc"

#include "CCeraShopGoods/GetRelatedItem.inc"

#include "CCeraShopGoods/isGiftDisallowance_.inc"

#include "CCeraShopGoods/isCeraPointDisallowance_.inc"

#include "CCeraShopGoods/isStackBuyable.inc"

// ============================================================================
// importCashShopItemList（商城脚本装载，ORIG 0x88fdcb5）
// 语义：加载 RDAR 商城脚本；按分区扫描商品行（每行首字段为条目序号），
// 以 STCeraShopItem 逐项填入商品向量；[limit] 类分区填限购映射。
// ============================================================================
// 全局 C++ 权威签名（由 DNFLexWrapperHelpers.cpp 提供，匹配 ORIG mangled 符号）：
bool loadRDARScriptFile(const char* directory, const char* path);
bool ScanType(std::string& token, bool consume);
int ScanInt(bool* ok);
bool ScanStr(std::string* value);

// [BigRndOpt] 本 TU 内 stCeraShopBonusItemInfo_t 的 trivial 视图（用途与依据见
// importCashShopItemList 的 [bonus item info] 段判词）。ORIG 0x088ff2c4..0x088ff36f
// 该段全程 0 ctor / 0 dtor：三字段（u32@0 / u16@4 / u32@8，sizeof 0xc）直落栈槽
// -0xc0/-0xbc/-0xb8，再以槽地址作 push_back 实参。共享头 CItemList.h 为补发弱符号
// _ZN25stCeraShopBonusItemInfo_tD1Ev（ORIG W 0x089363da，宿主 W9_StructMisc.cpp）
// 声明了用户析构 ⇒ 具名 bi 会多发 2 处 D1Ev（p167 EXTRA +2 的唯一来源）。
// 删共享头声明属跨 TU（R1 禁区），故此处改用同布局 POD 镜像承担局部量：
// 两侧皆 standard-layout、公共初始序列一致，reinterpret_cast 取引用合法，
// 零导出符号、零可观察行为变化。
namespace
{
struct BonusItemView
{
    unsigned int m_field0;        // +0x00（ORIG `mov %eax,-0xc0(%ebp)`）
    unsigned short m_field4;      // +0x04（ORIG `mov %ax,-0xbc(%ebp)`）
    unsigned int m_field8;        // +0x08（ORIG `mov %eax,-0xb8(%ebp)`）
};
// 0xc 的三重实证（详见 CItemList.h:156 同题注释）：max_size@0x08936166 = UINT_MAX/12、
// allocate@0x08945d62 = n*3*4、使用点元素槽 -0xc0..-0xb4。
static_assert(sizeof(BonusItemView) == 0xc, "ORIG stCeraShopBonusItemInfo_t = 0xc (0x08936166/0x08945d62)");
static_assert(sizeof(BonusItemView) == sizeof(stCeraShopBonusItemInfo_t),
              "镜像视图须与共享头同尺寸（偏移 0/4/8 逐条按 ORIG 反汇编核对）");
}   // namespace

#include "CCeraShopGoods/importCashShopItemList.inc"
