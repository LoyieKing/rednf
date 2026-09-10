HackFullIPCounter_t::HackFullIPCounter_t(unsigned int a, unsigned int b, char* ip,
                                         unsigned int cnt)
{
    m_a = (unsigned short)a;
    m_b = (unsigned short)b;
    m_cnt = cnt;
    memset(m_ip, 0, 0x10);
    strncpy(m_ip, ip, 0x10);
