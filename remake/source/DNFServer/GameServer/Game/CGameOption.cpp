// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 CGameOption/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN11CGameOptionC2Ev`（ORIG 0x84b731a）；ctor 已是首强符号。
#include <iostream>

#include "CGameOption.h"

#include "CGameOption/CGameOption.inc"
{
    resetFlag_QuickChatChnage();
    reset_load_hotkey_flag();
}

#include "CGameOption/SetHotKeyOption.inc"
{
    if (cSlot > 1)
        return;
    m_HotKeyOption[cSlot] = HotKeyOption;
    m_bLoadHotKeyFlag[cSlot] = 1;
}

#include "CGameOption/IsHotKeyOptionChanged.inc"
                                        CHotkeyOption& HotKeyOption)
{
    if (cSlot > 1)
        return 0;
    int nRet = memcmp(&m_HotKeyOption[cSlot], &HotKeyOption, 0xbc);
    if (nRet == 0)
        return 0;
    return 1;
}

#include "CGameOption/SetEtcOption.inc"
{
    m_EtcOption = EtcOption;
}

#include "CGameOption/SetQuickChatOption.inc"
                                     unsigned int nSlot)
{
    if (IsQuickChatOptionChanged(QuickChattingOption, nSlot))
    {
        setFlag_QuickChatChange();
        if (nSlot == 0 || nSlot == 1)
            m_QuickChattingOption[nSlot] = QuickChattingOption;
    }
}

#include "CGameOption/IsEtcOptionChanged.inc"
{
    int nRet = memcmp(&m_EtcOption, &EtcOption, 0x48);
    if (nRet == 0)
        return 0;
    return 1;
}

#include "CGameOption/IsQuickChatOptionChanged.inc"
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

#include "CGameOption/CopyQuickChatting.inc"
                                    unsigned int nSlot)
{
    if (nSlot == 0 || nSlot == 1)
        QuickChattingOption = m_QuickChattingOption[nSlot];
}

#include "CGameOption/SetEmoticonInfo.inc"
{
    if (memcmp(&m_ChattingEmoticonConfig, &ChattingEmoticonConfig, 0x210) != 0)
    {
        m_ChattingEmoticonConfig = ChattingEmoticonConfig;
        return true;
    }
    return false;
}

#include "CGameOption/SetFlag.inc"
{
    m_bEmoticonChanged = bFlag;
}

#include "CGameOption/isEmoticonChanged.inc"
{
    return m_bEmoticonChanged;
}

#include "CGameOption/isEqualEmoticon.inc"
{
    return memcmp(&m_ChattingEmoticonConfig, &ChattingEmoticonConfig, 0x210) == 0;
}

#include "CGameOption/set_etc_option.inc"
{
    m_EtcOption.setETCOption(eOption, sValue);
}

#include "CGameOption/get_etc_option.inc"
{
    return m_EtcOption.getETCOption(eOption);
}

#include "CGameOption/get_hotkey_option.inc"
{
    return &m_HotKeyOption[cSlot];
}

#include "CGameOption/reset_load_hotkey_flag.inc"
{
    for (int i = 0; i <= 1; ++i)
        m_bLoadHotKeyFlag[i] = 0;
}

#include "CGameOption/get_load_hotkey_flag.inc"
{
    if (cSlot > 1)
        return 0;
    return m_bLoadHotKeyFlag[cSlot];
}

#include "CGameOption/getEmoticonInfo.inc"
{
    return m_ChattingEmoticonConfig;
}
