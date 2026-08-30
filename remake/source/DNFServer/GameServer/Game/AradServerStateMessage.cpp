// df_game_r Game/ AradServerStateMessage 实现（批3：从 GameStubs.cpp 迁移）。
#include "AradServerStateMessage.h"


AradServerStateMessage::AradServerStateMessage()
{
    // ORIG ctor @0x8186652：movw $0,(+0) + movl $0,(+2)，仅清 m_active/m_endSec
    // （+0..+5）；+6..+0xb 的 m_pad06 不清零（发送/拷贝覆盖域）。
    m_active = 0;
    m_endSec = 0;
}