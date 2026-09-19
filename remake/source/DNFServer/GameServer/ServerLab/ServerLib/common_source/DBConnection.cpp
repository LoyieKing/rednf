// ============================================================================
// [Main·2026-09-10 round5 拆分 · ① 部件包含式] 本文件是**聚合器**：
//   每个完整函数块已移入 DBConnection/<Func>.inc（不参与编译、内部不含 #include）。
//   本文件逐行保留原文的一切 TU 级实体（顶部注释 / #include / 数据定义 /
//   namespace 开闭 / 宏 / 视图 / 函数间的任何非函数行），只把**完整函数块**
//   换成一行 #include；include 的**位置即原函数定义位置**。
//   ⇒ 仍是**同一个 TU**：旗标/头集/后端/CMake 条目自动继承；GI 键与
//     __static_initialization_and_destruction_0ii 仍各一份（不新增符号）；
//     outgoing 帧继承（P210②）/ EH cleanup / weak COMDAT 取值构造性保持。
//   ⚠ 部件按**花括号深度**切分（v3）：函数体内列 0 的 #define/struct/#pragma
//     不会再截断函数。改动 include 顺序会改 codegen。
// ============================================================================
// Rebuilt from df_auction_r (DWARF + Ghidra remake), 2026-08-08
#include "DBConnection.h"

#include <errmsg.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>

#include "TraceLog.h"

namespace nsl
{

#include "DBConnection/DBConnection.inc"
{
    h_db = (MYSQL*)0;
}

#include "DBConnection/dtor_DBConnection.inc"
{
    close();
}

#include "DBConnection/init_db_handle.inc"
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

#include "DBConnection/init.inc"
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

#include "DBConnection/close.inc"
{
    if (h_db != (MYSQL*)0)
    {
        mysql_close(h_db);
        h_db = (MYSQL*)0;
    }
}

#include "DBConnection/open.inc"
{
    strncpy(dbIp_, ip, 0x10);
    dbPort_ = port;
    strncpy(dbAcc_, id, 0x14);
    strncpy(dbPwd_, pass, 0x14);
    strncpy(dbName_, dbname, 0x1e);
    if (mysql_real_connect(h_db, ip, id, pass, dbname, port, 0, 0) == 0)
    {
        // ORIG: err 与 G_TraceLog() 均为内联临时（err 保存在 ebx 跨调用）。
        G_TraceLog()->errorLog("%s %u %s %s %s error=\'%d\' FAILED", ip, port, dbname, id, pass,
                               mysql_errno(h_db));
        printf("%s %u %s %s %s error=\'%d\' FAILED\n", ip, port, dbname, id, pass,
               mysql_errno(h_db));
        return false;
    }
    printf("%s %s SUCCESS\n", ip, dbname);
    return true;
}

#include "DBConnection/set_query.inc"
{
    va_list vl;
    va_start(vl, format);
    vsprintf(m_query, format, vl);
    va_end(vl);
    int len = (int)strlen(m_query);
    if (0x5fff < len)
    {
        return false;
    }
    m_query_len = len;
    return true;
}

#include "DBConnection/exec.inc"
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

#include "DBConnection/clear_result_set.inc"
{
    if (m_res != (MYSQL_RES*)0)
    {
        mysql_free_result(m_res);
    }
    m_res = (MYSQL_RES*)0;
    m_row = (MYSQL_ROW)0;
}

#include "DBConnection/fetch.inc"
{
    if (m_res == (MYSQL_RES*)0)
    {
        return false;
    }
    m_row = (MYSQL_ROW)mysql_fetch_row(m_res);
    if (m_row != (MYSQL_ROW)0)
    {
        m_len = (ulong*)mysql_fetch_lengths(m_res);
        return true;
    }
    return false;
}

#include "DBConnection/ping.inc"
{
    mysql_ping(h_db);
}

#include "DBConnection/set_compress_option.inc"
{
    if (mysql_options(h_db, 1, 0) != 0)
    {
        G_TraceLog()->sysLog(7, "Fail: set_compress_option");
        return false;
    }
    return true;
}

#include "DBConnection/set_read_default_grp_option.inc"
{
    if (mysql_options(h_db, 5, "UseSQL") != 0)
    {
        G_TraceLog()->sysLog(7, "Fail: set_read_default_grp_option");
        return false;
    }
    return true;
}

#include "DBConnection/set_charset_name_option.inc"
{
    if (mysql_options(h_db, 7, "utf8") != 0)
    {
        G_TraceLog()->sysLog(7, "Fail: Can\'t set mysql charset name option");
        return false;
    }
    return true;
}

#include "DBConnection/set_reconnect_option.inc"
{
    return true;
}

#include "DBConnection/escape_string.inc"
{
    return (unsigned long)mysql_real_escape_string(h_db, to, from, strlen(from));
}

#include "DBConnection/blob_to_str.inc"
{
    if (((idx < 0) || (9 < idx)) || (blob == (void*)0) || (0x5fff < size))
    {
        G_TraceLog()->errorLog("set SESSION wait_timeout Error");
    }
    temp_buf[idx][0] = '\0';
    if (0 < size)
    {
        char* p = temp_buf[idx];
        p = p + (int)mysql_real_escape_string(h_db, p, (const char*)blob, size);
        *p = '\0';
        p = p + 1;
    }
    return temp_buf[idx];
}

#include "DBConnection/getAffectedRowCount.inc"
{
    return mysql_affected_rows(h_db);
}

#include "DBConnection/exec_query.inc"
{
    clear_result_set();
    int db_ret = mysql_real_query(h_db, m_query, m_query_len);
    if (db_ret != 0)
    {
        m_db_err = (int)mysql_errno(h_db);
        // ORIG: m_db_err 内联（跨 G_TraceLog 调用保存在 ebx）。
        G_TraceLog()->sysLog(7, "Fail: mysql_real_query(), %d", m_db_err);
        if ((m_db_err == 0x7d5) || (m_db_err == 0x7dd) || (m_db_err == 0x7d3) ||
            (m_db_err == 0x7d6))
        {
            G_TraceLog()->sysLog(7, "Fail: DB connection lost, reconnecting...");
            int ping_ret = mysql_ping(h_db);
            if (ping_ret != 0)
            {
                // ORIG: 条件内联（sete+test），无 bool 局部。
                if (mysql_errno(h_db) == CR_SERVER_GONE_ERROR)
                {
                    if (mysql_real_connect(h_db, dbIp_, dbAcc_, dbPwd_, dbName_, dbPort_, 0, 0) ==
                        0)
                    {
                        G_TraceLog()->sysLog(7, "DB reconnection fail. err_no(%d)",
                                             mysql_errno(h_db));
                    }
                    else
                    {
                        G_TraceLog()->sysLog(7, "DB Reconnect By Server Gone Error");
                    }
                    G_TraceLog()->sysLog(
                        7,
                        "DB reconnection fail. %d, %d (dbname : %s, ip : %s, port : %d, id : %s, pwd : %s)",
                        ping_ret, mysql_errno(h_db), dbName_, dbIp_, dbPort_, dbAcc_, dbPwd_);
                }
                else
                {
                    G_TraceLog()->sysLog(7, "DB reconnection fail. %d, %d", ping_ret,
                                         mysql_errno(h_db));
                }
            }
            return 2;
        }
        if (m_db_err != 0x426)
        {
            G_TraceLog()->sysLog(7, "DB error occured (%d) Query(\'%s\')", mysql_errno(h_db),
                                 m_query);
        }
        return 1;
    }
    return 0;
}


} // namespace nsl
