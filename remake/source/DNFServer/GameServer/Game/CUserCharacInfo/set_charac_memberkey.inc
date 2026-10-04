void CUserCharacInfo::set_charac_memberkey(unsigned int key)
{
    if (m_selected)
    {
    m_selected->m_memberkey = key;
    }
}