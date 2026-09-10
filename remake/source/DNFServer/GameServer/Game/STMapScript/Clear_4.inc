void STAssignSpecialPassiveObj::Clear()     // ORIG 0x089e875a
{
    m_0 = -1;                               // +0x00 movl $-1
    m_c = 0;                                // +0x0c movl $0
    m_8 = m_c;                              // +0x08 ← +0x0c
    m_4 = m_8;                              // +0x04 ← +0x08
    m_vec10.clear();                        // vector@0x10
    m_1c = -1;                              // +0x1c movl $-1
