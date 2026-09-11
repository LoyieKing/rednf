// _onBoosterItemUseResult(CUser*, vector<Inven_Item>&, int) —— ORIG 0x082099bc
void Dispatcher_UseBoosterItem::_onBoosterItemUseResult(CUser* user, std::vector<Inven_Item>& items, int itemId)
{
    // ORIG：begin/end 迭代器各构造一次（P13 end 缓存；P50 for 内直构），
    // 循环体调 sendBroadCastItems(this, user, *it, itemId)。
    for (std::vector<Inven_Item>::const_iterator it = items.begin(), e = items.end();
         it != e; ++it)
        sendBroadCastItems(user, *it, itemId);
}