// Rebuilt from df_auction_r (DWARF + Ghidra decompile), 2026-08-08
#include "DBConnection.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>

#include "TraceLog.h"

namespace nsl
{

DBConnection::DBConnection()
{
    h_db = (MYSQL*)0;
}

DBConnection::~DBConnection()
{
    close();
}

bool DBConnection::init_db_handle()
{
    if (h_db != (MYSQL*)0)
    {
        G_TraceLog()->sysLog(7, "Fail: init_db_handle");
        return false;
    }
    h_db = (MYSQL*)mysql_init(0);
    if (h_db == (MYSQL*)0)
    {
        G_TraceLog()->sysLog(7, "Fail: Can\'t Alloc DB handle memory");
        return false;
    }
    return true;
}

bool DBConnection::init()
{
    bool ret = init_db_handle();
    if (!ret)
    {
        return false;
    }
    ret = set_compress_option();
    if (!ret)
    {
        return false;
    }
    ret = set_read_default_grp_option();
    if (!ret)
    {
        return false;
    }
    memset(m_query, 0, 0x6001);
    m_query_len = 0;
    m_res = (MYSQL_RES*)0;
    m_len = (ulong*)0;
    m_num_rows = 0;
    m_num_fields = 0;
    return true;
}

void DBConnection::close()
{
    if (h_db != (MYSQL*)0)
    {
        mysql_close(h_db);
        h_db = (MYSQL*)0;
    }
}

bool DBConnection::open(char* ip, char* dbname, char* id, char* pass, unsigned int port)
{
    strncpy(dbIp_, ip, 0x10);
    dbPort_ = port;
    strncpy(dbAcc_, id, 0x14);
    strncpy(dbPwd_, pass, 0x14);
    strncpy(dbName_, dbname, 0x1e);
    if (mysql_real_connect(h_db, ip, id, pass, dbname, port, 0, 0) == 0)
    {
        unsigned int err = mysql_errno(h_db);
        G_TraceLog()->errorLog("%s %u %s %s %s error=\'%d\' FAILED", ip, port, dbname, id, pass,
                               err);
        err = mysql_errno(h_db);
        printf("%s %u %s %s %s error=\'%d\' FAILED\n", ip, port, dbname, id, pass, err);
        return false;
    }
    else
    {
        printf("%s %s SUCCESS\n", ip, dbname);
        return true;
    }
}

bool DBConnection::set_query(const char* format, ...)
{
    va_list vl;
    va_start(vl, format);
    vsprintf(m_query, format, vl);
    va_end(vl);
    int len = (int)strlen(m_query);
    if (len <= 0x5fff)
    {
        m_query_len = len;
        return true;
    }
    return false;
}

int DBConnection::exec(bool bLog)
{
    int iret;
    int i;
    for (i = 0; i < 5; i = i + 1)
    {
        iret = exec_query();
        if (iret == 1)
        {
            return 1;
        }
        if (iret == 0)
        {
            break;
        }
    }
    if (iret == 0)
    {
        m_res = (MYSQL_RES*)mysql_store_result(h_db);
        if (m_res != (MYSQL_RES*)0)
        {
            m_num_rows = (ulong)mysql_num_rows(m_res);
            m_num_fields = mysql_num_fields(m_res);
        }
        else
        {
            m_num_rows = 0;
            m_num_fields = 0;
        }
        return 0;
    }
    else
    {
        if (bLog)
        {
            G_TraceLog()->sysLog(7, "Database query error. The last query(\'%s\') has been lost.(%d)",
                                 m_query, m_db_err);
        }
        return 2;
    }
}

void DBConnection::clear_result_set()
{
    if (m_res != (MYSQL_RES*)0)
    {
        mysql_free_result(m_res);
    }
    m_res = (MYSQL_RES*)0;
    m_row = (MYSQL_ROW)0;
}

bool DBConnection::fetch()
{
    if (m_res == (MYSQL_RES*)0)
    {
        return false;
    }
    m_row = (MYSQL_ROW)mysql_fetch_row(m_res);
    if (m_row == (MYSQL_ROW)0)
    {
        return false;
    }
    m_len = (ulong*)mysql_fetch_lengths(m_res);
    return true;
}

void DBConnection::ping()
{
    mysql_ping(h_db);
}

bool DBConnection::set_compress_option()
{
    if (mysql_options(h_db, 1, 0) != 0)
    {
        G_TraceLog()->sysLog(7, "Fail: set_compress_option");
        return false;
    }
    return true;
}

bool DBConnection::set_read_default_grp_option()
{
    if (mysql_options(h_db, 5, "UseSQL") != 0)
    {
        G_TraceLog()->sysLog(7, "Fail: set_read_default_grp_option");
        return false;
    }
    return true;
}

bool DBConnection::set_charset_name_option()
{
    if (mysql_options(h_db, 7, "utf8") != 0)
    {
        G_TraceLog()->sysLog(7, "Fail: Can\'t set mysql charset name option");
        return false;
    }
    return true;
}

bool DBConnection::set_reconnect_option()
{
    return true;
}

unsigned long DBConnection::escape_string(char* to, const char* from)
{
    return (unsigned long)mysql_real_escape_string(h_db, to, from, strlen(from));
}

char* DBConnection::blob_to_str(int idx, void* blob, int size)
{
    if (((idx < 0) || (9 < idx)) || (blob == (void*)0) || (0x5fff < size))
    {
        G_TraceLog()->errorLog("set SESSION wait_timeout Error");
    }
    char* p = temp_buf[idx];
    p[0] = '\0';
    if (0 < size)
    {
        int ret = (int)mysql_real_escape_string(h_db, p, (const char*)blob, size);
        p[ret] = '\0';
    }
    return temp_buf[idx];
}

unsigned long long DBConnection::getAffectedRowCount()
{
    return mysql_affected_rows(h_db);
}

int DBConnection::exec_query()
{
    clear_result_set();
    int db_ret = mysql_real_query(h_db, m_query, m_query_len);
    if (db_ret != 0)
    {
        m_db_err = (int)mysql_errno(h_db);
        int iVar2 = m_db_err;
        G_TraceLog()->sysLog(7, "Fail: mysql_real_query(), %d", iVar2);
        if ((m_db_err == 0x7d5) || (m_db_err == 0x7dd) || (m_db_err == 0x7d3) ||
            (m_db_err == 0x7d6))
        {
            G_TraceLog()->sysLog(7, "Fail: DB connection lost, reconnecting...", iVar2);
            if (mysql_ping(h_db) != 0)
            {
                int err = (int)mysql_errno(h_db);
                if (err == 0x7d6)
                {
                    int connect_ret = (int)mysql_real_connect(h_db, dbIp_, dbAcc_, dbPwd_, dbName_,
                                                              dbPort_, 0, 0);
                    if (connect_ret == 0)
                    {
                        unsigned int errno2 = mysql_errno(h_db);
                        G_TraceLog()->sysLog(7, "DB reconnection fail. err_no(%d)", errno2);
                    }
                    else
                    {
                        G_TraceLog()->sysLog(7, "DB Reconnect By Server Gone Error");
                    }
                    G_TraceLog()->sysLog(
                        7,
                        "DB reconnection fail. %d, %d (dbname : %s, ip : %s, port : %d, id : %s, pwd : %s)",
                        iVar2, mysql_errno(h_db), dbName_, dbIp_, dbPort_, dbAcc_, dbPwd_);
                }
                else
                {
                    int err2 = (int)mysql_errno(h_db);
                    G_TraceLog()->sysLog(7, "DB reconnection fail. %d, %d", iVar2, err2);
                }
            }
            db_ret = 2;
        }
        else
        {
            if (m_db_err != 0x426)
            {
                G_TraceLog()->sysLog(7, "DB error occured (%d) Query(\'%s\')",
                                     mysql_errno(h_db), m_query);
            }
            db_ret = 1;
        }
    }
    else
    {
        db_ret = 0;
    }
    return db_ret;
}


} // namespace nsl
