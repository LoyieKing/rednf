void CItemList::get_lotto_item(int idx, char* out)
{
    Inven_Item* pItem = (Inven_Item*)out;
    CItem* item = find_item(idx);
    if (!item)
    {
        pItem->m_addInfo = 0;
        pItem->set_add_info(0);
        pItem->m_endurance = 0;
        pItem->m_itemType = 0;
        pItem->ResetItemAttr();
        return;
    }
    if (!item->is_stackable())
    {
        pItem->m_addInfo = 0;
        pItem->set_add_info(0);
        pItem->m_endurance = 0;
        pItem->m_itemType = 1;
        pItem->ResetItemAttr();
    }
    else
    {
    if (((CStackableItem*)item)->m_nItemType != 6 &&
        ((CStackableItem*)item)->m_nItemType != 7 &&
        ((CStackableItem*)item)->m_nItemType != 0xd)
        {
            pItem->m_addInfo = 0;
            pItem->set_add_info(0);
            pItem->m_endurance = 0;
            pItem->m_itemType = 2;
            pItem->ResetItemAttr();
        }
        else
        {
            ((CStackableItem*)item)->get_lotto_item(out);
        }
    }
}