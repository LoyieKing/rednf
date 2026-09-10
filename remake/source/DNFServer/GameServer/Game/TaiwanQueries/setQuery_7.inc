bool CeraPointQuery::setQuery(MySQL& db)
{
    return db.set_query(
        "call usp_point_process('%s', '%s', '%c', %u, %d, %d , %d, @out_code)",
        m_account, m_p2, static_cast<char>(m_c8), m_price,
        static_cast<int>(m_c6),
        static_cast<int>(StringToNumber(m_uidStr)),
        static_cast<int>(m_c7));
