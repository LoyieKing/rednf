#ifndef DNF_GAME_MYSQL_H_
#define DNF_GAME_MYSQL_H_

// ============================================================================
// df_game_r 还原 —— MySQL（libmysqlclient C++ 封装，D4 批次）
// 参照 docs/class_func_reports/MySQL.md + df_game_r 反汇编。
// 布局（sizeof = 0x4208c，对象池数组元素间距）：
//   +0x00  CQueryCounter*  m_pQueryCounter
//   +0x04  MYSQL*          m_dbHandle
//   +0x08  MYSQL_RES*      m_result
//   +0x0c  MYSQL_ROW       m_row
//   +0x10  unsigned long*  m_lengths
//   +0x14  int             m_nRows
//   +0x18  int             m_nFields
//   +0x1c  char            m_query[0x6001]
//   +0x601d char           m_blob[10][0x6001]
//   +0x42028 size_t        m_queryLen
//   +0x4202c int           m_dbError
//   +0x42030 char          m_host[0x10]
//   +0x42040 unsigned int  m_port
//   +0x42044 char          m_user[0x14]
//   +0x42058 char          m_passwd[0x14]
//   +0x4206c char          m_db[0x14]
//   +0x42080 char          m_pad[0xa]
//   +0x4208a bool          m_bQueryCounterEnable
// 访问器（is_valid_col/get_*）为头文件 inline（ORIG 弱符号，分散实例化）。
// ============================================================================

#include <cstddef>
#include <stdlib.h>
#include <string.h>
#include <mysql.h>

class CQueryCounter;

// ---- CDnFTimer：vtable@0 = SetLastTime / vtable@4 = GetTimeInterval（无虚析构）----
class CDnFTimer
{
public:
    virtual void SetLastTime();
    virtual double GetTimeInterval();
};

class CDnFTimerFactory
{
public:
    static CDnFTimer* CreateDnFTimer();
    static void DestroyDnFTimer(CDnFTimer* timer);
};

class MySQL
{
public:
    MySQL() throw();                  // 0x083f3ac8（ORIG：nothrow，new 无 EH 清理）
    ~MySQL() throw();                 // 0x083f3aea

    bool set_compress_option();       // 0x083f3afe
    bool set_read_default_grp_option();  // 0x083f3b80
    bool set_charset_name_option();   // 0x083f3c02
    bool set_reconnect_option();      // 0x083f3c0c
    bool init_db_handle();            // 0x083f3c16
    bool init();                      // 0x083f3ce4
    void close();                     // 0x083f3e74
    bool open(char* host, char* db, char* user, char* passwd);  // 0x083f3e9e
    bool open(char* host, unsigned int port, char* db, char* user,
              char* passwd);          // 0x083f4024
    bool set_query(const char* fmt, ...);  // 0x083f41c0
    unsigned long long getAffectedRowCount();  // 0x083f4310
    bool exec(bool bLog);             // 0x083f4326
    bool fetch();                     // 0x083f44bc
    int ping();                       // 0x083f4514
    char* blob_to_str(int idx, void* data, int len);  // 0x083f452a
    const char* getServerVersion();   // 0x083f460c
    const char* getClientVersion();   // 0x083f4622
    int select();                     // 0x083f4630
    int select(const char* fmt, ...); // 0x083f4678
    int insert(const char* fmt, ...); // 0x083f46ae
    int update(const char* fmt, ...); // 0x083f4718
    bool set_query();                 // 0x083f4782
    void escape_string(char* dst, const char* src);  // 0x083f48aa

    int exec_query();                 // 0x083f5348（弱）
    void clear_result_set();          // 0x083f5630（弱）
    CQueryCounter* GetQueryCounter(); // 0x082a4202（弱）
    void SetQueryCounter(CQueryCounter* pCounter);  // 0x083f531c（弱）

    // ---- inline 访问器（ORIG 弱符号）----
    bool is_valid_col(int col);       // 0x080e22d0
    bool is_valid_data(int col);      // 0x085f41b2
    bool get_int(int col, int& out);  // 0x0811692c
    bool get_uint(int col, unsigned int& out);  // 0x080e22f2
    bool get_short(int col, short& out);        // 0x080fddec
    bool get_short(int col, int& out);          // 0x0814201c
    bool get_ushort(int col, unsigned short& out);  // 0x08116990
    bool get_byte(int col, char& out);          // 0x080fde54
    bool get_ubyte(int col, unsigned char& out);  // 0x080fdeba
    bool get_char(int col, char& out);          // 0x0815e6a6
    bool get_longlong(int col, long long& out); // 0x085f4214
    bool get_ulonglong(int col, unsigned long long& out);  // 0x081754c8
    bool get_ulong(int col, unsigned long& out);  // 0x0844d66a
    bool get_str(int col, char* buf, int size);   // 0x080ecdea
    bool get_binary(int col, void* buf, int size);  // 0x0812531a
    int get_binary_length(int col);               // 0x081253de
    int get_n_rows();                             // 0x080e236c
    int getDBError();                             // 0x0811b97a
    char* GetQueryString();                       // 0x0844d65e

private:
    CQueryCounter* m_pQueryCounter;   // +0x00
    MYSQL* m_dbHandle;                // +0x04
    MYSQL_RES* m_result;              // +0x08
    MYSQL_ROW m_row;                  // +0x0c
    unsigned long* m_lengths;         // +0x10
    int m_nRows;                      // +0x14
    int m_nFields;                    // +0x18
    char m_query[0x6001];             // +0x1c
    char m_blob[10][0x6001];          // +0x601d
    size_t m_queryLen;                // +0x42028
    int m_dbError;                    // +0x4202c
    char m_host[0x10];                // +0x42030
    unsigned int m_port;              // +0x42040
    char m_user[0x14];                // +0x42044
    char m_passwd[0x14];              // +0x42058
    char m_db[0x14];                  // +0x4206c
    char m_pad[0xa];                  // +0x42080
    bool m_bQueryCounterEnable;       // +0x4208a
};

