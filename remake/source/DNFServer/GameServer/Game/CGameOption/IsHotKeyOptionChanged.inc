bool CGameOption::IsHotKeyOptionChanged(unsigned char cSlot,
                                        CHotkeyOption& HotKeyOption)
{
    if (cSlot > 1)
        return 0;
    int nRet = memcmp(&m_HotKeyOption[cSlot], &HotKeyOption, 0xbc);
    if (nRet == 0)
        return 0;
    return 1;
