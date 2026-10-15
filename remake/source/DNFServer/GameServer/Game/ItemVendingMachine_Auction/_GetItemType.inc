// ---- _GetItemType —— ORIG 0x08176348 ----
// 函数局部 static Inven_Item temp（guard 惰性构造）；GetRelatedItem 取 CItem*
// 后经 vtable+0x08（ORIG vtable 槽 2 = make_item）拷入 temp，返回 temp.m_itemType
// （+0x01，ITEM_TYPE）。temp 每次调用先 reset（0x80cb7d8）。
Inven_Item::ITEM_TYPE ItemVendingMachine::_GetItemType(const CCeraShopGoods* goods)
{
    static Inven_Item temp;
    temp.reset();
    goods->GetRelatedItem()->make_item(temp);
    return (Inven_Item::ITEM_TYPE)temp.m_itemType;
}