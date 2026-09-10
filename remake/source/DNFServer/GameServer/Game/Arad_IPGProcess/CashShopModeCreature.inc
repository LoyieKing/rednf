void Arad_IPGProcess::CashShopModeCreature(CUser* user, unsigned long itemIdx,
                                           unsigned short stack, int& outCodeA,
                                           int& outCodeB, unsigned int accId,
                                           int characNo, char buyFlag)
{
    CItem* item = G_CDataManager()->find_item((int)itemIdx);
    if (item == 0)
        return;

    Inven_Item localItem;
    item->make_item(localItem);
    localItem.m_addInfo = (int)itemIdx;
    localItem.m_addInfo2 = (int)stack;

    if (buyFlag == 1)
    {
        // mangled _processCreatureGift(CUser*, Inven_Item, j, i, i, i)：
        //   esp 0x48=accId、0x4c=characNo、0x50=usablePeriod、0x54=expirationDate
        // P0：两 getter 结果无槽往返（ORIG ebx 跨 call 保活）⇒ 实参内联；
        // ORIG 求值序 = 右起（expirationDate 先 call、usablePeriod 后）。
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCreatureGift(
            user, localItem, accId, characNo, item->getUsablePeriod(),
            item->getExpirationDate());
    }
    else
    {
        // mangled _processCreature(CUser*, Inven_Item, Ri, Ri, b, i, i)
        reinterpret_cast<WongWork::CCeraShop*>(m_vm)->_processCreature(
            user, localItem, outCodeA, outCodeB, true, item->getUsablePeriod(),
            item->getExpirationDate());
    }
