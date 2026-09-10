bool TaiwanCash::init(char* host, unsigned int port,
                      char* db, char* user, char* pass)
{
    m_mysql.init();
    return m_mysql.open(host, port, db, user, pass);
