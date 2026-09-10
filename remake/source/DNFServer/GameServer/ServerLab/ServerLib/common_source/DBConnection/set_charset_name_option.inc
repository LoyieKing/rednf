bool DBConnection::set_charset_name_option()
{
    if (mysql_options(h_db, 7, "utf8") != 0)
    {
        G_TraceLog()->sysLog(7, "Fail: Can\'t set mysql charset name option");
        return false;
    }
    return true;
