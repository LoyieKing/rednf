bool MySQL::open(char* host, unsigned int port, char* db, char* user, char* passwd)
{
    if (port == 0)
    {
        port = 0xcea;
    }
    strcpy(m_db, db);
    strcpy(m_host, host);
    m_port = port;
    strcpy(m_user, user);
    strcpy(m_passwd, passwd);
    if (mysql_real_connect(m_dbHandle, host, user, passwd, db, port, 0, 0) == 0)
    {
        register int err = mysql_errno(m_dbHandle);  // P87
        cMyTrace(__PRETTY_FUNCTION__, 0x138, 9, true, true)(
            "\n\t- mysql_real_connect(ip:%s, port:%d, name:%s, id:%s error='%d') fail",
            host, port, db, user, err);
        cMyTrace(__PRETTY_FUNCTION__, 0x13c, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x13c);
        return false;
    }
    return true;
