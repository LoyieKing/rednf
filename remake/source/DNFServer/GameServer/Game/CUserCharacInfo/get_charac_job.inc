int CUserCharacInfo::get_charac_job() const
{
    return m_selected ? (signed char)m_selected->m_field26 : 0;
}