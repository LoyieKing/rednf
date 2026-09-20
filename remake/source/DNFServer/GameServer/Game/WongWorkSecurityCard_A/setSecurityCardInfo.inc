// T 0x085fde9c setSecurityCardInfo(char const*, bool, bool, int, int, int, char*)：
//   bool 形参截断暂存后直赋 bool 字段（ORIG movzbl 回读直写，无规范化）；
//   strncpy(this+8, serial, 0x27);
//   this[0x2f]=activate; this[0x30]=certified; *(int*)(this+0x34)=certCnt;
//   this[0x31]=(unsigned char)certFlag; *(int*)(this+0x38)=cancelCnt;
//   if (answers != NULL) { strncpy(this+0x3c, answers, 5); 每字节 -'0'（展开 4 次）；}
void CSecurityCard::setSecurityCardInfo(char const* serial, bool activate,
                                        bool certified, int certCnt,
                                        int certFlag, int cancelCnt,
                                        char* answers)
{
    strncpy(m_serial, serial, 0x27);
    m_activate2f  = activate;
    m_certified30 = certified;
    m_failCnt34   = certCnt;
    m_certFlag31  = static_cast<unsigned char>(certFlag);
    m_certCnt38   = cancelCnt;
    if (answers != reinterpret_cast<char*>(0))
    {
        strncpy(m_question3c, answers, 5);
        m_question3c[0] = static_cast<char>(m_question3c[0] - '0');
        m_question3c[1] = static_cast<char>(m_question3c[1] - '0');
        m_question3c[2] = static_cast<char>(m_question3c[2] - '0');
        m_question3c[3] = static_cast<char>(m_question3c[3] - '0');
    }
}