bool CTitleBook::_checkInsertTitle(ENUM_TITLE_BOOK_CATEGORY category,
                                   int slot, Inven_Item* item)
{
    if (!item)
    {
        return 0;
    }
    CItem* pItem = G_CDataManager()->find_item(item->m_addInfo);
    if (!pItem)
    {
        return 0;
    }
    if (pItem->is_stackable())
    {
        return 0;
    }
    if ((category == 3) && (item->m_seal != 0))
    {
        return 0;
    }
    return 1;
}