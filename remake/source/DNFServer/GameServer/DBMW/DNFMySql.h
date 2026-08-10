#ifndef DBMW_DNFMYSQL_H_
#define DBMW_DNFMYSQL_H_

#include "DBMWCommon.h"


class CDBHandle
{
public:
    CDBHandle();
    virtual ~CDBHandle();
    // dbmw 虚表顺序（vptr+0x08 起，共 31 个虚函数）
    virtual char init() { return 0; }
    virtual char open(const char* host, const char* user, const char* pass, const char* db) { return 0; }
    virtual char open(const char* host, unsigned int port, const char* user, const char* pass, const char* db) { return 0; }
    virtual void close() {}
    virtual char ping() { return 0; }
    virtual bool set_query(unsigned int q, char* fmt, ...) { return 0; }
    virtual bool exec(unsigned int q) { return 0; }
    virtual bool fetch() { return 0; }
    virtual char* blob_to_str(int col, void* buf, int len) { return 0; }
    virtual bool get_str(int col, char* buf, int len) { return 0; }
    virtual bool get_binary(int col, void* buf, int len) { return 0; }
    virtual bool get_int(int col, int& v) { return 0; }
    virtual bool get_uint(int col, unsigned int& v) { return 0; }
    virtual bool get_short(int col, int& v) { return 0; }
    virtual bool get_short(int col, short& v) { return 0; }
    virtual bool get_ushort(int col, int& v) { return 0; }
    virtual bool get_ushort(int col, unsigned short& v) { return 0; }
    virtual bool get_byte(int col, int& v) { return 0; }
    virtual bool get_byte(int col, char& v) { return 0; }
    virtual bool get_ubyte(int col, int& v) { return 0; }
    virtual bool get_ubyte(int col, unsigned char& v) { return 0; }
    virtual bool get_int(int col, unsigned int& v) { return 0; }
    virtual bool get_int(int col, unsigned long long& v) { return 0; }
    virtual bool get_uint(int col, unsigned long long& v) { return 0; }
    virtual int get_n_fields() { return 0; }
    virtual int get_n_rows() { return 0; }
    virtual void clear_result_set() {}
    virtual unsigned long long getAffectedRowCount() { return 0; }
    virtual char* escape_string(char* dst, char const* src) { return 0; }
    virtual const char* get_quest_str() const { return 0; }
    // 以下为原版非虚成员
    char init_db_handle();
    char set_compress_option();
    char set_read_default_grp_option();
    char is_valid_col(int col);
    int exec_query();
};

class CMySql : public CDBHandle
{
public:
    CMySql();
    virtual ~CMySql();
    virtual char init();
    char open(const char* host, const char* user, const char* pass, const char* db);
    char open(const char* host, unsigned int port, const char* user, const char* pass, const char* db);
    void close();
    char init_db_handle();  // 非虚
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
    char is_valid_col(int col); // 非虚
    char set_compress_option(); // 非虚
    char set_read_default_grp_option(); // 非虚
    char set_charset_name_option(); // 非虚
    char set_reconnect_option();    // 非虚
    char ping();        // 虚（虚表 vptr+0x14）
    // dbmw 布局（init/open/exec_query 反推）
    MYSQL* m_mysql;   // +4
    MYSQL_RES* m_result;  // +8
    MYSQL_ROW m_row;      // +0xc
    unsigned long* m_lengths;  // +0x10
    int m_nRows;      // +0x14
    int m_nFields;    // +0x18
    char m_host[0x10];  // +0x1c
    char m_pass[0x14];  // +0x2c
    char m_db[0x14];    // +0x40
    char m_user[0x1e];  // +0x54
    unsigned int m_port;  // +0x74
    char m_query[0x6001];  // +0x78（memset 0x6001）
    char m_blob[10][0x6001];  // +0x6079（blob_to_str 基址 this+0x6079+col*0x6001，10×0x6001=0x3c00a 至 0x42083）
    unsigned int m_queryLen;  // +0x42084
    unsigned int m_lastErrno; // +0x42088（sizeof=0x4208c，与 ORIG 分配一致）
};

#endif  // DBMW_DNFMYSQL_H_
