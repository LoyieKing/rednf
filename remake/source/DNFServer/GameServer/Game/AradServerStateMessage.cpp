// df_game_r Game/ AradServerStateMessage 实现（批3：从 GameStubs.cpp 迁移）。
#include "AradServerStateMessage.h"
#include <cstring>


AradServerStateMessage::AradServerStateMessage()
{
    // ORIG ctor @0x8186652：3×movl $0(+0/+4/+8) + movw $0(+0) + movl $0(+2)
    // —— 等价清空全对象 12B（含发送/拷贝覆盖的 m_pad06，ARAD.cpp:616
    // put_binary(&msg,0xc) 整 12B 发送）。旧注释误读为仅清 +0..+5，实为全零。
    // [b28c-30] 4.4 对 memset 恒发 call（N78）⇒ 以 m_words[3] 双视角写三条 dword
    // store 复现 ORIG 前三段；随后真实字段复清保 movw/movl 尾形。
    m_words[0] = 0;
    m_words[1] = 0;
    m_words[2] = 0;
    m_active = 0;
    m_endSec = 0;
}