bool MySQL::init_db_handle()
{
    if (m_dbHandle != 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x81, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x81);
        return false;
    }
    m_dbHandle = mysql_init(0);
    if (m_dbHandle == 0)
    {
        cMyTrace(__PRETTY_FUNCTION__, 0x8c, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x8c);
        return false;
    }
    return true;
}