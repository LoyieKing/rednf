#ifndef GAME_RDARSCRIPTSTRINGMANAGER_H_
#define GAME_RDARSCRIPTSTRINGMANAGER_H_

#include <istream>
#include <map>
#include <string>
#include "StringDictionary.h"
#include <wchar.h>
#include "NpcScript.h"          // STScriptFileList（+0 布局 = map<int,string>，与 ORIG 成员一致）

// ORIG 布局（objdump 逐段实测）：C1 0x88c424c W 仅构造两个 string（+0x04/+0x08，
//   各 _ZNSsC1Ev）；aSERKS_ 0x88c5192 W 依次拷贝 *(int*)other→this、SsaSERKSs ×2、
//   other+0xc int→this+0xc；D1 0x88c4298 W 仅逆序析构两 string。
//   ⇒ 4 字段：+0x00 int / +0x04 string / +0x08 string / +0x0c int，sizeof 0x10。

// ============================================================================
class ErrorLog
{
public:
    ErrorLog() {}                       // 两 string 默认构造；两 int 保持未初始化（ORIG 如此）
    ErrorLog(const ErrorLog& other)
        : m_code(other.m_code), m_message(other.m_message),
          m_detail(other.m_detail), m_flag(other.m_flag) {}
    ErrorLog& operator=(const ErrorLog& other)
    {
        m_code = other.m_code;
        m_message = other.m_message;
        m_detail = other.m_detail;
        m_flag = other.m_flag;
        return *this;
    }

    int         m_code;      // +0x00 错误码（如 0x80000003）
    std::string m_message;   // +0x04 主消息（文件名等）
    std::string m_detail;    // +0x08 追加信息【推断命名】
    int         m_flag;      // +0x0c 辅助整型【推断命名】
};                           // sizeof 0x10

class RDARScriptStringManager
{
public:
    // ORIG C1 0x8aa4e98：STScriptFileList at +0 (map 0x18), vector* at +0x18,
    // wstring at +0x24. getStringFileList returns this.
    STScriptFileList fileList_;
    std::vector<StringDictionary*> dictionaries_;
    std::wstring tag_;
    RDARScriptStringManager();
    ~RDARScriptStringManager();
    const char* findString(int table, const char* key, bool* flag) const;
    const char* findString(int table, const wchar_t* key, bool* flag) const;
    int getKeyIndex(int table, const wchar_t* key) const;
    void getTagByName(std::vector<std::wstring>* a, std::vector<std::wstring>* b,
                      int table, const wchar_t* name) const;
    const wchar_t* getTagByName(int table, const wchar_t* a, const wchar_t* b, bool flag) const;
    void setTableValue(int table, int key, const char* value);
    bool loadStringFile(std::istream* in, const char* path, int table, std::vector<ErrorLog>* err);
    void writeStringFile(int table);
    int getTagSizeInFile(int table);
    const char* findChangedString(int table, const char* key, bool* flag);
    void* getStringFileList();
    bool changeSpacilTagStr(std::string& s);   // ORIG 0x8aa60b0 返回 bool（成功替换=1）
    void release();
};

extern RDARScriptStringManager g_scriptStringManager_;

#endif  // GAME_RDARSCRIPTSTRINGMANAGER_H_
