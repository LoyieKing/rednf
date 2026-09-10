void DBConnection::close()
{
    if (h_db != (MYSQL*)0)
    {
        mysql_close(h_db);
        h_db = (MYSQL*)0;
    }
