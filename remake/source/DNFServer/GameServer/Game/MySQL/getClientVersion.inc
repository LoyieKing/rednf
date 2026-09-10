const char* MySQL::getClientVersion()
{
    return mysql_get_client_info();
