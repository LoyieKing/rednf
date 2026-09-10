void CSecurityCard::setSecurityCardInfo(char const* serial, bool activate,
                                        bool certified, int certCnt,
                                        int certFlag, int cancelCnt,
                                        char* answers)
{
    strncpy(reinterpret_cast<char*>(this) + 8, serial, 0x27);
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
