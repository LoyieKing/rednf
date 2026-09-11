void CInventory::ShowAllExistItemsForErrorCheck() const
{
    for (int i = 3; i < 0x138; i++)
    {
        if (m_pEquipSlot[i].m_addInfo != 0)
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__, 0x1af0,
                "Inventory(%d) : %d\n", i, m_pEquipSlot[i].m_addInfo);
        }
    }
    for (int i = 0; i < 0x1a; i++)
    {
        if (m_invenItem[i].m_addInfo != 0)
        {
            LogManager::logFormat(
                1, "inventory.cpp",
                __PRETTY_FUNCTION__, 0x1af6,
                "Equip(%d) : %d\n", i, m_invenItem[i].m_addInfo);
        }
    }
}