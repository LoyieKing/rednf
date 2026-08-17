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

enum mysql_option
{
  MYSQL_OPT_CONNECT_TIMEOUT, MYSQL_OPT_COMPRESS, MYSQL_OPT_NAMED_PIPE,
  MYSQL_INIT_COMMAND, MYSQL_READ_DEFAULT_FILE, MYSQL_READ_DEFAULT_GROUP,
  MYSQL_SET_CHARSET_DIR, MYSQL_SET_CHARSET_NAME, MYSQL_OPT_LOCAL_INFILE,
  MYSQL_OPT_PROTOCOL, MYSQL_SHARED_MEMORY_BASE_NAME, MYSQL_OPT_READ_TIMEOUT,
  MYSQL_OPT_WRITE_TIMEOUT, MYSQL_OPT_USE_RESULT,
  MYSQL_OPT_USE_REMOTE_CONNECTION, MYSQL_OPT_USE_EMBEDDED_CONNECTION,
  MYSQL_OPT_GUESS_CONNECTION, MYSQL_SET_CLIENT_IP, MYSQL_SECURE_AUTH,
  MYSQL_REPORT_DATA_TRUNCATION, MYSQL_OPT_RECONNECT,
  MYSQL_OPT_SSL_VERIFY_SERVER_CERT
};

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
const char* mysql_get_server_info(MYSQL* mysql);
const char* mysql_get_client_info(void);

#ifdef __cplusplus
}
#endif

#endif // NSL_MYSQL_STUB_H_
