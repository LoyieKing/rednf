// _CheckEnableGiftCondition —— ORIG 0x08176722 (T)
bool ItemVendingMachine::_CheckEnableGiftCondition(const CCeraShopGoods* goods,
                                                   CUser* user, char c)
{
    if (user->IsGameMasterMode())
        return true;
    if (goods->GetSticker() == 99)
        return false;
    if (c == 1 && goods->isGiftDisallowance_())
        return false;
    return true;
}