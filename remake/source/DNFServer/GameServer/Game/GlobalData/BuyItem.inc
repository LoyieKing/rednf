bool ItemVendingMachine::BuyItem(CUser* user, const MSG_BUY_CERASHOP_ITEM& msg)
{
    // ORIG 0x0854c4d0：仅委托 ProcessBuyRequestEx(this, user, msg) 后返回 1。
    ProcessBuyRequestEx(user, msg);
    return 1;
