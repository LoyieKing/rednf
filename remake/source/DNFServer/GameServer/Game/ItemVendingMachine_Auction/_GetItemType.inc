Inven_Item::ITEM_TYPE ItemVendingMachine::_GetItemType(const CCeraShopGoods* goods)
{
    static Inven_Item temp;
    temp.reset();
    goods->GetRelatedItem()->make_item(temp);
    return (Inven_Item::ITEM_TYPE)temp.m_field1;
