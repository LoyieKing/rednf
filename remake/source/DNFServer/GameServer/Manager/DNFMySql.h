#ifndef DNF_MYSQL_H_
#define DNF_MYSQL_H_

#include <mysql.h>

// ---- CDBHandle / CMySql ----
class CDBHandle
{
public:
    CDBHandle();
    virtual ~CDBHandle();
    // 虚表顺序与原版完全一致（vptr+0x08 起）
    virtual bool init() { return 0; }
    virtual bool open(const char* host, const char* user, const char* pass, const char* db) { return 0; }
    virtual void close() {}
    virtual int ping() { return 0; }
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
    virtual int get_n_fields() { return 0; }
    virtual int get_n_rows() { return 0; }
    virtual void clear_result_set() {}
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
    int get_n_rows();   // 虚（虚表 vptr+0x5c）
    int get_n_fields(); // 虚（虚表 vptr+0x58）
    bool is_valid_col(int col); // 非虚
    bool set_compress_option(); // 非虚
    bool set_read_default_grp_option(); // 非虚
    int ping();         // 虚（虚表 vptr+0x14，ORIG 返回 int：无 test/setne）
    MYSQL* m_mysql;   // +4
    MYSQL_RES* m_result;  // +8
    MYSQL_ROW m_row;      // +0xc
    unsigned long* m_lengths;  // +0x10
    int m_nRows;      // +0x14
    int m_nFields;    // +0x18
    char m_query[0x1001];  // +0x1c（memset 0x1001 证实）
    char m_blob[10][0x1001];  // +0x101d（blob 索引按 0x1001 步长）
    unsigned int m_queryLen;  // +0xb028
    unsigned int m_lastErrno; // +0xb02c
};

#endif  // DNF_MYSQL_H_
