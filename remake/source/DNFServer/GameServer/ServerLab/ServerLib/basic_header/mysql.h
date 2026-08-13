#ifndef NSL_MYSQL_STUB_H_
#define NSL_MYSQL_STUB_H_

typedef struct st_mysql MYSQL;
typedef struct st_mysql_res MYSQL_RES;
typedef char** MYSQL_ROW;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

#ifdef __cplusplus
extern "C" {
#endif

MYSQL* mysql_init(MYSQL* mysql);
void mysql_close(MYSQL* mysql);
MYSQL* mysql_real_connect(MYSQL* mysql, const char* host, const char* user,
                          const char* passwd, const char* db, unsigned int port,
                          const char* unix_socket, unsigned long clientflag);
unsigned int mysql_errno(MYSQL* mysql);
int mysql_options(MYSQL* mysql, int option, const void* arg);
int mysql_real_query(MYSQL* mysql, const char* q, unsigned long length);
MYSQL_RES* mysql_store_result(MYSQL* mysql);
void mysql_free_result(MYSQL_RES* result);
MYSQL_ROW mysql_fetch_row(MYSQL_RES* result);
unsigned long* mysql_fetch_lengths(MYSQL_RES* result);
unsigned long long mysql_num_rows(MYSQL_RES* result);
unsigned int mysql_num_fields(MYSQL_RES* result);
unsigned long long mysql_affected_rows(MYSQL* mysql);
int mysql_ping(MYSQL* mysql);
unsigned long mysql_real_escape_string(MYSQL* mysql, char* to, const char* from,
                                       unsigned long length);

#ifdef __cplusplus
}
#endif

#endif // NSL_MYSQL_STUB_H_
