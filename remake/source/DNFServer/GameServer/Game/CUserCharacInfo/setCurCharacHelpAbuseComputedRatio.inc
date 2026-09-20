void CUserCharacInfo::setCurCharacHelpAbuseComputedRatio(unsigned int ratio)
{
    if (m_selected)
    {
    m_selected->m_fieldDFE = ratio;
    }
}