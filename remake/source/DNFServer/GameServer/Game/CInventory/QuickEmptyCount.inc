int CInventory::QuickEmptyCount() const
{
    int count = 0;
    for (int i = 3; i < 9; i++)
    {
        if (m_pEquipSlot[i].m_addInfo == 0)
        {
            count++;
        }
    }
    return count;
}