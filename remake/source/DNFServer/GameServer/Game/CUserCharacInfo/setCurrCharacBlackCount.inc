void CUserCharacInfo::setCurrCharacBlackCount(unsigned int count)
{
    if (m_selected)
    {
    CUR->m_blackCount = count;
    }
}