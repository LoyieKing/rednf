// df_game_r Redeem_Item::CRedeemItem —— 恢复实现（独立 TU）。
// 语义依据 docs/class_func_reports/Redeem_Item__CRedeemItem/（ORIG 反汇编 + Ghidra C）。
// 仅实现 CUser::Add_RedeemInfo 调用的两个方法 + 私有辅助 _isFPItem；
// 其余 ORIG 方法（ctor/dtor/loadData/_saveData/GetItemPrice/RedeemItem 等）未建模。

#include "Redeem_Item__CRedeemItem.h"

#include "CDataManager.h"
#include "CItem.h"

namespace Redeem_Item
{

// ORIG 0x085f7c04：itemId 在 (0x1963, 0x1b57] 区间 → true（"FP 物品"判定）。
bool CRedeemItem::_isFPItem(const Inven_Item& item) const
{
    int itemId = item.m_addInfo;
    return itemId > 0x1963 && itemId <= 0x1b57;
}

// ORIG 0x085f76f0：空物品/FP 物品/固定 ID 表/物品不存在/低稀有度可升级
// 装备/禁兑换物品 → 0；否则 1。
bool CRedeemItem::isAddableFilter(const Inven_Item& item)
{
    if (item.isEmpty())
        return 0;
    if (_isFPItem(item))
        return 0;
    switch (item.m_addInfo)
    {
    case 0x114c:
    case 0xcf1:
    case 0xbdd:
    case 0x1057:
    case 0x10fe:
    case 0x1157:
    case 0x1159:
    case 0x1973:
    case 0x1ee1:
    case 0x28e770:
        return 0;
    default:
        break;
    }
    CItem* ci = G_CDataManager()->find_item(item.m_addInfo);
    if (ci == 0)
        return 0;
    bool bVar2;
    if (!item.isEquipableItemType() || ci->get_rarity() != 0 ||
        item.GetUpgrade() != 0)
    {
        bVar2 = false;
    }
    else
    {
        bVar2 = true;
    }
    if (bVar2)
        return 0;
    // ORIG CItem::IsBanRedeemItem() = 读 CItem+0x144（m_char144）
    if (ci->m_char144 != 0)
        return 0;
    return 1;
}

// ORIG 0x085f71a8：构造 Inven_Item_Expand_RedeemInfo（复制物品前 0x3c 字节 +
// 末字节 + count=param2+1000/1000），alter()，list（this+8）超 0x14 项时
// pop_front，push_back，返回 1。
int CRedeemItem::AddRedeemList(const Inven_Item& item, int param2, bool param3)
{
    Inven_Item_Expand_RedeemInfo info;
    std::memcpy(&info.m_data, &item, 0x3c);
    info.m_byte3c = *((const unsigned char*)&item + 0x3c);
    info.m_count40 = param3 ? (param2 + 1000) : 1000;
    alter();
    if (m_redeemList.size() > 0x14)
        m_redeemList.pop_front();
    m_redeemList.push_back(info);
    return 1;
}

}  // namespace Redeem_Item
