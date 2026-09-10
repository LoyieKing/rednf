void DBConnection::ping()
{
    mysql_ping(h_db);
