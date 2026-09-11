void CUserCharacInfo::set_charac_memberkey(unsigned int key)
{
    if (m_selected)
    {
    CUR->m_memberkey = key;
    }
}