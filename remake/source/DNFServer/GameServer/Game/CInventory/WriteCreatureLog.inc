void CInventory::WriteCreatureLog(INVEN_TYPE invenType, int a, int b, int c, int d,
                                  eItemAddReason reason) const
{
    if (m_pParent != 0)
    {
        m_pParent->m_historyLog.CreatureItemAdd(
            invenType, a, b, c, d, reason);
    }
    else
    {
        LogManager::logFormat(
            1, "inventory.cpp",
            __PRETTY_FUNCTION__,
            0x24ce, "History ERROR, m_pParent NULL, ItemAdd ");
    }
