#ifndef GAME_CGAMEOPTION_H_
#define GAME_CGAMEOPTION_H_

// ============================================================================
// df_game_r 还原 —— CGameOption（G1 批次，ORIG 布局）
// 布局依据 CGameOption.md 反汇编逐成员推导：
//   +0x000 CHotkeyOption m_HotKeyOption[2]（0xbc * 2）
//   +0x178 unsigned char m_bLoadHotKeyFlag[2]
//   +0x17a CETCOption m_EtcOption（0x48）
//   +0x1c2 CQuickChattingOption m_QuickChattingOption[2]（0x12c * 2）
//   +0x41a bool m_bQuickChatChange
//   +0x41b CChattingEmoticonConfig m_ChattingEmoticonConfig（0x210）
//   +0x62b bool m_bEmoticonChanged
// 依赖子类成员函数在 ORIG 中为 weak（头文件 inline），本头一并给出。
// ============================================================================

#include <cstring>

enum ENUM_OPTION_ETC
{
    ENUM_OPTION_ETC_0 = 0,
    ENUM_OPTION_ETC_1 = 1,
    ENUM_OPTION_ETC_2 = 2,
    ENUM_OPTION_ETC_3 = 3,
    ENUM_OPTION_ETC_4 = 4,
    ENUM_OPTION_ETC_5 = 5,
    ENUM_OPTION_ETC_6 = 6,
    ENUM_OPTION_ETC_7 = 7,
    ENUM_OPTION_ETC_8 = 8,
    ENUM_OPTION_ETC_9 = 9,
    ENUM_OPTION_ETC_10 = 10,
    ENUM_OPTION_ETC_11 = 11,
    ENUM_OPTION_ETC_12 = 12,
    ENUM_OPTION_ETC_13 = 13,
    ENUM_OPTION_ETC_14 = 14,
    ENUM_OPTION_ETC_15 = 15,
    ENUM_OPTION_ETC_16 = 16,
    ENUM_OPTION_ETC_17 = 17,
    ENUM_OPTION_ETC_18 = 18,
    ENUM_OPTION_ETC_19 = 19,
    ENUM_OPTION_ETC_20 = 20,
    ENUM_OPTION_ETC_21 = 21,
    ENUM_OPTION_ETC_22 = 22,
    ENUM_OPTION_ETC_23 = 23,
    ENUM_OPTION_ETC_24 = 24,
    ENUM_OPTION_ETC_25 = 25,
    ENUM_OPTION_ETC_26 = 26,
    ENUM_OPTION_ETC_27 = 27,
    ENUM_OPTION_ETC_28 = 28,
    ENUM_OPTION_ETC_29 = 29,
    ENUM_OPTION_ETC_30 = 30,
    ENUM_OPTION_ETC_31 = 31,
    ENUM_OPTION_ETC_32 = 32,
    ENUM_OPTION_ETC_33 = 33,
    ENUM_OPTION_ETC_34 = 34,
    ENUM_OPTION_ETC_35 = 35
};

#pragma pack(push, 1)

class CHotkeyOption
{
public:
    CHotkeyOption() { clear(); }
    void clear()
    {
        for (int i = 0; i <= 0x5d; ++i)
            m_HotKey[i] = 0xffff;
    }

private:
    short m_HotKey[0x5e];  // +0x00，0xbc 字节
};

class CETCOption
{
public:
    CETCOption() { clear(); }

    void clear()
    {
        int nDefault = 0;
        nDefault = 0x7fff;
        for (int i = 0; i <= 0x23; ++i)
            m_Option[i] = (short)nDefault;
    }

    void setETCOption(ENUM_OPTION_ETC eOption, short sValue)
    {
        if (eOption > 0x23 || eOption < 0)
            return;
        m_Option[eOption] = sValue;
    }

    short getETCOption(ENUM_OPTION_ETC eOption) const
    {
        if (eOption > 0x23 || eOption < 0)
            return -1;
        return m_Option[eOption];
    }

private:
    short m_Option[0x24];  // +0x00，0x48 字节
};

