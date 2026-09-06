// df_game_r Game/ AradServerStateMessage 实现（批3：从 GameStubs.cpp 迁移）。
#include "AradServerStateMessage.h"
#include <cstring>


AradServerStateMessage::AradServerStateMessage()
{
    // ORIG ctor @0x8186652：3×movl $0(+0/+4/+8) + movw $0(+0) + movl $0(+2)
    // —— 等价清空全对象 12B（含发送/拷贝覆盖的 m_pad06，ARAD.cpp:616
    // put_binary(&msg,0xc) 整 12B 发送）。旧注释误读为仅清 +0..+5，实为全零。
    // 4.4 对 memset 不内联（N78），语义等价的整对象清零优先；字段复清保 ORIG
    // movw/movl 尾形。
    std::memset(this, 0, sizeof(*this));
    m_active = 0;
    m_endSec = 0;
}