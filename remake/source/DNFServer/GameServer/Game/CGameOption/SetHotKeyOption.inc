void CGameOption::SetHotKeyOption(unsigned char cSlot, CHotkeyOption& HotKeyOption)
{
    if (cSlot > 1)
        return;
    m_HotKeyOption[cSlot] = HotKeyOption;
    m_bLoadHotKeyFlag[cSlot] = 1;