#pragma pack(pop)

class CQuickChattingOption
{
public:
    CQuickChattingOption() { clear(); }
    void clear() { memset((char*)this, 0, 0x12c); }

private:
    char m_data[0x12c];  // +0x00，0x12c 字节
};

#pragma pack(push, 1)

class CChattingEmoticonConfigRow
{
public:
    CChattingEmoticonConfigRow() { clear(); }

    void clear()
    {
        memset((char*)this, 0, 0x14);
        m_nCount = 0;
    }

private:
    char m_data[0x14];  // +0x00
    short m_nCount;     // +0x14，总 0x16 字节
};

class CChattingEmoticonConfig
{
public:
    CChattingEmoticonConfig() {}

    void clear()
    {
        for (int i = 0; i <= 0x17; ++i)
            m_Row[i].clear();
    }

private:
    CChattingEmoticonConfigRow m_Row[24];  // +0x00，24 * 0x16 = 0x210 字节
};

#pragma pack(pop)

class CGameOption
{
public:
    CGameOption();

    void SetHotKeyOption(unsigned char cSlot, CHotkeyOption& HotKeyOption);
    unsigned char IsHotKeyOptionChanged(unsigned char cSlot, CHotkeyOption& HotKeyOption);
    void SetEtcOption(CETCOption& EtcOption);
    void SetQuickChatOption(const CQuickChattingOption& QuickChattingOption,
                            unsigned int nSlot);
    unsigned char IsEtcOptionChanged(CETCOption& EtcOption);
    bool IsQuickChatOptionChanged(const CQuickChattingOption& QuickChattingOption,
                                  unsigned int nSlot);
    void CopyQuickChatting(CQuickChattingOption& QuickChattingOption,
                           unsigned int nSlot);
    bool SetEmoticonInfo(const CChattingEmoticonConfig& ChattingEmoticonConfig);
    void SetFlag(bool bFlag);
    unsigned char isEmoticonChanged();
    bool isEqualEmoticon(const CChattingEmoticonConfig& ChattingEmoticonConfig);
    void set_etc_option(ENUM_OPTION_ETC eOption, short sValue);
    short get_etc_option(ENUM_OPTION_ETC eOption);
    CHotkeyOption* get_hotkey_option(unsigned char cSlot);
    void reset_load_hotkey_flag();
    unsigned char get_load_hotkey_flag(unsigned char cSlot);
    CChattingEmoticonConfig getEmoticonInfo() const;

    unsigned char isChanged_QuickChatData() { return m_bQuickChatChange; }
    void resetFlag_QuickChatChnage() { m_bQuickChatChange = false; }
    void setFlag_QuickChatChange() { m_bQuickChatChange = true; }

private:
    CHotkeyOption m_HotKeyOption[2];                 // +0x000
    unsigned char m_bLoadHotKeyFlag[2];              // +0x178
    CETCOption m_EtcOption;                          // +0x17a
    CQuickChattingOption m_QuickChattingOption[2];   // +0x1c2
    bool m_bQuickChatChange;                         // +0x41a
    CChattingEmoticonConfig m_ChattingEmoticonConfig;// +0x41b
    bool m_bEmoticonChanged;                         // +0x62b

public:
    // ---- ORIG 直访尾部字段（CUser 方法经 this+0x62c.. 访问；ORIG 为 public）----
    char m_gmQuestFlag;                              // +0x62c
    char m_forceDropFlag;                            // +0x62d
    char m_webAddress[0x12];                         // +0x62e（getWebAddress）
    int m_playExpAdd;                                // +0x640
    char m_pad644[4];                                // +0x644..0x647
    int m_breakAwayDungeonClearCnt;                  // +0x648
    char m_pad64c[4];                                // +0x64c..0x64f
    int m_breakAwayQuestTime;                        // +0x650
    char m_pad654b[0x660 - 0x654];                   // +0x654..0x660（ORIG 尺寸 0x660）
};

#endif  // GAME_CGAMEOPTION_H_
