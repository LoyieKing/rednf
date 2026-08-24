#ifndef GAME_RDARSCRIPTSTRINGMANAGER_H_
#define GAME_RDARSCRIPTSTRINGMANAGER_H_

#include <istream>
#include <map>
#include <string>
#include "StringDictionary.h"
#include <wchar.h>

// ORIG 布局（nm W _ZN8ErrorLogC1Ev 0x88c424c / D1 0x88c4298 / aSERKS_ 0x88c5192）：
//   +0x00 int m_code（如 0x80000003）/ +0x04 std::string m_message。总大小 8。

// ============================================================================
class ErrorLog
{
public:
    ErrorLog() : m_code(0), m_message() {}
    ErrorLog(const ErrorLog& other)
        : m_code(other.m_code), m_message(other.m_message) {}
    ~ErrorLog() {}
    ErrorLog& operator=(const ErrorLog& other)
    {
        m_code = other.m_code;
        m_message = other.m_message;
        return *this;
    }

    int m_code;             // +0x00
    std::string m_message;  // +0x04
};

class RDARScriptStringManager
{
public:
    // ORIG C1 0x8aa4e98：STScriptFileList at +0 (map 0x18), vector* at +0x18,
    // wstring at +0x24. getStringFileList returns this.
    std::map<int, std::string> fileList_;
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
    void changeSpacilTagStr(std::string& s);
    void release();
};

extern RDARScriptStringManager g_scriptStringManager_;

#endif  // GAME_RDARSCRIPTSTRINGMANAGER_H_
