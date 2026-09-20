void CUserCharacInfo::setCurrCharacBlackCount(unsigned int count)
{
    if (m_selected)
    {
    m_selected->m_fieldE3C = count;
    }
}