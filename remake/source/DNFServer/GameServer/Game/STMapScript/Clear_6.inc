void STAssignAnimation::Clear()             // ORIG 0x089e832c
{
    m_name = "";                            // ORIG Ss::operator=("")
    m_layer = 1;                            // +0x04 movl $1
    m_field08 = 0;                          // +0x08
    m_field0c = 0;                          // +0x0c
    m_field10 = 0;                          // +0x10
}