bool CUser::DoGetGoldItem(int itemIdx, std::vector<std::pair<int, int> >& out,
                          bool bReset, ENUM_ERROR& err)
{
    int amount;
    if (itemIdx <= 0x28d291)
        amount = (itemIdx - 0x28d287) * 1000000;
    else
        amount = itemIdx * 10000000 - 0x549a800;

    if (bReset)
    {
        unsigned int limit = G_CDataManager()->GetMoneyLimitPerLevel(
            get_charac_level(), get_acc_name());
        if ((unsigned int)getCurCharacMoney() + (unsigned int)amount > limit)
        {
            err = (ENUM_ERROR)0xa;
            return false;
        }
    }
    else
    {
        CInventory* inven = getCurCharacInvenW();
        inven->gain_money(amount, (eMoneyAddReason)0xe, true, 0);
        // b28d-solo-CUser p167：ORIG 0x086928c2 实例化
        // `make_pair<ENUM_ITEMSPACE, Inven_Item::INVEN_SLOT>`（我方两 int）。
        out.push_back(std::make_pair(ENUM_ITEMSPACE_0, (Inven_Item::INVEN_SLOT)0));
    }
    return true;
