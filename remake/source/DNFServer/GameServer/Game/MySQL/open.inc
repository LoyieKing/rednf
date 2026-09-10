bool MySQL::open(char* host, char* db, char* user, char* passwd)
{
    strcpy(m_db, db);
    strcpy(m_host, host);
    strcpy(m_user, user);
    strcpy(m_passwd, passwd);
    if (mysql_real_connect(m_dbHandle, host, user, passwd, db, 0xcea, 0, 0x400) == 0)
    {
        register int err = mysql_errno(m_dbHandle);  // P87：ORIG ebx 跨 C1 调用缓存
        cMyTrace(__PRETTY_FUNCTION__, 0x10a, 9, true, true)(
            "\n\t- mysql_real_connect(ip:%s, port:%d, name:%s, id:%s error='%d') fail",
            host, 0xcea, db, user, err);
        cMyTrace(__PRETTY_FUNCTION__, 0x10e, 5)("[%s][%d]", __PRETTY_FUNCTION__, 0x10e);
        return false;
    }
    return true;
