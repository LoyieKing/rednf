int Store::GetSellItemPrice(CUser* user, char invenType, short slot, short count,
                            bool flag, int& outPrice, int& outCount)
{
    Inven_Item item;

    switch (invenType)
    {
    case 0:
        item = user->getCurCharacInvenR()->GetInvenSlot(1, slot);   // 消息 0 → type 1
        break;
    case 2:
        item = user->getCurCharacCargoR()->get_cargo_slot(slot);
        break;
    case 3:
        // creature slot
        item = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)0, slot);   // 消息 3 → type 0
        break;
    case 7:
        // avatar slot
        item = user->getCurCharacInvenR()->GetInvenSlot((INVEN_TYPE)3, slot);   // 消息 7 → type 3
        if (item.m_field1 == 5)   // ORIG 分支独占检查：ITEM_TYPE==5 → 0x11（objdump 0x861932c）
            return 0x11;
        break;
    default:
        return 0x11;   // ORIG 0x0861933e：default 返 0x11
    }

    if (item.m_addInfo == 0)   // ORIG 共享段检查（find_item 前，m_addInfo 直读）
    {
        return 0x11;
    }
    // ORIG 0x0861936b：call CDataManager::find_item(int) const（非 m_itemList）。
    const CItem* pItem = G_CDataManager()->find_item(item.m_addInfo);

    if (pItem == NULL)
    {
        return 0x11;
    }

    if (pItem->GetFinishPointPrice() != 0)
        return 0x17;
    // ORIG 0x086193a6：`call GetAttachType; cmp $0x2,%eax; jne <body>` 直跳
    // （4.4 对 if 比较恒物化 setne/test/je，P197④ 镜像族）⇒ 单 case switch（P286）。
    switch (pItem->GetAttachType())
    {
    case 2:
        return 0x17;
    default:
        break;
    }
    // ORIG 0x086193b5：第 6 参（0x20(%ebp)）= get_index()。
    outPrice = pItem->get_index();
    // ORIG 0x086193c2：第 7 参（0x24(%ebp)）直传下层价格出参。
    return GetSellItemPrice(item, pItem, count, flag, outCount);
