int CTitleBook::putTitleOnly(CUser* user, int itemIdx,
                             ENUM_TITLE_BOOK_CATEGORY& category, int& slot)
{
    // [b28c-6] N7 残差（ndiff 22→12）：唯一差异为 pItem->make_item 虚调用
    // 目标寄存器 %edx↔%eax 与 this 中转寄存器互换（N7）。
    // P92 已修：_getFitSlot 首参从局部 item.m_addInfo 读（ORIG -0x4b(%ebp)）。
    CTitleBook* book = (CTitleBook*)user->GetCharacExpandData(
        (ENUM_CHARAC_EXPAND_TYPE)0xe);
    if (!book)
    {
        return 0;
    }
    if (itemIdx == 0)
    {
        return 0;
    }
    Inven_Item item;
    item.m_addInfo = itemIdx;
    CItem* pItem = G_CDataManager()->find_item(itemIdx);
    if (!pItem)
    {
        return 0;
    }
    pItem->make_item(item);
    item.set_add_info(1);
    // P92：ORIG 0x8641e88 段实参从局部 item 的 +0x02（-0x4b(%ebp)）读取，
    // 非形参 itemIdx（0xc(%ebp)）——两值相等（item.m_addInfo = itemIdx）。
    if (!_getFitSlot(item.m_addInfo, category, slot))
    {
        return 0;
    }
    if (!book->_checkInsertTitle(category, slot, &item))
    {
        return 0;
    }
    book->_insertTitle(category, slot, &item);
    return 1;
}