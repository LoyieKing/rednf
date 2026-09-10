unsigned char CUserCharacInfo::getDemensionInoutValue(char index)
{
    if (index > 5 || index < 0)
        return 0;
    return CUR->m_demensionInout[index];
