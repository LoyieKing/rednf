int CInventory::GetExistCreatureSlot(Inven_Item& item) const
{
    int start = 0;
    int end = 0;
    StartEndPos((Inven_Item::ITEM_TYPE)item.m_itemType, start, end);
    for (int i = start; i <= end; i++)
    {
        if (m_pCreatureSlot[i].m_addInfo == item.m_addInfo)
        {
            return i;
        }
    }
    return -1;
}