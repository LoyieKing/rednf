bool DBConnection::init_db_handle()
{
    if (h_db != (MYSQL*)0)
    {
        G_TraceLog()->sysLog(7, "Fail: init_db_handle");
        return false;
    }
    h_db = (MYSQL*)mysql_init(0);
    if (h_db == (MYSQL*)0)
    {
        G_TraceLog()->sysLog(7, "Fail: Can\'t Alloc DB handle memory");
        return false;
    }
    return true;
