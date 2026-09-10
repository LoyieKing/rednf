bool MySQL::init()
{
    bool ret = init_db_handle();
    if (ret != true)
    {
        return false;
    }
    ret = set_compress_option();
    if (ret != true)
    {
        return false;
    }
    ret = set_read_default_grp_option();
    if (ret != true)
    {
        return false;
    }
    ret = set_charset_name_option();
    if (ret != true)
    {
        return false;
    }
    ret = set_reconnect_option();
    if (ret != true)
    {
        return false;
    }
    memset(m_query, 0, 0x6001);
    m_queryLen = 0;
    m_result = 0;
    m_lengths = 0;
    m_nRows = 0;
    m_nFields = 0;
    memset(m_host, 0, 0x10);
    memset(m_user, 0, 0x14);
    memset(m_db, 0, 0x1e);
    memset(m_passwd, 0, 0x14);
    return true;
