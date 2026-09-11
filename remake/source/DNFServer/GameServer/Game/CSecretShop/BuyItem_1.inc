bool CBuyItembyGold::BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                             int count)
{
    // [b28d-solo-CSecretShop] P0/P48：ORIG 0x085fb8f4 的 inven（0x085fba8d 结果
    // 直接作 use_money 的 this）、dungeonIdx（%ebx 跨 GetSecretShopData）、
    // m_pStatistic（call 前直读 this+4）均无槽往返 ⇒ 全部内联；
    // CheckLimit 已 bool 化（见头声明）复现 `call; xor $1; test; je`。
    // 残余：money 比较 `cmp mem,reg; setl` vs 4.4 直折（setcc-fold 族）记 N。
    SALE_INFO* sale = info.m_retailer.GetSaleInfo(itemIdx);
    unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x42);
    if (err != 0)
    {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, static_cast<char>(err));
        return 0;
    }
    if (sale == 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 0x11);
        return 0;
    }
    int remain = 0;
    if (!CheckLimit(info, itemIdx, count, remain)) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 0x5f);
        return 0;
    }
    // [b28d-solo-CSecretShop] total 声明 int：ORIG 0x085fb9f8 `cmp -0x10(%ebp),%eax;
    // setl` 为内存直比；unsigned 局部 + (int) cast 会多一条 `mov %edx` 中转（P41 变体）。
    int total = sale->m_price * count;
    if (user->getCurCharacMoney() < total) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 10);
        return 0;
    }
    Inven_Item item;
    int slot = InsertItemIntoInventory(user, itemIdx, count, item);
    if (slot < 0) {
        user->SendCmdErrorPacket((ENUM_CMDPACKET)0x129, 4);
        return 0;
    }
    ((CInventory*)user->getCurCharacInvenW())
        ->use_money(total, (eMoneySubReason)0x27, 1);
    SendSecretShopBuyItem(user, slot, item, -1, 0, remain);
    LogValueStatistic(user, total);
    info.m_shopper.BuyItem(itemIdx, count);
    m_pStatistic->RecordPrice(*(int*)user->GetSecretShopData(),
                              user->getDungeonIdxAfterClear(), total);
    return 1;
}