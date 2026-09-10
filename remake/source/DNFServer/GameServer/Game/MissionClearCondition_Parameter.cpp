// df_game_r Game/ MissionClearCondition_Parameter 实现（独立 TU，从 GameStubs.cpp 拆出）。
// [GI-ioinit L21 · P338/P327] 补 <iostream> 恢复 __ioinit ⇒ 发射
// `_GLOBAL__I__ZN31MissionClearCondition_ParameterC2E15PVP_BATTLE_MODE`（ORIG 0x85e6878）。
#include <iostream>

#include "MissionClearCondition_Parameter.h"

// ORIG C1/C2 0x085e1a44：m_mode 默认 0xa；mode==2 → 1；mode==3 → 2。
MissionClearCondition_Parameter::MissionClearCondition_Parameter(PVP_BATTLE_MODE mode)
{
    m_mode = 0xa;
    if (mode == (PVP_BATTLE_MODE)2)
        m_mode = 1;
    else if (mode == (PVP_BATTLE_MODE)3)
        m_mode = 2;
}
