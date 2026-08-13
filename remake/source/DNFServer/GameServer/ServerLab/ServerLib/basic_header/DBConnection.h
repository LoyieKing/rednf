#ifndef NSL_DBCONNECTION_H_
#define NSL_DBCONNECTION_H_

#include <stdlib.h>
#include <string.h>

#include "mysql.h"

namespace nsl
{

class DBConnection
{
public:
    DBConnection();
    ~DBConnection();
    bool init_db_handle();
    bool init();
    void close();
    bool open(char* ip, char* dbname, char* id, char* pass, unsigned int port);
    bool set_query(const char* format, ...);
    int exec(bool bLog);
    void clear_result_set();
    bool fetch();
    void ping();
    bool set_compress_option();
    bool set_read_default_grp_option();
    bool set_charset_name_option();
    bool set_reconnect_option();
    unsigned long escape_string(char* to, const char* from);
    char* blob_to_str(int idx, void* blob, int size);
    unsigned long long getAffectedRowCount();
    int exec_query();
    bool is_valid_col(int col0)
    {
        if ((col0 < 0) || ((int)m_num_fields <= col0))
        {
            return false;
        }
        return true;
    }
    ulong get_n_rows()
    {
        return m_num_rows;
    }
    bool get_str(int col0, char* buf, int buf_size)
    {
        if (m_row == (MYSQL_ROW)0 || !is_valid_col(col0) || buf_size < 1)
        {
            return false;
        }
        strncpy(buf, m_row[col0], buf_size);
        buf[buf_size + -1] = '\0';
        return true;
    }
    bool get_binary(int col0, void* buf, int buf_size)
    {
        if (m_row == (MYSQL_ROW)0 || !is_valid_col(col0) || buf_size < 1)
        {
            return false;
        }
        int copy_size = ((int)m_len[col0] < buf_size) ? (int)m_len[col0] : buf_size;
        memcpy(buf, m_row[col0], copy_size);
        return true;
    }
    bool get_int(int col0, int& v)
    {
        if (m_row == (MYSQL_ROW)0 || !is_valid_col(col0))
        {
            return false;
        }
        v = atoi(m_row[col0]);
        return true;
    }
    bool get_uint(int col0, uint& v)
    {
        if (m_row == (MYSQL_ROW)0 || !is_valid_col(col0))
        {
            return false;
        }
        v = (uint)atoi(m_row[col0]);
        return true;
    }
    bool get_long(int col0, long& v)
    {
        if (m_row == (MYSQL_ROW)0 || !is_valid_col(col0))
        {
            return false;
        }
        v = atoi(m_row[col0]);
        return true;
    }
    bool get_ulong(int col0, ulong& v)
    {
        if (m_row == (MYSQL_ROW)0 || !is_valid_col(col0))
        {
            return false;
        }
        v = atoi(m_row[col0]);
        return true;
    }
    bool get_short(int col0, short& v)
    {
        if (m_row == (MYSQL_ROW)0 || !is_valid_col(col0))
        {
            return false;
        }
        v = (short)atoi(m_row[col0]);
        return true;
    }
    bool get_ushort(int col0, ushort& v)
    {
        if (m_row == (MYSQL_ROW)0 || !is_valid_col(col0))
        {
            return false;
        }
        v = (ushort)atoi(m_row[col0]);
        return true;
    }
    bool get_bool(int col0, bool& v)
    {
        if (m_row == (MYSQL_ROW)0 || !is_valid_col(col0))
        {
            return false;
        }
        v = atoi(m_row[col0]) != 0;
        return true;
    }
    bool get_ubyte(int col0, unsigned char& v)
    {
        if (m_row == (MYSQL_ROW)0 || !is_valid_col(col0))
        {
            return false;
        }
        v = (unsigned char)atoi(m_row[col0]);
        return true;
    }
    bool get_longlong(int col0, long long& v)
    {
        if (m_row == (MYSQL_ROW)0 || !is_valid_col(col0))
        {
            return false;
        }
        v = strtoll(m_row[col0], (char**)0, 10);
        return true;
    }
    bool get_int64(int col0, unsigned long long& v)
    {
        if (m_row == (MYSQL_ROW)0 || !is_valid_col(col0))
        {
            return false;
        }
        v = strtoull(m_row[col0], (char**)0, 10);
        return true;
    }

private:
    MYSQL* h_db;
    MYSQL_RES* m_res;
    MYSQL_ROW m_row;
    ulong* m_len;
    ulong m_num_rows;
    unsigned int m_num_fields;
    char m_query[0x6001];
    char temp_buf[10][0x6001];
    int m_query_len;
    int m_db_err;
    char dbIp_[0x10];
    unsigned int dbPort_;
    char dbAcc_[0x14];
    char dbPwd_[0x14];
    char dbName_[0x1e];
};

} // namespace nsl

#endif // NSL_DBCONNECTION_H_
