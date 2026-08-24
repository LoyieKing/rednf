#ifndef GAME_CCHARACLIST_H_
#define GAME_CCHARACLIST_H_

// ============================================================================
// df_game_r 还原 —— CCharacList（角色成长脚本宿主）。
// 布局证据：ctor 0x083493ea T（11 个 CCharacter 连续构造，步长 0x7dc）、
// dtor 0x0834945e T（逆序析构）、destroy 0x083494da、init 0x08349514
// （docs/class_func_reports/CCharacList/*.md）。sizeof = 11 * 0x7dc = 0x5694。
// ============================================================================

#include "CCharacter.h"

class STCharacterScript;   // 完整类型由 InitScripts_* 批次提供；init 以栈上
                           // 存储调用 ORIG C1/D1（asm 标签桥，见 CtorProducers_1.cpp）

class CCharacList
{
public:
    CCharacList();   // ORIG _ZN11CCharacListC1Ev 0x083493ea T
    ~CCharacList();  // ORIG _ZN11CCharacListD1Ev 0x0834945e T
    void destroy();  // ORIG _ZN11CCharacList7destroyEv 0x083494da T
    bool init();     // ORIG _ZN11CCharacList4initEv 0x08349514 T

    CCharacter m_characs[11];   // +0x000（ORIG 步长 0x7dc）
};  // sizeof 0x5694

#endif  // GAME_CCHARACLIST_H_
