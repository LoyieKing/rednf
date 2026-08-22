#ifndef SCRIPTSTRINGMANAGER_H
#define SCRIPTSTRINGMANAGER_H

#include <map>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// ScriptStringManager（ORIG 0x8acf110 构造，0x28 字节）
// 权威声明点：ScriptStringManager.cpp 实现；DNFLexWrapper/CompiledDNFLex 经
// 本头调用。
//   +0x00 vptr（虚析构；D0 槽 = deleting）
//   +0x04 strings_    std::vector<std::string>
//   +0x10 string_map_ std::map<std::string,int>
// ---------------------------------------------------------------------------
class ScriptStringManager
{
public:
    ScriptStringManager();
    virtual ~ScriptStringManager();

    bool importFromBuffer(const unsigned char* buffer, int buffSize);
    bool importFromFile(const char* fileName);
    void clear();
    int exportToBuffer(unsigned char* buffer, int buffSize) const;
    bool exportToFile(const char* fileName) const;
    bool getString(int index, std::string* outputString) const;
    const std::string* getString(int index) const;
    int insertString(const char* string);

private:
    std::vector<std::string> strings_;      // +0x04
    std::map<std::string, int> string_map_; // +0x10
};

#endif  // SCRIPTSTRINGMANAGER_H
