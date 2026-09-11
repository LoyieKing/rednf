// T 0x085fdd68 init：清 +0x2f/0x30/0x34/0x44/0x48/0x38/0x31/0x40。
// 字段顺序与 ORIG 清零序列一致；+0x40 为单字节占位区（init 只清该字节），
// 头中以 m_pad40 区表达。
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
}