#ifndef DBMW_DNFMYSQL_H_
#define DBMW_DNFMYSQL_H_

#include "DBMWCommon.h"


class CDBHandle
{
public:
    CDBHandle();
    virtual ~CDBHandle();
    // ORIG vtable (_ZTV9CDBHandle = 0x88, slot2.. 全 __cxa_pure_virtual) ⇒ 本类是纯接口，
    // 实现全在派生类 CMySql（其 vtable 与 ORIG 逐槽一致）。原空壳 { return 0; } 违反
    // 「禁止空壳充数」，按 ORIG 证据改回纯虚。
    virtual bool init() = 0;
    virtual bool open(const char* host, const char* user, const char* pass, const char* db) = 0;
    virtual bool open(const char* host, unsigned int port, const char* user, const char* pass, const char* db) = 0;
    virtual void close() = 0;
    virtual char ping() = 0;
    virtual bool set_query(unsigned int q, char* fmt, ...) = 0;
    virtual bool exec(unsigned int q) = 0;
    virtual bool fetch() = 0;
    virtual char* blob_to_str(int col, void* buf, int len) = 0;
    virtual bool get_str(int col, char* buf, int len) = 0;
    virtual bool get_binary(int col, void* buf, int len) = 0;
    virtual bool get_int(int col, int& v) = 0;
    virtual bool get_uint(int col, unsigned int& v) = 0;
    virtual bool get_short(int col, int& v) = 0;
    virtual bool get_short(int col, short& v) = 0;
    virtual bool get_ushort(int col, int& v) = 0;
    virtual bool get_ushort(int col, unsigned short& v) = 0;
    virtual bool get_byte(int col, int& v) = 0;
    virtual bool get_byte(int col, char& v) = 0;
    virtual bool get_ubyte(int col, int& v) = 0;
    virtual bool get_ubyte(int col, unsigned char& v) = 0;
    virtual bool get_int(int col, unsigned int& v) = 0;
    virtual bool get_int(int col, unsigned long long& v) = 0;
    virtual bool get_uint(int col, unsigned long long& v) = 0;
    virtual int get_n_fields() = 0;
    virtual int get_n_rows() = 0;
    virtual void clear_result_set() = 0;
    virtual unsigned long long getAffectedRowCount() = 0;
    virtual char* escape_string(char* dst, char const* src) = 0;
    virtual const char* get_quest_str() const = 0;
    // 以下为原版非虚成员
    bool init_db_handle();
    bool set_compress_option();
    bool set_read_default_grp_option();
    bool is_valid_col(int col);
    int exec_query();
};

class CMySql : public CDBHandle
{
public:
    CMySql();
    virtual ~CMySql();
    virtual bool init();
    bool open(const char* host, const char* user, const char* pass, const char* db);
    bool open(const char* host, unsigned int port, const char* user, const char* pass, const char* db);
    void close();
    bool init_db_handle();  // 非虚
    int exec_query();       // 非虚
    bool set_query(unsigned int q, char* fmt, ...);
    bool exec(unsigned int q);
    bool fetch();
    void clear_result_set();
    char* blob_to_str(int col, void* buf, int len);
    bool get_str(int col, char* buf, int len);
    bool get_binary(int col, void* buf, int len);
    bool get_int(int col, int& v);
    bool get_uint(int col, unsigned int& v);
    bool get_short(int col, int& v);
    bool get_short(int col, short& v);
    bool get_ushort(int col, int& v);
    bool get_ushort(int col, unsigned short& v);
    bool get_byte(int col, int& v);
    bool get_byte(int col, char& v);
    bool get_ubyte(int col, int& v);
    bool get_ubyte(int col, unsigned char& v);
    bool get_int(int col, unsigned int& v);
    bool get_int(int col, unsigned long long& v);
    bool get_uint(int col, unsigned long long& v);
    const char* get_quest_str() const;  // 虚（虚表 vptr+0x7c，返回 m_query）
    int get_n_rows();   // 虚（虚表 vptr+0x5c）
    int get_n_fields(); // 虚（虚表 vptr+0x58）
    unsigned long long getAffectedRowCount();
    char* escape_string(char* dst, char const* src);
    bool get_ulonglong(int col, unsigned long long& v);
    bool is_valid_col(int col); // 非虚
    bool set_compress_option(); // 非虚
    bool set_read_default_grp_option(); // 非虚
    bool set_charset_name_option(); // 非虚
    bool set_reconnect_option();    // 非虚
    char ping();        // 虚（虚表 vptr+0x14）
    // dbmw 布局（init/open/exec_query 反推）
    MYSQL* m_mysql;   // +4
    MYSQL_RES* m_result;  // +8
    MYSQL_ROW m_row;      // +0xc
    unsigned long* m_lengths;  // +0x10
    int m_nRows;      // +0x14（ORIG store 为 RHS+edx 中转 ⇒ 8 字节返回值转 int，保持 int）
    unsigned int m_nFields;  // +0x18（ORIG exec 0x808d7xx 对 +0x18 的 store 为 LHS-first 直存
                             // ⇒ 与 mysql_num_fields() 的 unsigned int 同型；is_valid_col 的
                             // 有符号比较由比较点 (int) 显式转换取得）
    char m_host[0x10];  // +0x1c
    char m_pass[0x14];  // +0x2c
    char m_db[0x14];    // +0x40
    char m_user[0x1e];  // +0x54
    unsigned int m_port;  // +0x74
    char m_query[0x6001];  // +0x78（memset 0x6001）
    char m_blob[10][0x6001];  // +0x6079（blob_to_str 基址 this+0x6079+col*0x6001，10×0x6001=0x3c00a 至 0x42083）
    int m_queryLen;  // +0x42084（ORIG int 形态：赋值无转换，LHS 先求址）
    int m_lastErrno; // +0x42088（sizeof=0x4208c，与 ORIG 分配一致；ORIG 该 store 为
                     // RHS-first + `mov %eax,%edx` 中转 ⇒ 与 mysql_errno() 的 unsigned int
                     // 异型，按 P342 判定原字段为 int）
};

#endif  // DBMW_DNFMYSQL_H_
