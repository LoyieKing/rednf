// df_game_r Game/ MissionClearCondition_Parameter 实现（独立 TU，从 GameStubs.cpp 拆出）。
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
