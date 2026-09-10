void CUserCharacInfo::setDemensionInoutValue(char index, char value)
{
    if (index > 5 || index < 0)
        return;
    CUR->m_saveDemensionInout = 1;
    CUR->m_demensionInout[index] = (unsigned char)value;
