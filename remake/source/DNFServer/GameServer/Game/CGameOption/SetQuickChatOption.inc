void CGameOption::SetQuickChatOption(const CQuickChattingOption& QuickChattingOption,
                                     unsigned int nSlot)
{
    if (IsQuickChatOptionChanged(QuickChattingOption, nSlot))
    {
        setFlag_QuickChatChange();
        if (nSlot == 0 || nSlot == 1)
            m_QuickChattingOption[nSlot] = QuickChattingOption;
    }
}