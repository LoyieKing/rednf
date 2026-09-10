void CUserCharacInfo::subDemensionInoutValue(char index)
{
    if (index > 5 || index < 0)
        return;
    CUR->m_saveDemensionInout = 1;
    --CUR->m_demensionInout[index];
