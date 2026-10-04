void CUserCharacInfo::SetAssaultCount(int count)
{
    if (m_selected)
    {
    m_selected->m_assaultCount = count;
    }
}