inline bool MySQL::is_valid_col(int col)
{
    if (!(col >= 0 && col < m_nFields))
    {
        return false;
    }
    return true;
}

inline bool MySQL::is_valid_data(int col)
{
    if (m_row == 0 || !is_valid_col(col))
    {
        return false;
    }
    return m_row[col] != 0;
}

inline bool MySQL::get_int(int col, int& out)
{
    if (m_row == 0 || !is_valid_col(col))
    {
        return false;
    }
    out = atoi(m_row[col]);
    return true;
}

inline bool MySQL::get_uint(int col, unsigned int& out)
{
    if (m_row == 0 || !is_valid_col(col))
    {
        return false;
    }
    unsigned long value = strtoul(m_row[col], 0, 10);
    out = (unsigned int)value;
    return true;
}

inline bool MySQL::get_short(int col, short& out)
{
    if (m_row == 0 || !is_valid_col(col))
    {
        return false;
    }
    out = (short)atoi(m_row[col]);
    return true;
}

inline bool MySQL::get_short(int col, int& out)
{
    if (m_row == 0 || !is_valid_col(col))
    {
        return false;
    }
    out = atoi(m_row[col]);
    return true;
}

inline bool MySQL::get_ushort(int col, unsigned short& out)
{
    if (m_row == 0 || !is_valid_col(col))
    {
        return false;
    }
    out = (unsigned short)atoi(m_row[col]);
    return true;
}

inline bool MySQL::get_byte(int col, char& out)
{
    if (m_row == 0 || !is_valid_col(col))
    {
        return false;
    }
    out = (char)atoi(m_row[col]);
    return true;
}

inline bool MySQL::get_ubyte(int col, unsigned char& out)
{
    if (m_row == 0 || !is_valid_col(col))
    {
        return false;
    }
    out = (unsigned char)atoi(m_row[col]);
    return true;
}

inline bool MySQL::get_char(int col, char& out)
{
    if (m_row == 0 || !is_valid_col(col))
    {
        return false;
    }
    out = m_row[col][0];
    return true;
}

inline bool MySQL::get_longlong(int col, long long& out)
{
    if (m_row == 0 || !is_valid_col(col))
    {
        return false;
    }
    out = strtoll(m_row[col], 0, 10);
    return true;
}

inline bool MySQL::get_ulonglong(int col, unsigned long long& out)
{
    if (m_row == 0 || !is_valid_col(col))
    {
        return false;
    }
    out = strtoull(m_row[col], 0, 10);
    return true;
}

inline bool MySQL::get_ulong(int col, unsigned long& out)
{
    if (m_row == 0 || !is_valid_col(col))
    {
        return false;
    }
    out = strtoul(m_row[col], 0, 10);
    return true;
}

inline bool MySQL::get_str(int col, char* buf, int size)
{
    if (m_row == 0 || !is_valid_col(col) || size <= 0)
    {
        return false;
    }
    strncpy(buf, m_row[col], size);
    buf[size - 1] = 0;
    return true;
}

inline bool MySQL::get_binary(int col, void* buf, int size)
{
    if (m_row == 0 || !is_valid_col(col) || size <= 0)
    {
        return false;
    }
    int len = 0;
    if (m_lengths[col] < (unsigned long)size)
    {
        len = m_lengths[col];
        memset(buf, 0, size);
    }
    else
    {
        len = size;
    }
    memcpy(buf, m_row[col], len);
    return true;
}

inline int MySQL::get_binary_length(int col)
{
    return m_lengths[col];
}

inline int MySQL::get_n_rows()
{
    return m_nRows;
}

inline int MySQL::getDBError()
{
    return m_dbError;
}

inline char* MySQL::GetQueryString()
{
    return m_query;
}

inline CQueryCounter* MySQL::GetQueryCounter()
{
    return m_pQueryCounter;
}

inline void MySQL::SetQueryCounter(CQueryCounter* pCounter)
{
    m_pQueryCounter = pCounter;
    if (m_pQueryCounter != 0)
    {
        m_bQueryCounterEnable = true;
    }
}

#endif  // DNF_GAME_MYSQL_H_
