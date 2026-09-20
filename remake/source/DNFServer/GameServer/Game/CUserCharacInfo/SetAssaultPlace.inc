void CUserCharacInfo::SetAssaultPlace(int place)
{
    if (m_selected)
    {
    m_selected->m_fieldE41 = place;
    }
}