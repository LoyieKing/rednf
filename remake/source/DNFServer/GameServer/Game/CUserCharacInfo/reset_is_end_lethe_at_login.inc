void CUserCharacInfo::reset_is_end_lethe_at_login()
{
    if (m_selected)
    {
    m_selected->m_endLetheAtLogin = 0;
    }
}