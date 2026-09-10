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

// setBreakAwayRewardOrder(ORIG W 0x0831d4de) 透传写 m_breakAwayRewardOrder（+0x654）；
// ORIG 产 `mov 0x8(%ebp),%eax; mov 0xc(%ebp),%edx; mov %edx,0x654(%eax)` this 先载
// ⇒ 成员须与形参同型（gcc4.4 装载序定律）。定义与 CUser.h:88 同 guard 块（CUser.h
// 经 #include "CGameOption.h"@68 晚于此，互不重复）。
#ifndef DNF_ENUM_BREAK_AWAY_REWARD_ORDER_DEFINED
#define DNF_ENUM_BREAK_AWAY_REWARD_ORDER_DEFINED
enum ENUM_BREAK_AWAY_REWARD_ORDER { ENUM_BREAK_AWAY_REWARD_ORDER_0 = 0 };
#endif

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

    char m_data[0x14];  // +0x00
    short m_nCount;     // +0x14，总 0x16 字节
};

class CChattingEmoticonConfig
{
public:
    CChattingEmoticonConfig() {}

    void clear()
    {
        // ORIG 0x80e5f12：头部 jmp→cond `mov -0xc,%eax; cmp $0x17,%eax;
        // setbe %al; test; jne` —— 4.1 无 cmpl mem 融合；4.4 对 unsigned
        // 槽比较恒折 `cmpl $0x17,-0xc(%ebp)`。
        // [Main·检查点30 P0/N96 清绑] 原形为 `register unsigned int v asm("eax")`
        //   + for-cond 逗号表达式（技巧28）——**违反 P0 限定②/N96 全域禁绑**，
        //   按 P209附 的实测结论（for 上界/<=/do-while/while 四形均不可复现
        //   ORIG 的 `mov -0xc,%eax; cmp $0x17,%eax`）⇒ 清绑后取最短自然形，
        //   残差归 **N96+N18a 封顶**（4.4 对 unsigned 槽比较恒 cmpl mem 融合）。
        for (unsigned int i = 0; i <= 0x17u; ++i)
            m_Row[i].clear();
    }

    // 行数据公开（CChattingEmoticonList::ReCalc 0x80e5880 逐行读写；布局/尺寸不变）
    CChattingEmoticonConfigRow m_Row[24];  // +0x00，24 * 0x16 = 0x210 字节
};

#pragma pack(pop)

#pragma pack(push, 1)
// ORIG 模板（and $0x1 头部对齐检查）证明 CGameOption align 1 —— 类声明在 pack(1) 内
class CGameOption
{
public:
    CGameOption();

    void SetHotKeyOption(unsigned char cSlot, CHotkeyOption& HotKeyOption);
    // [Main·检查点22 P292/T3] 返回 uchar→**bool**。直证：ORIG 唯一消费点
    //   Dispatcher_SaveCharacterOption::process 0x081ceedx 序列
    //   `call IsHotKeyOptionChanged; xor $0x1,%eax; test %al,%al; je`（3 条、无
    //   setne/sete、结果不落槽）⇒ callee 返回 bool；返回 uchar 时 4.4 恒多一条
    //   `sete %al`（ndiff=3 全在此）。定义体本就只有 `return 0/1`，语义不变。
    bool IsHotKeyOptionChanged(unsigned char cSlot, CHotkeyOption& HotKeyOption);
    void SetEtcOption(CETCOption& EtcOption);
    void SetQuickChatOption(const CQuickChattingOption& QuickChattingOption,
                            unsigned int nSlot);
    // [Q4DispatchBig 2026-09-11 · bool 轴] 返回类型 unsigned char -> bool（ORIG 调用点
    //   0x081cec17 `call; xor $0x1,%eax; test %al,%al; je` 直证；两侧定义体逐字节相同
    //   ORIG 0x084b6f46 len 0x3d ≡ 我方 .o @0x2ee ⇒ 纯调用点收益）。
    bool IsEtcOptionChanged(CETCOption& EtcOption);
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
    bool get_load_hotkey_flag(unsigned char cSlot);  // ORIG 返回 bool（P32 直返判据：函数体
                                                              // movzbl 直返无 setne ⇔ bool 字段直返；调用点
                                                              // CUser::LoadCharacTypeHotKey 0x08692d2a 处
                                                              // test %al,%al; je 无 setne ⇔ bool 返回）
    CChattingEmoticonConfig getEmoticonInfo() const;

