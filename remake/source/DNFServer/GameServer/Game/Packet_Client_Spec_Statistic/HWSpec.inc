HWSpec::HWSpec()
{
    m_category1 = static_cast<char>(0xff);   // ORIG movb $0xff,(%eax)
    m_category2 = 0xffffffffu;               // ORIG movl $0xffffffff,+0x04
    m_category3 = 0xffffffffu;               // ORIG movl $0xffffffff,+0x08
