bool CInventory::IsEquipAvatar() const
{
    for (int i = 0; i <= 9; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            return true;
        }
    }
    return false;
