unsigned long long DBConnection::getAffectedRowCount()
{
    return mysql_affected_rows(h_db);
