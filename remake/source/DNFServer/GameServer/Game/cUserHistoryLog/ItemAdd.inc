void cUserHistoryLog::ItemAdd(INVEN_TYPE invenType, int a3, int a4,
                              const Inven_Item& item, eItemAddReason reason)
{
    if ((int)reason == 1 || (int)reason == 2)
    {
        if (*(0, m_traderCharac) != 0)
        {
            m_user->LogHistory(
                "Item+,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
                "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
                (int)invenType, item.m_addInfo, a3, a4, (int)reason,
                m_traderCharac, m_traderAccount,
                item.GetUpgrade(), (unsigned)item.m_fieldb,
                ((const InvenByteView*)&item)->m_b25, ((const InvenByteView*)&item)->m_b26, ((const InvenByteView*)&item)->m_b27, ((const InvenByteView*)&item)->m_b28, ((const InvenByteView*)&item)->m_b29, ((const InvenByteView*)&item)->m_b2a,
                ((const InvenByteView*)&item)->m_b2b, ((const InvenByteView*)&item)->m_b2c, ((const InvenByteView*)&item)->m_b2d, ((const InvenByteView*)&item)->m_b2e, ((const InvenByteView*)&item)->m_b2f, ((const InvenByteView*)&item)->m_b30,
                ((const InvenByteView*)&item)->m_b31, ((const InvenByteView*)&item)->m_b32,
                item.m_field0,
                item.GetReSealCount(),
                CItemGloballyUniqueIdentifierPrint()(
                    *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
        }
    }
    else
    {
        // ORIG：getItemXPos/YPos(m_user + 0x79700)（history 子对象 +0x40/+0x42 网格坐标）。
        // 全部实参就地内联 ⇒ GCC -O0 右→左求值，与 ORIG 装载序一致
        // （末实参 GUID 最先，XPos 最后）。
        m_user->LogHistory(
            "Item+,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,"
            "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
            (int)invenType, item.m_addInfo, a3, a4, (int)reason,
            m_user->m_historyLog.getItemXPos() & 0xffff,
            m_user->m_historyLog.getItemYPos() & 0xffff,
            item.m_addInfo2, item.m_fieldd,
            (unsigned)item.m_amp.m_abilityType, (unsigned)item.m_amp.m_abilityValue,
            item.GetUpgrade(), (unsigned)item.m_fieldb,
            ((const InvenByteView*)&item)->m_b25, ((const InvenByteView*)&item)->m_b26, ((const InvenByteView*)&item)->m_b27, ((const InvenByteView*)&item)->m_b28, ((const InvenByteView*)&item)->m_b29, ((const InvenByteView*)&item)->m_b2a,
            ((const InvenByteView*)&item)->m_b2b, ((const InvenByteView*)&item)->m_b2c, ((const InvenByteView*)&item)->m_b2d, ((const InvenByteView*)&item)->m_b2e, ((const InvenByteView*)&item)->m_b2f, ((const InvenByteView*)&item)->m_b30,
            ((const InvenByteView*)&item)->m_b31, ((const InvenByteView*)&item)->m_b32,
            item.m_field0,
            item.GetReSealCount(),
            CItemGloballyUniqueIdentifierPrint()(
                *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
    }
