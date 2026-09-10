unsigned long DBConnection::escape_string(char* to, const char* from)
{
    return (unsigned long)mysql_real_escape_string(h_db, to, from, strlen(from));
