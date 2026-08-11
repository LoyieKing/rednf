#ifndef DB_MANAGER_H_
#define DB_MANAGER_H_

class CApplication;
class CDBHandle;

enum ENUM_DB_HANDLE_IDX { DB_HANDLE_0 = 0 };
enum ENUM_DB_KIND { DB_KIND_0 = 0 };

// ---- CDBManager ----
class CDBManager
{
public:
    CDBManager();
    ~CDBManager();
    void Open(ENUM_DB_HANDLE_IDX idx, const char* host, const char* user, const char* pass, const char* db);
    void Init(ENUM_DB_KIND kind, CApplication* app);
    void Close();
    CDBHandle* GetDBHandle(ENUM_DB_HANDLE_IDX idx);
    bool UpdateQueryCount(unsigned int idx, int count, int time);  // ORIG 返回 bool（调用方 xor $1 取反）
    char SelectTest();
    CDBHandle* m_handles[0x10];  // +0
    char m_pad40[4];        // +0x40
    CApplication* m_app;    // +0x44
};

#endif  // DB_MANAGER_H_
