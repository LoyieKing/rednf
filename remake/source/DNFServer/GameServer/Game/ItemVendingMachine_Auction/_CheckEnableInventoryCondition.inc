// ---- _CheckEnableInventoryCondition —— ORIG 0x081768ba ----
// getCurCharacInvenR()->GetRemainCapacity(invenType,itemType) > 0，或物品
// get_index ∈ {200,0xcb,0xcc,0xcd,5,6,7} → true。ORIG 形态：条件真路径
// 统一 je/jg 到 mov $0，全假 fallthrough mov $1，再 test/je 双常反转
// （对应 if (!(C)) return false; return true; 逐字同构）。
bool ItemVendingMachine::_CheckEnableInventoryCondition(CUser* user, const CItem* item,
                                                        INVEN_TYPE invenType,
                                                        Inven_Item::ITEM_TYPE itemType)
{
    if (!(user->getCurCharacInvenR()->GetRemainCapacity(invenType, itemType) > 0 ||
          item->get_index() == 200 ||
          item->get_index() == 0xcb ||
          item->get_index() == 0xcc ||
          item->get_index() == 0xcd ||
          item->get_index() == 5 ||
          item->get_index() == 6 ||
          item->get_index() == 7))
        return false;
    return true;
}