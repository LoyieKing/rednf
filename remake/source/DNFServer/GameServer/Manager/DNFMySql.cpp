// df_manager_r — CDBHandle / CMySql（ORIG DNFMySql.cpp）
#include "DNFMySql.h"

#include <new>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "DNFFileLog.h"
#include "QueryCounter.h"

CDBHandle::CDBHandle() {}
CDBHandle::~CDBHandle() {}

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

int CMySql::exec_query()
{
    clear_result_set();
    int ret = mysql_real_query(m_mysql, m_query, m_queryLen);
    if (ret != 0)
    {
        m_lastErrno = mysql_errno(m_mysql);
        if (m_lastErrno == 0x7d5 || m_lastErrno == 0x7dd)
        {
            int pingRet = mysql_ping(m_mysql);
            if (pingRet != 0)
            {
                CMyFileLog log("exec_query", 0xa3);
                log("./log/MysqlErr.log", "DB reconnection fail. %d\n", pingRet);
            }
            return 2;
        }
        if (m_lastErrno != 0x426)
        {
            CMyFileLog log("exec_query", 0xaa);
            log("./log/MysqlErr.log", "DB error occured (%d) Query('%s')\n", m_lastErrno, m_query);
            if (m_lastErrno == 0x7d6)
            {
                CMyFileLog log("exec_query", 0xac);
                log("./log/MysqlErr.log",
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
            CQueryCounter* p1 = CQueryCounterInstance();
            p1->SetResponseTime(q);
            return 0;
        }
        if (ret == 0)
            break;
    }
    CQueryCounter* p2 = CQueryCounterInstance();
    p2->SetResponseTime(q);
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
    CMyFileLog log("exec", 0xed);
    log("./log/MysqlErr.log", "Database query error. The last query('%s') has been lost.", m_query);
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
    if (len > 0xfff)
        return 0;
    m_queryLen = len;
    CQueryCounter* pCounter = CQueryCounterInstance();
    pCounter->IncreQureyCount(q);
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
    v = (unsigned int)atoi(m_row[col]);
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

char* CMySql::blob_to_str(int col, void* buf, int len)
{
    if (col < 0 || col > 9)
        return 0;
    if (buf == 0 && len > 0xfff)
        return 0;
    ((char*)this + col * 0x1001 + 0x1010)[0xd] = 0;
    if (len > 0)
    {
        char* dst = (char*)this + col * 0x1001 + 0x1010 + 0xd;
        dst += mysql_real_escape_string(m_mysql, dst, (const char*)buf, len);
        *dst++ = 0;
    }
    return (char*)this + col * 0x1001 + 0x1010 + 0xd;
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

bool CMySql::open(const char* host, const char* user, const char* pass, const char* db)
{
    if (!host || !user || !pass || !db)
        return 0;
    // 原版实际传参顺序（host, pass, db, user）
    if (!mysql_real_connect(m_mysql, host, pass, db, user, 0xcea, 0, 0))
    {
        printf("Can't connect db : ( dbname : %s, ip : %s, id : %s, pwd : %s )\n",
               user, host, pass, db);
        CMyFileLog log("open", 0x6b);
        log("./log/DBErr", "Can't connect db : ( dbname : %s, ip : %s, id : %s, pwd : %s )\n",
            user, host, pass, db);
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

int CMySql::get_n_rows() { return m_nRows; }
int CMySql::get_n_fields() { return m_nFields; }
bool CMySql::ping() { return mysql_ping(m_mysql); }
bool CMySql::init()
{
    bool b;
    b = init_db_handle();
    if (b == 0)
        return 0;
    b = set_compress_option();
    if (b == 0)
        return 0;
    b = set_read_default_grp_option();
    if (b == 0)
        return 0;
    memset(m_query, 0, 0x1001);
    m_queryLen = 0;
    m_nRows = 0;
    m_nFields = 0;
    return 1;
}

// TIME_to_ulonglong 系列（DB 时间转换，原版符号）
unsigned long long TIME_to_ulonglong_date(void* t)
{
    unsigned int* p = (unsigned int*)t;
    return (unsigned long long)(p[0] * 10000 + p[1] * 100 + p[2]);
}

unsigned long long TIME_to_ulonglong_time(void* t)
{
    unsigned int* p = (unsigned int*)t;
    return (unsigned long long)(p[3] * 10000 + p[4] * 100 + p[5]);
}

unsigned long long TIME_to_ulonglong(void* t)
{
    return 0;
}

unsigned long long TIME_to_ulonglong_datetime(void* t)
{
    return 0;
}

// ---- nothrow new/delete（原版来自 libstdc++ 弱符号）----
void* operator new(std::size_t size, const std::nothrow_t&) throw()
{
    void* p = 0;
    try
    {
        p = ::operator new(size);
    }
    catch (...)
    {
    }
    return p;
}

void operator delete(void* ptr, const std::nothrow_t&) throw()
{
    if (ptr)
        free(ptr);
}