    unsigned char isChanged_QuickChatData() { return m_bQuickChatChange; }
    void resetFlag_QuickChatChnage() { m_bQuickChatChange = false; }
    void setFlag_QuickChatChange() { m_bQuickChatChange = true; }

private:
    CHotkeyOption m_HotKeyOption[2];                 // +0x000
    bool m_bLoadHotKeyFlag[2];                    // +0x178（ORIG get_load_hotkey_flag movzbl 直返
                                                     //   无 setne ⇔ bool 字段直返；reset/set 均为 movb 写）
    CETCOption m_EtcOption;                          // +0x17a
    CQuickChattingOption m_QuickChattingOption[2];   // +0x1c2
    bool m_bQuickChatChange;                         // +0x41a
    CChattingEmoticonConfig m_ChattingEmoticonConfig;// +0x41b
    bool m_bEmoticonChanged;                         // +0x62b

public:
    // ---- ORIG 直访尾部字段（CUser 方法经 this+0x62c.. 访问；ORIG 为 public）----
    bool m_gmQuestFlag;                              // +0x62c（ORIG setGmQuestFlag 0x0822fc8e this-先 bool=bool 直落形）
    bool m_forceDropFlag;                            // +0x62d（ORIG setForceDropFlag 0x0868f6fa 同形；getForceDropFlag 0x0868f716 movzbl 直返兼容）
    char m_webAddress[0x12];                         // +0x62e（getWebAddress）
    unsigned int m_playExpAdd;                       // +0x640（P211 异型化实证：incPlayExpAdd
                                                     //   ORIG 0x86972a0 `mov field,%edx; mov v,%eax;
                                                     //   add %eax,%edx` 双段形=unsigned 字段+int 参
                                                     //   转换中转（probe p10.incG）；宽度/偏移不变）
    bool m_breakAwayDungeonClear;                    // +0x644（CUser+0x703a8；ORIG movzbl 直返，bool 实证
                                                     //   ORIG W 0x085bfd1a 写 byte / isBreakAwayDungeonClear
                                                     //   0x086971de movzbl 读；CUserOnlyOrig.cpp:7704 同义实现）
    char m_pad645[3];                                // +0x645..0x647（ORIG 无具名访问）
    int m_breakAwayDungeonClearCnt;                  // +0x648
    int m_breakAwayAccureCera;                       // +0x64c（get/setBreakAwayAccureCera 实证，原 m_pad64c[4]；§8.3 字段化，证据 CUser.cpp:4500/4684）
    unsigned int m_breakAwayQuestTime;               // +0x650（ORIG setBreakAwayQuestTime 0x0822fc7c this-先 uint 直写；getBreakAwayQuestTime 32 位 mov 直读）
    // +0x654 区按 ORIG setter 实证具名（CGameOption 止于 +0x657，sizeof 0x658）：
    ENUM_BREAK_AWAY_REWARD_ORDER m_breakAwayRewardOrder;  // +0x654（setBreakAwayRewardOrder W 0x0831d4de 直写；同型装载序实证）
    // 注意：+0x658..+0x65f 8 字节为 CUser 字段（m_multiboxLotteryFailCnt @ CUser+0x703bc、
    // m_breakAwayRewardData @ CUser+0x703c0、m_usedBreakAwayLuckPoint @ CUser+0x703c1），
    // 原误入 CGameOption 尾部（Main 批准 2026-08-29 收敛）。
};

#pragma pack(pop)
#endif  // GAME_CGAMEOPTION_H_
