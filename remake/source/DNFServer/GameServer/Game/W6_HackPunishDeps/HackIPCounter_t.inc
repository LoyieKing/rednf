// ----------------------------------------------------------------------------
// HackIPCounter_t —— ORIG W C1/C2(EjjPcj) 0x080f9892 (0x63)：
// +0/+2 截 u16、+0x14=cnt，ip 区 memset(0xd)+strncpy(0xd)。C2 与 C1 同地址。
// ----------------------------------------------------------------------------
HackIPCounter_t::HackIPCounter_t(unsigned int a, unsigned int b, char* ip,
                                 unsigned int cnt)
{
    m_a = (unsigned short)a;   // mov %dx,(%eax)
    m_b = (unsigned short)b;   // mov %dx,0x2(%eax)
    m_cnt = cnt;               // mov %edx,0x14(%eax)
    memset(m_ip, 0, 0xd);      // memset(this+4, 0, 0xd)
    strncpy(m_ip, ip, 0xd);    // strncpy(this+4, param3, 0xd)
}