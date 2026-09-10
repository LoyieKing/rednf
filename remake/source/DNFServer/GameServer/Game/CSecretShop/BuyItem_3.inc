void CSecretShop::BuyItem(CUser* user, SECRET_SHOP_INFO& info, int itemIdx, int count)
{
    // [b28d-solo-CSecretShop] P0：ORIG 0x085fb241/0x085fb24e 中 dungeonIdx 留 %ebx
    // 跨 GetSecretShopData 调用、npcIdx 直读解引用，均无槽往返 ⇒ 内联实参（右→左
    // 求值序与 ORIG 一致）；局部 sale/rule/item 有槽（-0x18/-0x14/-0x10）保留。
    SALE_INFO* sale = info.GetSaleInfo(itemIdx);
    if (sale == 0)
        return;
    IBuyRule* rule = GetRule((eBuyRule)sale->m_rule);
    if (rule == 0)
        return;
    CItem* item = G_CDataManager()->find_item(itemIdx);
    if (item == 0)
        return;
    if (!item->is_stackable())
        count = 1;
    // [b28d-solo-CSecretShop] ORIG 0x085fb21f `mov %al,-0x9; cmpb $0x0,-0x9; je`
    // ⇒ BuyItem 结果有具名 bool 局部；0x085fb22b `movzbl m_bClear; xor $1; test; je`
    // ⇒ 字段 bool 裸否定（P292 档②，勿写 `!= 1`）。
    bool bBuy = rule->BuyItem(user, info, itemIdx, count);
    if (bBuy) {
        if (!info.m_bClear) {
            m_statistic.RecordBuy(*(int*)user->GetSecretShopData(),
                                  user->getDungeonIdxAfterClear());
            info.m_bClear = 1;
        }
    }
