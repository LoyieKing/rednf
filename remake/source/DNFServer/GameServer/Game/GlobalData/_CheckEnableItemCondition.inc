bool ItemVendingMachine::_CheckEnableItemCondition(const CCeraShopGoods* goods,
                                                   const CItem*& item,
                                                   std::vector<std::pair<int, int> >&)
{
    if (goods->GetGroupIdx() != 1)
    {
        // ORIG 0x081769c3：&& 短路结果落 %eax(0/1) 后 `test %al,%al; je`
        // 物化判（无局部槽）——(bool) 显式物化形。
        if ((bool)(goods->GetGroupIdx() == 11 && goods->GetSubGroupIndex() == 1))
            return true;
        item = goods->GetRelatedItem();
        if (item == 0)
            return false;
    }
    return true;
