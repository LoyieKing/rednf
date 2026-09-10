bool DBConnection::set_compress_option()
{
    if (mysql_options(h_db, 1, 0) != 0)
    {
        G_TraceLog()->sysLog(7, "Fail: set_compress_option");
        return false;
    }
    return true;
