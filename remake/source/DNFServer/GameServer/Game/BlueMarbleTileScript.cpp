// df_game_r Game/ BlueMarbleTileScript 专属 TU（从 GameStubs.cpp 拆出）。
// 逐方法对照 ORIG df_game_r 反汇编实现。
#include "BlueMarbleTileScript.h"

// ORIG 0x88d5068：ctor 仅调 clear()。
BlueMarbleTileScript::BlueMarbleTileScript()
{
    clear();
}

// ORIG 0x88d507c：m_type=0、m_char4=0、m_value=0。
void BlueMarbleTileScript::clear()
{
    m_type = 0;
    m_char4 = 0;
    m_value = 0;
}