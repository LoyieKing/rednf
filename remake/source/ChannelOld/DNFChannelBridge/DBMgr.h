#ifndef DBMGR_H_
#define DBMGR_H_

struct st_mysql;
typedef struct st_mysql MYSQL;
struct st_mysql_res;
typedef struct st_mysql_res MYSQL_RES;
typedef char** MYSQL_ROW;

#ifdef __cplusplus
extern "C" {
#endif

MYSQL* mysql_init(MYSQL* mysql);
MYSQL* mysql_real_connect(MYSQL* mysql, const char* host, const char* user, const char* passwd,
                          const char* db, unsigned int port, const char* unix_socket, unsigned long clientflag);
int mysql_query(MYSQL* mysql, const char* q);
MYSQL_RES* mysql_store_result(MYSQL* mysql);
MYSQL_ROW mysql_fetch_row(MYSQL_RES* result);
void mysql_free_result(MYSQL_RES* result);
void mysql_close(MYSQL* mysql);
unsigned int mysql_errno(MYSQL* mysql);

#ifdef __cplusplus
}
#endif

class DBMgr
{
public:
    int Mysql_logon();
    void Mysql_logoff();
    void Mysql_error();
    int Mysql_relogon();
    MYSQL_RES* Mysql_query(char* query);
    MYSQL_ROW* Mysql_fetch(MYSQL_ROW* row, MYSQL_RES* res);
    void Mysql_free(MYSQL_RES* res);
    MYSQL* Mysql_gethandle()
    {
        return h_db;
    }

    MYSQL* h_db;
};

#endif // DBMGR_H_
