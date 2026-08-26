// df_game_r Game/ AradServerStateMessage 实现（批3：从 GameStubs.cpp 迁移）。
#include "AradServerStateMessage.h"
#include <cstring>


AradServerStateMessage::AradServerStateMessage()
{
    // ORIG ctor @0x8186652：等价 memset(this,0,0xc)（3×movl + movw/movl 冗余重写）。
    m_active = 0;
    m_endSec = 0;
    memset(m_pad06, 0, sizeof(m_pad06));
}