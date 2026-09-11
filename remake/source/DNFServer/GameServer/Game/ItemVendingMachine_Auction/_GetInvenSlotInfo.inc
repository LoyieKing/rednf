// ---- _GetInvenSlotInfo —— ORIG 0x081763be ----
// 稀疏 switch 决策树（P46：case 数值升序 0/2/3/7；叶块排放 = 书写序）。
// groupIdx: 0→AVATAR；2→EQUIP+_GetItemType；3→CREATURE+_GetItemType；
// 7→EQUIP+itemType=2；其余保持调用方初值，恒 return true。
// [b28c-L19] AE：store 序 LHS-first 靠 _GetItemType 返回类型改 Inven_Item::ITEM_TYPE
// （P211 同型化，ItemVendingMachine.h:125 同步；mangled 不含返回类型）。
bool ItemVendingMachine::_GetInvenSlotInfo(const CCeraShopGoods* goods,
                                           INVEN_TYPE& invenType,
                                           Inven_Item::ITEM_TYPE& itemType)
{
    int groupIdx = goods->GetGroupIdx();
    switch (groupIdx)
    {
    case 0:
        invenType = INVEN_TYPE_AVATAR;
        break;
    case 2:
        invenType = INVEN_TYPE_EQUIP;
        itemType = _GetItemType(goods);
        break;
    case 3:
        invenType = INVEN_TYPE_CREATURE;
        itemType = _GetItemType(goods);
        break;
    case 7:
        invenType = INVEN_TYPE_EQUIP;
        itemType = (Inven_Item::ITEM_TYPE)2;
        break;
    }
    return true;
}