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
PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA::PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GA()
{
    memset(this, 0, 0x3d);
    m_header.setCategory(0);
    m_header.setPacketID(2);
    m_header.setSize(0x3d);
}

// ORIG 0x822e9b4（W 同址 C1/C2）
PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP::PCK_AUCTION_ASK_REGISTED_ITEM_NUM_GP()
{
    memset(this, 0, 0x2d);
    m_header.setCategory(0x12);
    m_header.setPacketID(2);
    m_header.setSize(0x2d);
}

// ORIG 0x822e6aa（W 同址 C1/C2）
PCK_AUCTION_SEARCH_BY_ITEMKEY_GA::PCK_AUCTION_SEARCH_BY_ITEMKEY_GA()
{
    memset(this, 0, 0x81);
    m_header.setCategory(0);
    m_header.setPacketID(6);
    m_header.setSize(0x81);
}

// ORIG 0x822ea84（W 同址 C1/C2）
PCK_AUCTION_SEARCH_BY_ITEMKEY_GP::PCK_AUCTION_SEARCH_BY_ITEMKEY_GP()
{
    memset(this, 0, 0x81);
    m_header.setCategory(0x12);
    m_header.setPacketID(6);
    m_header.setSize(0x81);
}

// ORIG W 0x822e694：仅 ROI_Category 成员构造（+0x9）。
TSearchByItemId_::TSearchByItemId_()
{
}

// ---------------------------------------------------------------------------
// stSeriaBlessingScript::FindOneByLv（ORIG W 0x822aebe / 0x822aecc）
// ---------------------------------------------------------------------------

stSeriaBlessingScript::FindOneByLv::FindOneByLv(int lv)
    : m_lv(lv)
{
}

bool stSeriaBlessingScript::FindOneByLv::operator()(
    const std::pair<const int, Value>& p) const
{
    if (p.second.m_minLv == p.second.m_maxLv)
    {
        if (p.second.m_minLv == m_lv)
            return true;
    }
    if (p.second.m_minLv > m_lv)
        return false;
    if (m_lv >= p.second.m_maxLv)
        return false;
    return true;
}

// ---------------------------------------------------------------------------
// stLevelUpRewardItemScript::GetGrowUpRewardItemFunctor
// （ORIG W 0x81b88e8 / 0x81b88fe）
// ---------------------------------------------------------------------------

stLevelUpRewardItemScript::GetGrowUpRewardItemFunctor::GetGrowUpRewardItemFunctor(
    const LevelUpRewardItemTableKey& key, std::vector<LevelUpRewardItemData>& out)
    : m_key(&key), m_out(&out)
{
}

void stLevelUpRewardItemScript::GetGrowUpRewardItemFunctor::operator()(
    const std::pair<const LevelUpRewardItemTableKey,
                    LevelUpRewardItemData>& p) const
{
    if (m_key->m_grow != p.first.m_grow)
        return;
    if (m_key->m_job != p.first.m_job)
        return;
    if ((int)p.first.m_lv > (int)m_key->m_lv)
        return;
    m_out->push_back(p.second);
}

// ---------------------------------------------------------------------------
// 头内联值类显式实例化 —— 生成 ORIG 同名 W 符号（C1/C2 同址别名）。
// ORIG 中这些类同为头内联（全 W），重建 .o 只需提供同名符号。
// ---------------------------------------------------------------------------
namespace
{

void forceInstantiateRandomOptionValueCtors()
{
    BaseItemSelectionValue a;
    BaseItemSelectionValue b(a);
    OptionGroupChoiceValue c;
    OptionGroupChoiceValue d(c);
    OptionGroupValue e;
    OptionGroupValue f(e);
    PartSelectonValue g;
    PartSelectonValue h(g);
    stCeraShopBonusInfo_t i;
    stCeraShopBonusInfo_t j(i);
    AradItemInfo k;
    AradItemInfo l(k);
    (void)b; (void)d; (void)f; (void)h; (void)j; (void)l;
}

}  // namespace
