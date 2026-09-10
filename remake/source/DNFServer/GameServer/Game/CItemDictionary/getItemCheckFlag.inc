bool CItemDictionary::getItemCheckFlag(unsigned int item, unsigned int flag) const
{
    int index = (int)(flag >> 3);
    int bit = (int)(flag & 7);
    if (index < 0 || index > 0x1387 || bit < 0 || bit > 7)
    {
        LogManager::logFormat(
            1, "ItemDictionary.cpp",
            __PRETTY_FUNCTION__,
            0x7d,
            "getItemCheckFlag() characNo=%u, item_flag=%u, index=%d, flag=%d",
            item, flag, index, bit);
        return 1;
    }
    unsigned char v = m_data[index];   // ORIG 局部（栈 -0x9）
    if (!(v >> bit & 1))   // ORIG xor $0x1 取反形
    {
        return 0;
    }
    return 1;
