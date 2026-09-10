void CPad::setPassword(char* pwd)
{
    // ORIG: strncpy(&m_pad[0], pwd, 0x24)
    strncpy(m_pad, pwd, 0x24);
