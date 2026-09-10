void CUserCharacInfo::setUltimateInoutValue(char index, char value)
{
    CUR->m_saveBloodInout = 1;
    CUR->m_ultimateInout[index] = (unsigned char)value;
