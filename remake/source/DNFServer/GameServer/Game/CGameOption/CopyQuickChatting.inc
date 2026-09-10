void CGameOption::CopyQuickChatting(CQuickChattingOption& QuickChattingOption,
                                    unsigned int nSlot)
{
    if (nSlot == 0 || nSlot == 1)
        QuickChattingOption = m_QuickChattingOption[nSlot];
