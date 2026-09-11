bool MySQL::set_read_default_grp_option()
{
    if (mysql_options(m_dbHandle, MYSQL_READ_DEFAULT_GROUP, "UseSQL") != 0)
    {
        cMyTrace trace(__PRETTY_FUNCTION__, 0x4f, 5);
        trace("[%s][%d]", __PRETTY_FUNCTION__, 0x4f);
        return false;
    }
    return true;
}