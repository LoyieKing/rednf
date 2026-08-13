// df_dbmw_r - DNFMySql (ORIG DNFMySql.cpp)
#include "DBMWCommon.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/resource.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/times.h>

#include "DNFFileLog.h"
#include "DNFFunctionLib.h"

int getErrno();
CQueryCounter* CQueryCounterInstance();

CMySql::CMySql()
{
    m_mysql = 0;
    m_result = 0;
    m_lengths = 0;
}
CMySql::~CMySql()
{
    close();
}
void CMySql::close()
{
    if (m_mysql)
    {
        mysql_close(m_mysql);
        m_mysql = 0;
    }
}
bool CMySql::init_db_handle()
{
    if (m_mysql)
        return 0;
    m_mysql = mysql_init(0);
    if (!m_mysql)
        return 0;
    return 1;
}
char* CMySql::blob_to_str(int col, void* buf, int len)
{
    if (col < 0 || col > 9)
        return 0;
    if (buf == 0 && len > 0x5fff)
        return 0;
    char* base = (char*)this + col * 0x6001 + 0x6070;
    base[9] = 0;
    if (len > 0)
    {
        char* dst = base + 9;
        dst += mysql_real_escape_string(m_mysql, dst, (const char*)buf, len);
        *dst++ = 0;
    }
    return base + 9;
}
bool CMySql::set_compress_option()
{
    if (mysql_options(m_mysql, MYSQL_OPT_COMPRESS, 0) != 0)
        return 0;
    return 1;
}
bool CMySql::set_read_default_grp_option()
{
    if (mysql_options(m_mysql, MYSQL_READ_DEFAULT_GROUP, "UseSQL") != 0)
        return 0;
    return 1;
}
bool CMySql::set_charset_name_option()
{
    return 1;
}
bool CMySql::set_reconnect_option()
{
    return 1;
}
unsigned long long CMySql::getAffectedRowCount()
{
    return mysql_affected_rows(m_mysql);
}
bool CMySql::get_int(int col, unsigned int& v)
{
    return get_uint(col, v);
}
bool CMySql::get_int(int col, unsigned long long& v)
{
    return get_ulonglong(col, v);
}
bool CMySql::get_uint(int col, unsigned long long& v)
{
    return get_ulonglong(col, v);
}
bool CMySql::get_ulonglong(int col, unsigned long long& v)
{
    if (!(m_row && is_valid_col(col)))
        return 0;
    v = strtoull(m_row[col], 0, 10);
    return 1;
}
const char* CMySql::get_quest_str() const
{
    return m_query;
}
char* CMySql::escape_string(char* dst, char const* src)
{
    return (char*)mysql_real_escape_string(m_mysql, dst, src, strlen(src));
}
int CMySql::exec_query()
{
    clear_result_set();
    int ret = mysql_real_query(m_mysql, m_query, m_queryLen);
    if (ret != 0)
    {
        m_lastErrno = mysql_errno(m_mysql);
        if (m_lastErrno == 0x7d5 || m_lastErrno == 0x7dd ||
            m_lastErrno == 0x7d6)
        {
            int ping = mysql_ping(m_mysql);
            if (ping != 0)
            {
                if (mysql_errno(m_mysql) == 0x7d6)
                {
                    if (!mysql_real_connect(m_mysql, m_host, m_pass, m_db,
                                            m_user, 0xcea, 0, 0x400))
                    {
                        register int e2 = mysql_errno(m_mysql);
                        CMyFileLog log(__FUNCTION__, 0x118);
                        log("./log/MysqlErr.log",
                            "DB reconnection fail. err_no(%d)\n", e2);
                    }
                    else
                    {
                        CMyFileLog log(__FUNCTION__, 0x11c);
                        log("./log/MysqlErr.log",
                            "DB Reconnect By Server Gone Error\n");
                    }
                }
            }
            return 2;
        }
        if (m_lastErrno != 0x426)
        {
            register char* q = m_query;
            register int err = m_lastErrno;
            CMyFileLog log(__FUNCTION__, 0x12a);
            log("./log/MysqlErr.log",
                "DB error occured (%d) Query('%s')\n", err, q);
            if (m_lastErrno == 0x7d6)
            {
                CMyFileLog log2(__FUNCTION__, 300);
                log2("./log/MysqlErr.log",
                     "CMySql::open() Function Error!\tCheck Connection First, Must Be Not Connected!\n");
            }
        }
        return 1;
    }
    return 0;
}
bool CMySql::exec(unsigned int q)
{
    int ret;
    for (int i = 0; i <= 4; i++)
    {
        ret = exec_query();
        if (ret == 1)
        {
            CQueryCounter* p = CQueryCounterInstance();
            p->SetResponseTime(q);
            register char* qs = m_query;
            CMyFileLog log(__FUNCTION__, 0x14e);
            log("./log/MysqlErr.log",
                "Database query error. The last query('%s') has been lost. iret == R_FAIL",
                qs);
            return 0;
        }
        if (ret == 0)
            break;
    }
    CQueryCounter* p = CQueryCounterInstance();
    p->SetResponseTime(q);
    if (ret == 0)
    {
        m_result = mysql_store_result(m_mysql);
        if (m_result)
        {
            m_nRows = mysql_num_rows(m_result);
            m_nFields = mysql_num_fields(m_result);
        }
        else
        {
            m_nRows = 0;
            m_nFields = 0;
        }
        return 1;
    }
    register char* qs = m_query;
    CMyFileLog log(__FUNCTION__, 0x16e);
    log("./log/MysqlErr.log",
        "Database query error. The last query('%s') has been lost.", qs);
    return 0;
}
bool CMySql::fetch()
{
    if (!m_result)
        return 0;
    m_row = mysql_fetch_row(m_result);
    if (m_row)
    {
        m_lengths = mysql_fetch_lengths(m_result);
        return 1;
    }
    return 0;
}
void CMySql::clear_result_set()
{
    if (m_result)
        mysql_free_result(m_result);
    m_result = 0;
    m_row = 0;
}
bool CMySql::set_query(unsigned int q, char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsprintf(m_query, fmt, ap);
    va_end(ap);
    int len = strlen(m_query);
    if (len > 0x5fff)
        return 0;
    this->m_queryLen = len;
    CQueryCounter* pCounter = CQueryCounterInstance();
    pCounter->IncreQureyCount(q, fmt);
    return 1;
}
bool CMySql::get_int(int col, int& v)
{
    if (!(m_row && is_valid_col(col)))
        return 0;
    v = atoi(m_row[col]);
    return 1;
}
bool CMySql::get_uint(int col, unsigned int& v)
{
    if (!(m_row && is_valid_col(col)))
        return 0;
    unsigned long uRet = strtoul(m_row[col], 0, 10);
    v = (unsigned int)uRet;
    return 1;
}
bool CMySql::get_short(int col, short& v)
{
    if (!(m_row && is_valid_col(col)))
        return 0;
    v = (short)atoi(m_row[col]);
    return 1;
}
bool CMySql::get_short(int col, int& v)
{
    if (!(m_row && is_valid_col(col)))
        return 0;
    short s = (short)atoi(m_row[col]);
    v = s;
    return 1;
}
bool CMySql::get_ushort(int col, unsigned short& v)
{
    if (!(m_row && is_valid_col(col)))
        return 0;
    v = (unsigned short)atoi(m_row[col]);
    return 1;
}
bool CMySql::get_ushort(int col, int& v)
{
    if (!(m_row && is_valid_col(col)))
        return 0;
    unsigned short s = (unsigned short)atoi(m_row[col]);
    v = s;
    return 1;
}
bool CMySql::get_byte(int col, char& v)
{
    if (!(m_row && is_valid_col(col)))
        return 0;
    v = (char)atoi(m_row[col]);
    return 1;
}
bool CMySql::get_byte(int col, int& v)
{
    if (!(m_row && is_valid_col(col)))
        return 0;
    char c = (char)atoi(m_row[col]);
    v = c;
    return 1;
}
bool CMySql::get_ubyte(int col, unsigned char& v)
{
    if (!(m_row && is_valid_col(col)))
        return 0;
    v = (unsigned char)atoi(m_row[col]);
    return 1;
}
bool CMySql::get_ubyte(int col, int& v)
{
    if (!(m_row && is_valid_col(col)))
        return 0;
    unsigned char c = (unsigned char)atoi(m_row[col]);
    v = c;
    return 1;
}
bool CMySql::get_str(int col, char* buf, int len)
{
    if (!(m_row && is_valid_col(col)) || len <= 0)
        return 0;
    strncpy(buf, m_row[col], len);
    buf[len - 1] = 0;
    return 1;
}
bool CMySql::get_binary(int col, void* buf, int len)
{
    if (!(m_row && is_valid_col(col)) || len <= 0)
        return 0;
    int copyLen = (int)m_lengths[col] < len ? (int)m_lengths[col] : len;
    memcpy(buf, m_row[col], copyLen);
    return 1;
}
char CMySql::open(const char* host, const char* user, const char* pass, const char* db)
{
    if (!host || !user || !pass || !db)
        return 0;
    strcpy(m_user, user);
    strcpy(m_host, host);
    strcpy(m_pass, pass);
    strcpy(m_db, db);
    m_port = 0xcea;
    // 原版实际传参顺序（host, pass, db, user），client_flag 0x400
    if (!mysql_real_connect(m_mysql, host, pass, db, user, 0xcea, 0, 0x400))
    {
        m_lastErrno = mysql_errno(m_mysql);
        printf("Can't connect db : ( dbname : %s, ip : %s, id : %s )\n",
               user, host, pass);
        CMyFileLog log(__FUNCTION__, 0xad);
        log("./log/DBErr", "Can't connect db : ( dbname : %s, ip : %s, id : %s )\n",
            user, host, pass);
        return 0;
    }
    return 1;
}
char CMySql::open(const char* host, unsigned int port, const char* user, const char* pass,
                  const char* db)
{
    if (!host || !user || !pass || !db)
        return 0;
    strcpy(m_user, user);
    strcpy(m_host, host);
    m_port = port;
    strcpy(m_pass, pass);
    strcpy(m_db, db);
    if (!mysql_real_connect(m_mysql, host, pass, db, user, port, 0, 0))
    {
        m_lastErrno = mysql_errno(m_mysql);
        printf("Can't connect db : ( dbname : %s, ip : %s, id : %s )\n", user, host, pass);
        CMyFileLog log(__FUNCTION__, 0xd2);
        log("./log/DBErr", "Can't connect db : ( dbname : %s, ip : %s, id : %s )\n",
            user, host, pass);
        return 0;
    }
    return 1;
}
bool CMySql::is_valid_col(int col)
{
    if (col < 0 || m_nFields <= col)
        return 0;
    return 1;
}
bool CMySql::init()
{
    bool r;
    r = init_db_handle();
    if (!r)
        return 0;
    r = set_compress_option();
    if (!r)
        return 0;
    r = set_read_default_grp_option();
    if (!r)
        return 0;
    r = set_charset_name_option();
    if (!r)
        return 0;
    r = set_reconnect_option();
    if (!r)
        return 0;
    memset(m_query, 0, 0x6001);
    m_queryLen = 0;
    m_nRows = 0;
    m_nFields = 0;
    memset(m_host, 0, 0x10);
    memset(m_pass, 0, 0x14);
    memset(m_user, 0, 0x1e);
    memset(m_db, 0, 0x14);
    m_port = 0xcea;
    return 1;
}
int CMySql::get_n_rows() { return m_nRows; }
int CMySql::get_n_fields() { return m_nFields; }
char CMySql::ping() { return mysql_ping(m_mysql); }
