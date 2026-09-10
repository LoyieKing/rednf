bool CBuyItembyRecipe::BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx,
                               int count)
{
    // [b28d-solo-CSecretShop] P0/P183：ORIG 0x085fbc52 先构造 pair 再直写
    // cubeStat.first/second（无 materialIdx/need 独立槽）；invenR/invenW/invenR2
    // 结果均无槽往返 ⇒ 全内联；remain2 复用 cubeStat.second 的 if/else 双写
    // （ORIG -0x24 两处 store）；need 跨 getCurCharacInvenW 由 %ebx 保活。
    SALE_INFO* sale = info.m_retailer.GetSaleInfo(itemIdx);
    unsigned int err = GlobalData::s_pSecuProtectionField->Check(user, (SECURITY_PROTCTION)0x43);
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
    std::pair<int, int> cubeStat;
    cubeStat.first = sale->m_material;
    cubeStat.second = sale->m_materialCount * count;
    Inven_Item material;
    int materialSlot = ((CInventory*)user->getCurCharacInvenR())
                           ->GetInvenData(cubeStat.first, material);
    if (materialSlot == -1 || material.m_addInfo2 < cubeStat.second) {
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
        ->delete_item((INVEN_TYPE)1, materialSlot, cubeStat.second,
                      (eItemDelReason)0x1a, 1);
    const Inven_Item* ref = ((const CInventory*)user->getCurCharacInvenR())
                                ->GetInvenRef(1, materialSlot);
    if (ref == 0)
        cubeStat.second = 0;
    else
        cubeStat.second = ref->m_addInfo2;
    SendSecretShopBuyItem(user, slot, item, cubeStat.first, cubeStat.second, remain);
    LogCubeStatistic(user, cubeStat);
    info.m_shopper.BuyItem(itemIdx, count);
    return 1;
