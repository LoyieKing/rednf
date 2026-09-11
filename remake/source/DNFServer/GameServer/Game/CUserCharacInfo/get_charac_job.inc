int CUserCharacInfo::get_charac_job() const
{
    return m_selected ? (signed char)CUR->m_job : 0;
}