void BackGroundEffect::clear()              // ORIG 0x089e88da
{
    m_type = 2;                             // +0x00 movl $2
    m_layer = 7;                            // +0x04 movl $7
    m_field08 = -1;                         // +0x08
    m_field0c = -1;                         // +0x0c
    m_field10 = -1;                         // +0x10
    m_field14 = -1;                         // +0x14
    m_field18 = -1;                         // +0x18
    m_animName.clear();                     // +0x1c
