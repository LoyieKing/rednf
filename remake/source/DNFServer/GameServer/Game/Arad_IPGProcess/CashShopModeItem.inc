void Arad_IPGProcess::CashShopModeItem(CUser* user, unsigned long itemIdx,
                                       unsigned short stack, int count, long seq,
                                       std::vector<Inven_Item>& out,
                                       WongWork::CCeraShop::stCeraShopItemParam_t& param,
                                       int& restrictOut, unsigned int accId,
                                       int characNo, char buyFlag)
{
    CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item == 0)
        return;

    Inven_Item localItem;
    item->make_item(localItem);
    localItem.m_addInfo = (int)itemIdx;    // ORIG -0x47 = item +2
    localItem.m_addInfo2 = (int)stack;     // ORIG -0x42 = item +7

    if (buyFlag == 1)
    {
        // mangled _processItemGift(CUser*, Inven_Item, j, i)：esp 0x48=accId、0x4c=characNo
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processItemGift(user, localItem, accId, characNo);
    }
    else
    {
        // mangled _processItem(CUser*, Inven_Item, i count, Ri restrict, l seq,
        //   b 0, b 1, Rvector, RN..param_t&)
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processItem(
            user, localItem, count, restrictOut, seq, false, true, out, param);
    }
