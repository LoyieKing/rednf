bool CGameOption::get_load_hotkey_flag(unsigned char cSlot)
{
    if (cSlot > 1)
        return 0;
    return m_bLoadHotKeyFlag[cSlot];
