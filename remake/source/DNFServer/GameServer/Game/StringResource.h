// df_game_r 还原 —— StringResource / app::stringResource / CStringManager
//
// ORIG 证据（df_game_r elf32-i386）：
//   class StringResource（std::map<int,CharString> 包装）
//     C1/D1                      T 0x08ad47a8 / 0x08ad47bc（仅构造/析构 map 成员）
//     add(int, CharString const&)  T 0x08ad47d0（map.insert，返回是否新增）
//     get(int, CharString const**) const T 0x08ad4878（find；命中 *out=&second 返回 1）
//     set(int, CharString const&)  T 0x08ad48e6
//     loadFromFile(char const*, CharString*)  T 0x08ad4ce4（fopen "rb"+ftell+fread
//                                              → loadFromBinary；打不开写
//                                              "Cannot open file" 到 err）
//     loadFromBinary(char*, int, CharString*) T 0x08ad4e04（SC01 加密头解密 /
//                                              UTF-8 BOM / UTF-16LE BOM /
//                                              文本四分支 → loadFromString）
//     loadFromString(char*, int, CharString*) T 0x08ad4ac2（逐行 parse：
//                                              "//" 注释、"<key> > <value>"，
//                                              \n 转义，duplicate 记入 err）
//   app::stringResource()        T 0x08ad5439（guard 单例 + __cxa_atexit 析构注册）
//   loadString(CharString*)      T 0x08ac35cc（=stringResource().loadFromFile(
//                                  g_STRING_FILE, err)；文件路径存全局 0x093f567c
//                                  _ZL11STRING_FILE）
//   class CStringManager
//     getStr(int)                T 0x08ac35f8（惰性加载字符串资源表；命中返回
//                                  CharString::c_str()；未命中 sprintf
//                                  "getStr(%d) == none" 进静态 errorMsg 缓冲；
//                                  加载失败返回 0）
//     setStringResourceLoaded()  T 0x08ac36d2（置全局 _ZL12is_sr_loaded=1，
//                                  地址 0x0950189a）
//
// 依赖关系：本 TU 补齐 convertFromUtf8 / convertToCharString 编码原语
//   _Z15convertFromUtf8PKciP10WideString     T 0x08adf893
//   _Z19convertToCharStringPKw               T 0x08adf54c（内部转 PKwj）
//   _Z19convertToCharStringPKwj              T 0x08adf57f（wcstombs 定长求值
//                                 → CharStringData 缓冲）
//   _Z19convertToCharStringRK10WideString    T 0x08adf624（sret，转发 PKw）

#ifndef DNF_GAME_STRINGRESOURCE_H_
#define DNF_GAME_STRINGRESOURCE_H_

#include <map>

#include "StringTypes.h"   // CharString / WideString 完整定义

class StringResource
{
public:
    StringResource();
    ~StringResource();

    bool add(int id, const CharString& value);        // ORIG T 0x08ad47d0
    void set(int id, const CharString& value);        // ORIG T 0x08ad48e6
    bool get(int id, const CharString** out) const;   // ORIG T 0x08ad4878

    bool loadFromFile(const char* path, CharString* error);          // ORIG T 0x08ad4ce4
    bool loadFromBinary(const unsigned char* data, int size, CharString* error);  // ORIG T 0x08ad4e04
    bool loadFromResource(void* data, int size, CharString* error);  // ORIG T 0x08ad4dfa
    bool loadFromString(char* text, int length, CharString* error);  // ORIG T 0x08ad4ac2

private:
    std::map<int, CharString> m_strings;   // map<int,CharString> @+0x00
};

namespace app {

// ORIG T 0x08ad5439：进程级唯一 StringResource 实例（guard 单例）。
StringResource* stringResource();
// ORIG T 0x08ad54cc：get(id) 命中返回表项拷贝；未命中
// CharString::format("getStr(%d) == none", id)（定义 StringResource.cpp）。
CharString getStr(int id);

}  // namespace app

// ORIG T 0x08ac35cc：app::stringResource()->loadFromFile(g_STRING_FILE, err)。
bool loadString(CharString* error);

// ORIG 中为编译单元内静态指针 _ZL11STRING_FILE（0x093f567c），恒 0（无写入方）；
// 重建版以同 TU 内 static STRING_FILE 复现（fopen(NULL)="rb" 失败
// → loadString false → getStr 回退 errmsg 路径）。
// 该路径无外部设置方（全仓库无引用），loadString 直读本 TU static。

class CStringManager
{
public:
    static char* getStr(int id);                     // ORIG T 0x08ac35f8
    static void setStringResourceLoaded();           // ORIG T 0x08ac36d2
};

// 编码原语（见文件头地址证据）
bool convertFromUtf8(const char* src, int srcLength, WideString* out);  // ORIG T 0x08adf893
CharString convertToCharString(const wchar_t* src);                     // ORIG T 0x08adf54c
CharString convertToCharString(const wchar_t* src, unsigned int srcLength);  // ORIG T 0x08adf57f（PKwj）
CharString convertToCharString(const WideString& src);                  // ORIG T 0x08adf624

#endif  // DNF_GAME_STRINGRESOURCE_H_
