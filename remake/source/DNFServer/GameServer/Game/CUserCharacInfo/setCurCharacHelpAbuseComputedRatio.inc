void CUserCharacInfo::setCurCharacHelpAbuseComputedRatio(unsigned int ratio)
{
    if (m_selected)
    {
    CUR->m_helpAbuseComputedRatio = ratio;
    }
