size_t CInventory::getSortedKeyArray(unsigned long* out, int maxCount)
{
    int i = 0;
    for (i = 0; i < maxCount; i++)
    {
        out[i] = 0;
    }
    int count = 0;
    for (i = 0; i < 0x1a; i++)
    {
        Inven_Item* pInvenItem = &m_invenItem[i];
        if (pInvenItem->m_addInfo != 0)
        {
            out[count] = pInvenItem->getKey();
            count++;
            if (maxCount <= (int)count)
            {
                qsort(out, count, 4, my_compare_unsigned_short);
                return count;
            }
        }
    }
    i = 0;
    for (i = 0; i < 0x138; i++)
    {
        Inven_Item* pEquipItem = &m_pEquipSlot[i];
        if (pEquipItem->m_addInfo != 0)
        {
            out[count] = pEquipItem->getKey();
            count++;
            if (maxCount <= (int)count)
            {
                qsort(out, count, 4, my_compare_unsigned_short);
                return count;
            }
        }
    }
    i = 0;
    for (i = 0; i < 0x69; i++)
    {
        Inven_Item* pAvatarItem = &m_pAvatarSlot[i];
        if (pAvatarItem->m_addInfo != 0)
        {
            out[count] = pAvatarItem->getKey();
            count++;
            if (maxCount <= (int)count)
            {
                qsort(out, count, 4, my_compare_unsigned_short);
                return count;
            }
        }
    }
    i = 0;
    for (i = 0; i <= 0xf1; i++)
    {
        Inven_Item* pCreatureItem = &m_pCreatureSlot[i];
        if (pCreatureItem->m_addInfo != 0)
        {
            out[count] = pCreatureItem->getKey();
            count++;
            if (maxCount <= (int)count)
            {
                qsort(out, count, 4, my_compare_unsigned_short);
                return count;
            }
        }
    }
    qsort(out, count, 4, my_compare_unsigned_short);
    return count;
}