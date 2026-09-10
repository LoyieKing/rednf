void _writeIncorrectItemInventoryForRecovery(unsigned int characNo,
                                             const Inven_Item& item,
                                             const char* tag, int err,
                                             int slot)
{
    LogManager::logFormat(
        1, "InterDispatcher.cpp",
        __PRETTY_FUNCTION__,
        0x1645,
        "ERROR_ITEM_LOG %u,%d,%d,%u,%d,%d,%d,%u,%d - %s slot(%d), error_line(%d)\n",
        // ORIG 实参槽序（0x28=+0x7, 0x2c=+0xb, 0x30=+0xd, 0x34=+0x11）：
        // 第 6..9 参 = m_addInfo2, m_fieldb, m_fieldd, amp 首 int（P0 内联）
        characNo, (int)item.m_field0, (int)item.m_field1, item.m_addInfo,
        (int)item.GetItemAttr(), item.m_addInfo2, (int)item.m_fieldb,
        item.m_fieldd, *(const int*)(const void*)&item.m_amp, tag, slot, err);
