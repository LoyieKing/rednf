unsigned long long MySQL::getAffectedRowCount()
{
    return mysql_affected_rows(m_dbHandle);
