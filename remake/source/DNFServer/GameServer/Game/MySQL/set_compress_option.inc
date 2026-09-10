bool MySQL::set_compress_option()
{
    if (mysql_options(m_dbHandle, MYSQL_OPT_COMPRESS, 0) != 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x40, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x40);
        return false;
    }
    return true;
