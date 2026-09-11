void MySQL::clear_result_set()
{
    if (m_result != 0)
    {
        mysql_free_result(m_result);
    }
    m_result = 0;
    m_row = 0;
}