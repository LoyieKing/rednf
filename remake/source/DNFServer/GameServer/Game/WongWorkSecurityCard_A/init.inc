void CSecurityCard::init()
{
    m_activate2f    = 0;                                          // movb $0x0,0x2f
    m_certified30   = 0;                                          // movb $0x0,0x30
    m_failCnt34     = 0;                                          // movl $0x0,0x34
    m_requestState44= 0;                                          // movl $0x0,0x44
    m_cancelCnt48   = 0;                                          // movl $0x0,0x48
    m_certCnt38     = 0;                                          // movl $0x0,0x38
    m_certFlag31    = 0;                                          // movb $0x0,0x31
    m_pad40         = 0;                                          // movb $0x0,0x40
