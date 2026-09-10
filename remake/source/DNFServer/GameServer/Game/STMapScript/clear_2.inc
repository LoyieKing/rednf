void AssignNpc::clear()                     // ORIG 0x089e87ee
{
    m_npcIdx = 0;                           // +0x00 movl $0
    m_field04 = -1;                         // +0x04 movl $-1
    m_visible = false;                      // +0x08 movb $0
    m_dir = 0;                              // +0x0c
    m_x = 0;                                // +0x10
    m_y = 0;                                // +0x14
    m_z = 0;                                // +0x18
