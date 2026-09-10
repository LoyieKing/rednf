void WarRoom::RemoveKC()
{
    for (int i = 0; i <= 5; ++i)
    {
        if (m_slots[i] != 0)
        {
            (m_slotsUser[i])->InitFinishPoint();
            ((CInventory*)(m_slotsUser[i])->getCurCharacInvenW())->RemoveKCItem();
        }
    }
