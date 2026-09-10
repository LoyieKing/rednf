bool DBConnection::init()
{
    bool ret = init_db_handle();
    if (!ret)
    {
        return false;
    }
    ret = set_compress_option();
    if (!ret)
    {
        return false;
    }
    ret = set_read_default_grp_option();
    if (!ret)
    {
        return false;
    }
    memset(m_query, 0, 0x6001);
    m_query_len = 0;
    m_res = (MYSQL_RES*)0;
    m_len = (ulong*)0;
    m_num_rows = 0;
    m_num_fields = 0;
    return true;
