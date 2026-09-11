bool CGameOption::IsQuickChatOptionChanged(
    const CQuickChattingOption& QuickChattingOption, unsigned int nSlot)
{
    if (nSlot == 0 || nSlot == 1)
    {
        int nRet = memcmp(&m_QuickChattingOption[nSlot], &QuickChattingOption, 0x12c);
        if (nRet == 0)
            return 0;
        return 1;
    }
    return 0;
}