#include "CGameOption.h"

CGameOption::CGameOption()
{
    resetFlag_QuickChatChnage();
    reset_load_hotkey_flag();
}

void CGameOption::SetHotKeyOption(unsigned char cSlot, CHotkeyOption& HotKeyOption)
{
    if (cSlot > 1)
        return;
    m_HotKeyOption[cSlot] = HotKeyOption;
    m_bLoadHotKeyFlag[cSlot] = 1;
}

unsigned char CGameOption::IsHotKeyOptionChanged(unsigned char cSlot,
                                                 CHotkeyOption& HotKeyOption)
{
    if (cSlot > 1)
        return 0;
    int nRet = memcmp(&m_HotKeyOption[cSlot], &HotKeyOption, 0xbc);
    if (nRet == 0)
        return 0;
    return 1;
}

void CGameOption::SetEtcOption(CETCOption& EtcOption)
{
    m_EtcOption = EtcOption;
}

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

unsigned char CGameOption::IsEtcOptionChanged(CETCOption& EtcOption)
{
    int nRet = memcmp(&m_EtcOption, &EtcOption, 0x48);
    if (nRet == 0)
        return 0;
    return 1;
}

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

void CGameOption::CopyQuickChatting(CQuickChattingOption& QuickChattingOption,
                                    unsigned int nSlot)
{
    if (nSlot == 0 || nSlot == 1)
        QuickChattingOption = m_QuickChattingOption[nSlot];
}

bool CGameOption::SetEmoticonInfo(const CChattingEmoticonConfig& ChattingEmoticonConfig)
{
    if (memcmp(&m_ChattingEmoticonConfig, &ChattingEmoticonConfig, 0x210) != 0)
    {
        m_ChattingEmoticonConfig = ChattingEmoticonConfig;
        return true;
    }
    return false;
}

void CGameOption::SetFlag(bool bFlag)
{
    m_bEmoticonChanged = bFlag;
}

unsigned char CGameOption::isEmoticonChanged()
{
    return m_bEmoticonChanged;
}

bool CGameOption::isEqualEmoticon(const CChattingEmoticonConfig& ChattingEmoticonConfig)
{
    return memcmp(&m_ChattingEmoticonConfig, &ChattingEmoticonConfig, 0x210) == 0;
}

void CGameOption::set_etc_option(ENUM_OPTION_ETC eOption, short sValue)
{
    m_EtcOption.setETCOption(eOption, sValue);
}

short CGameOption::get_etc_option(ENUM_OPTION_ETC eOption)
{
    return m_EtcOption.getETCOption(eOption);
}

CHotkeyOption* CGameOption::get_hotkey_option(unsigned char cSlot)
{
    return &m_HotKeyOption[cSlot];
}

void CGameOption::reset_load_hotkey_flag()
{
    for (int i = 0; i <= 1; ++i)
        m_bLoadHotKeyFlag[i] = 0;
}

unsigned char CGameOption::get_load_hotkey_flag(unsigned char cSlot)
{
    if (cSlot > 1)
        return 0;
    return m_bLoadHotKeyFlag[cSlot];
}

CChattingEmoticonConfig CGameOption::getEmoticonInfo() const
{
    return m_ChattingEmoticonConfig;
}
