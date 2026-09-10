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
    // ORIG 0x85f77e4：三条件常量岛汇聚（mov$1/jmp/mov$0）后 test 直判
    if (item.isEquipableItemType() && ci->get_rarity() == 0 &&
        item.GetUpgrade() == 0)
        return 0;
    // ORIG 0x085f77f4 调用 CItem::IsBanRedeemItem()（非内联，生成 call）；
    // 后随 `test %al,%al; je` 直判（无 setne 归一）。
    // [b28d-L28 round2 实测] 调用点由 `!= 0` 改隐式转 bool **无效**（4.4 对
    // 非 bool 返回的条件恒插 `test; setne %al; test; je`，99/101 不变）⇒ 正解在
    // **被调方返回类型**（CItem::IsBanRedeemItem 应 bool，属 CItem 头，需 spec 上报）。
    if (ci->IsBanRedeemItem() != 0)
        return 0;
    return 1;
