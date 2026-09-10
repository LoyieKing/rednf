void cUserHistoryLog::PrivateStoreItemAdd(INVEN_TYPE invenType, Inven_Item const& item,
                                          int addNew, int addOld, eItemAddReason reason,
                                          char const* name1, char const* name2)
{
    m_user->LogHistory(
        "Item+,%d,%d,%d,%d,%d,\"%s\",\"%s\",%d,%d,"
        "%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X%02X,%d,%d,%s",
        (int)invenType, item.m_addInfo, addNew, addOld, (int)reason,
        name1, name2,
        item.GetUpgrade(),
        (unsigned)item.m_fieldb,
        ((const InvenByteView*)&item)->m_b25, ((const InvenByteView*)&item)->m_b26,
        ((const InvenByteView*)&item)->m_b27, ((const InvenByteView*)&item)->m_b28,
        ((const InvenByteView*)&item)->m_b29, ((const InvenByteView*)&item)->m_b2a,
        ((const InvenByteView*)&item)->m_b2b, ((const InvenByteView*)&item)->m_b2c,
        ((const InvenByteView*)&item)->m_b2d, ((const InvenByteView*)&item)->m_b2e,
        ((const InvenByteView*)&item)->m_b2f, ((const InvenByteView*)&item)->m_b30,
        ((const InvenByteView*)&item)->m_b31, ((const InvenByteView*)&item)->m_b32,
        item.m_field0,            // ORIG IVar1 = *param_3（item+0x00 字节）
        item.GetReSealCount(),
        CItemGloballyUniqueIdentifierPrint()(
            *reinterpret_cast<const itemGloballyUniqueIdentifier_t*>(item.m_amp.m_pad4)));
