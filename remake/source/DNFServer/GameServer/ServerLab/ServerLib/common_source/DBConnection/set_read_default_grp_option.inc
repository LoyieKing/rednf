bool DBConnection::set_read_default_grp_option()
{
    if (mysql_options(h_db, 5, "UseSQL") != 0)
    {
        G_TraceLog()->sysLog(7, "Fail: set_read_default_grp_option");
        return false;
    }
    return true;
