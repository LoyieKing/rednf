// ORIG 0x0811d9d7：user==0 直接返回；index=flag>>3、bit=flag&7 越界（
// index∉[0,0x1387] 或 bit∉[0,7]）→ logFormat 后返回；getItemCheckFlag(-1,flag)
// 已置位则跳过；否则 m_data[index] += (1<<bit)（加法非 or），
// sendItemDictionaryInfo + CData::alter()。
void CItemDictionary::setItemCheckFlag(CUser* user, unsigned int item,
                                       unsigned int flag)
{
    if (user == 0)
        return;
    int index = (int)(flag >> 3);
    int bit = (int)(flag & 7);
    if (index < 0 || index > 0x1387 || bit < 0 || bit > 7)
    {
        LogManager::logFormat(
            1, "ItemDictionary.cpp",
            __PRETTY_FUNCTION__,
            0x62,
            "setItemCheckFlag() characNo=%u, item_flag=%u, index=%d, flag=%d",
            user->get_charac_no(-1), flag, index, bit);
        return;
    }
    unsigned char v = m_data[index];   // ORIG 死读（保留以匹配指令序列）
    if (!getItemCheckFlag(user->get_charac_no(-1), flag))
    {
        m_data[index] =
            static_cast<unsigned char>(m_data[index] + (1 << bit));
        sendItemDictionaryInfo(user, item);
        alter();
        return;
    }
    else
    {
    }
    return;
}
