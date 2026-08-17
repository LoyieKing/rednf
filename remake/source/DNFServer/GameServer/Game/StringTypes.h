#ifndef DNF_GAME_STRING_TYPES_H_
#define DNF_GAME_STRING_TYPES_H_

// ============================================================================
// df_game_r 还原 —— CharString / WideString（字符串类）
// ORIG 使用共享数据 + 引用计数模式；当前用简单 char*/wchar_t* 实现，
// 后续按 ORIG 反汇编细化。
// ============================================================================

#include <string.h>
#include <stdarg.h>
#include <vector>

class CharStringData
{
public:
    CharStringData() : m_refCount(1), m_length(0), m_data(0) {}
    ~CharStringData() { delete[] m_data; }

    int m_refCount;
    int m_length;
    char* m_data;
};

class WideStringData
{
public:
    WideStringData() : m_refCount(1), m_length(0), m_data(0) {}
    ~WideStringData() { delete[] m_data; }

    int m_refCount;
    int m_length;
    wchar_t* m_data;
};

class CharString
{
public:
    CharString();
    CharString(const char* str);
    CharString(const CharString& other);
    ~CharString();

    CharString& operator=(const CharString& other);
    CharString& operator=(const char* str);

    void attachData(CharStringData* data);
    void replaceData(CharStringData* data);

    void clear();
    void assign(const char* str);
    void assign(const char* str, int len);
    void assign(const CharString& other);
    void assign(char c);

    void concat(const char* str, const CharString& other);
    void concat(const char* str, const CharString& other, int);
    void concat(const char* str, int, const CharString& other, int);
    void concat(const CharString& other, const char* str);
    void concat(const CharString& other, const char* str, int);
    void concat(const CharString& other, const CharString& other2);
    void concat(const CharString& other, char c);
    void concat(char c, const CharString& other);

    int compare(const char* str, const CharString& other) const;
    int compare(const CharString& other, const char* str) const;
    int compare(const CharString& other, const CharString& other2) const;

    int compareNoCase(const char* str, const CharString& other) const;
    int compareNoCase(const CharString& other, const char* str) const;
    int compareNoCase(const CharString& other, const CharString& other2) const;

    void setAt(int index, char c);
    int pattern(const char* str, int);

    void format(const char* fmt, ...);
    void vformat(const char* fmt, va_list args);

    void tokenize(const CharString& str, std::vector<CharString>& tokens);
    void tokenize(const CharString& str, std::vector<CharString>& tokens,
                  const char* delimiters, const char*, bool);
    void tokenizeNewLine(const CharString& str, std::vector<CharString>& tokens);

    void join(const std::vector<CharString>& items, const CharString& separator);

    const char* c_str() const { return m_data ? m_data->m_data : ""; }
    int length() const { return m_data ? m_data->m_length : 0; }

private:
    CharStringData* m_data;
};

class WideString
{
public:
    WideString();
    WideString(const wchar_t* str);
    WideString(const WideString& other);
    ~WideString();

    WideString& operator=(const WideString& other);
    WideString& operator=(const wchar_t* str);

    void attachData(WideStringData* data);
    void replaceData(WideStringData* data);

    void clear();
    void assign(const wchar_t* str);
    void assign(const wchar_t* str, int len);
    void assign(const WideString& other);

    void concat(const wchar_t* str, const WideString& other);
    void concat(const WideString& other, const wchar_t* str);
    void concat(const WideString& other, const WideString& other2);

    int compare(const wchar_t* str, const WideString& other) const;
    int compare(const WideString& other, const wchar_t* str) const;
    int compare(const WideString& other, const WideString& other2) const;

    void format(const wchar_t* fmt, ...);

    const wchar_t* c_str() const { return m_data ? m_data->m_data : L""; }
    int length() const { return m_data ? m_data->m_length : 0; }

private:
    WideStringData* m_data;
};

#endif  // DNF_GAME_STRING_TYPES_H_
