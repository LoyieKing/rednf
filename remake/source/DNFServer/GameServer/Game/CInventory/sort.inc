int CInventory::sort(INVEN_TYPE invenType)
{
    if (invenType == 1)
    {
        int start;
        int end;
        for (int i = 1; i < 5; i++)
        {
            start = 0;
            end = 0;
            StartEndPos((Inven_Item::ITEM_TYPE)i, start, end);
            qsort(m_pEquipSlot + start, end - start + 1, 0x3d, _CompareSlot);
        }
        int start2 = 0;
        int end2 = 0;
        StartEndPos((Inven_Item::ITEM_TYPE)10, start2, end2);
        qsort(m_pEquipSlot + start2, end2 - start2 + 1, 0x3d, _CompareSlot);
        return 0;
    }
    if (invenType == 3)
    {
        for (int i = 6; i < 8; i++)
        {
            int start = 0;
            int end = 0;
            StartEndPos((Inven_Item::ITEM_TYPE)i, start, end);
            qsort(m_pCreatureSlot + start, end - start + 1, 0x3d, _CompareSlot);
        }
        return 0;
    }
    return 1;
