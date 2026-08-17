// df_game_r 还原 —— CharString / WideString 实现（桩）
#include "StringTypes.h"
#include <stdio.h>
#include <string.h>
#include <string>
#include <wchar.h>

// ---- CharString ----

CharString::CharString() : m_data(0)
{
    m_data = new CharStringData();
    m_data->m_data = new char[1];
    m_data->m_data[0] = 0;
    m_data->m_length = 0;
}

CharString::CharString(const char* str) : m_data(0)
{
    m_data = new CharStringData();
    int len = str ? (int)strlen(str) : 0;
    m_data->m_data = new char[len + 1];
    if (str) memcpy(m_data->m_data, str, len);
    m_data->m_data[len] = 0;
    m_data->m_length = len;
}

CharString::CharString(const CharString& other) : m_data(other.m_data)
{
    if (m_data) m_data->m_refCount++;
}

CharString::~CharString()
{
    if (m_data)
    {
        m_data->m_refCount--;
        if (m_data->m_refCount <= 0)
            delete m_data;
    }
}

CharString& CharString::operator=(const CharString& other)
{
    if (this != &other)
    {
        if (m_data)
        {
            m_data->m_refCount--;
            if (m_data->m_refCount <= 0)
                delete m_data;
        }
        m_data = other.m_data;
        if (m_data) m_data->m_refCount++;
    }
    return *this;
}

CharString& CharString::operator=(const char* str)
{
    assign(str);
    return *this;
}

void CharString::attachData(CharStringData* data)
{
    if (m_data)
    {
        m_data->m_refCount--;
        if (m_data->m_refCount <= 0)
            delete m_data;
    }
    m_data = data;
    if (m_data) m_data->m_refCount++;
}

void CharString::replaceData(CharStringData* data)
{
    attachData(data);
}

void CharString::clear()
{
    if (m_data)
    {
        delete[] m_data->m_data;
        m_data->m_data = new char[1];
        m_data->m_data[0] = 0;
        m_data->m_length = 0;
    }
}

void CharString::assign(const char* str)
{
    if (m_data)
    {
        delete[] m_data->m_data;
        int len = str ? (int)strlen(str) : 0;
        m_data->m_data = new char[len + 1];
        if (str) memcpy(m_data->m_data, str, len);
        m_data->m_data[len] = 0;
        m_data->m_length = len;
    }
}

void CharString::assign(const char* str, int len)
{
    if (m_data)
    {
        delete[] m_data->m_data;
        m_data->m_data = new char[len + 1];
        if (str) memcpy(m_data->m_data, str, len);
        m_data->m_data[len] = 0;
        m_data->m_length = len;
    }
}

void CharString::assign(const CharString& other)
{
    assign(other.c_str(), other.length());
}

void CharString::assign(char c)
{
    char buf[2] = {c, 0};
    assign(buf);
}

void CharString::concat(const char* str, const CharString& other)
{
    int len1 = str ? (int)strlen(str) : 0;
    int len2 = other.length();
    int total = len1 + len2;
    char* buf = new char[total + 1];
    if (str) memcpy(buf, str, len1);
    memcpy(buf + len1, other.c_str(), len2);
    buf[total] = 0;
    assign(buf, total);
    delete[] buf;
}

void CharString::concat(const char* str, const CharString& other, int)
{
    concat(str, other);
}

void CharString::concat(const char* str, int, const CharString& other, int)
{
    concat(str, other);
}

void CharString::concat(const CharString& other, const char* str)
{
    concat(other.c_str(), CharString(str));
}

void CharString::concat(const CharString& other, const char* str, int)
{
    concat(other, str);
}

void CharString::concat(const CharString& other, const CharString& other2)
{
    concat(other.c_str(), other2);
}

void CharString::concat(const CharString& other, char c)
{
    char buf[2] = {c, 0};
    concat(other, buf);
}

void CharString::concat(char c, const CharString& other)
{
    char buf[2] = {c, 0};
    concat(buf, other);
}

int CharString::compare(const char* str, const CharString& other) const
{
    return strcmp(str ? str : "", other.c_str());
}

int CharString::compare(const CharString& other, const char* str) const
{
    return strcmp(other.c_str(), str ? str : "");
}

int CharString::compare(const CharString& other, const CharString& other2) const
{
    return strcmp(other.c_str(), other2.c_str());
}

int CharString::compareNoCase(const char* str, const CharString& other) const
{
    return strcasecmp(str ? str : "", other.c_str());
}

int CharString::compareNoCase(const CharString& other, const char* str) const
{
    return strcasecmp(other.c_str(), str ? str : "");
}

int CharString::compareNoCase(const CharString& other, const CharString& other2) const
{
    return strcasecmp(other.c_str(), other2.c_str());
}

void CharString::setAt(int index, char c)
{
    if (m_data && index >= 0 && index < m_data->m_length)
        m_data->m_data[index] = c;
}

int CharString::pattern(const char* str, int)
{
    return 0;
}

void CharString::format(const char* fmt, ...)
{
    char buf[4096];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);
    assign(buf);
}

void CharString::vformat(const char* fmt, va_list args)
{
    char buf[4096];
    vsnprintf(buf, sizeof(buf), fmt, args);
    assign(buf);
}

void CharString::tokenize(const CharString& str, std::vector<CharString>& tokens)
{
    tokenize(str, tokens, " \t", 0, false);
}

void CharString::tokenize(const CharString& str, std::vector<CharString>& tokens,
                          const char* delimiters, const char*, bool)
{
    tokens.clear();
    const char* p = str.c_str();
    const char* start = p;
    while (*p)
    {
        if (strchr(delimiters, *p))
        {
            if (p > start)
                tokens.push_back(CharString(std::string(start, p - start).c_str()));
            start = p + 1;
        }
        p++;
    }
    if (p > start)
        tokens.push_back(CharString(std::string(start, p - start).c_str()));
}

void CharString::tokenizeNewLine(const CharString& str, std::vector<CharString>& tokens)
{
    tokenize(str, tokens, "\n", 0, false);
}

void CharString::join(const std::vector<CharString>& items, const CharString& separator)
{
    clear();
    for (size_t i = 0; i < items.size(); ++i)
    {
        if (i > 0) concat(*this, separator);
        concat(*this, items[i]);
    }
}

// ---- WideString ----

WideString::WideString() : m_data(0)
{
    m_data = new WideStringData();
    m_data->m_data = new wchar_t[1];
    m_data->m_data[0] = 0;
    m_data->m_length = 0;
}

WideString::WideString(const wchar_t* str) : m_data(0)
{
    m_data = new WideStringData();
    int len = str ? (int)wcslen(str) : 0;
    m_data->m_data = new wchar_t[len + 1];
    if (str) wmemcpy(m_data->m_data, str, len);
    m_data->m_data[len] = 0;
    m_data->m_length = len;
}

WideString::WideString(const WideString& other) : m_data(other.m_data)
{
    if (m_data) m_data->m_refCount++;
}

WideString::~WideString()
{
    if (m_data)
    {
        m_data->m_refCount--;
        if (m_data->m_refCount <= 0)
            delete m_data;
    }
}

WideString& WideString::operator=(const WideString& other)
{
    if (this != &other)
    {
        if (m_data)
        {
            m_data->m_refCount--;
            if (m_data->m_refCount <= 0)
                delete m_data;
        }
        m_data = other.m_data;
        if (m_data) m_data->m_refCount++;
    }
    return *this;
}

WideString& WideString::operator=(const wchar_t* str)
{
    assign(str);
    return *this;
}

void WideString::attachData(WideStringData* data)
{
    if (m_data)
    {
        m_data->m_refCount--;
        if (m_data->m_refCount <= 0)
            delete m_data;
    }
    m_data = data;
    if (m_data) m_data->m_refCount++;
}

void WideString::replaceData(WideStringData* data)
{
    attachData(data);
}

void WideString::clear()
{
    if (m_data)
    {
        delete[] m_data->m_data;
        m_data->m_data = new wchar_t[1];
        m_data->m_data[0] = 0;
        m_data->m_length = 0;
    }
}

void WideString::assign(const wchar_t* str)
{
    if (m_data)
    {
        delete[] m_data->m_data;
        int len = str ? (int)wcslen(str) : 0;
        m_data->m_data = new wchar_t[len + 1];
        if (str) wmemcpy(m_data->m_data, str, len);
        m_data->m_data[len] = 0;
        m_data->m_length = len;
    }
}

void WideString::assign(const wchar_t* str, int len)
{
    if (m_data)
    {
        delete[] m_data->m_data;
        m_data->m_data = new wchar_t[len + 1];
        if (str) wmemcpy(m_data->m_data, str, len);
        m_data->m_data[len] = 0;
        m_data->m_length = len;
    }
}

void WideString::assign(const WideString& other)
{
    assign(other.c_str(), other.length());
}

void WideString::concat(const wchar_t* str, const WideString& other)
{
    int len1 = str ? (int)wcslen(str) : 0;
    int len2 = other.length();
    int total = len1 + len2;
    wchar_t* buf = new wchar_t[total + 1];
    if (str) wmemcpy(buf, str, len1);
    wmemcpy(buf + len1, other.c_str(), len2);
    buf[total] = 0;
    assign(buf, total);
    delete[] buf;
}

void WideString::concat(const WideString& other, const wchar_t* str)
{
    concat(other.c_str(), WideString(str));
}

void WideString::concat(const WideString& other, const WideString& other2)
{
    concat(other.c_str(), other2);
}

int WideString::compare(const wchar_t* str, const WideString& other) const
{
    return wcscmp(str ? str : L"", other.c_str());
}

int WideString::compare(const WideString& other, const wchar_t* str) const
{
    return wcscmp(other.c_str(), str ? str : L"");
}

int WideString::compare(const WideString& other, const WideString& other2) const
{
    return wcscmp(other.c_str(), other2.c_str());
}

void WideString::format(const wchar_t* fmt, ...)
{
    wchar_t buf[4096];
    va_list args;
    va_start(args, fmt);
    vswprintf(buf, sizeof(buf) / sizeof(wchar_t), fmt, args);
    va_end(args);
    assign(buf);
}

// ---- StringUnorderedMap explicit instantiations ----
#include "HashMap.h"

template class StringUnorderedMap<CharStringHashTrait, std::string>;
template class StringUnorderedMap<CharStringHashTrait, float>;
template class StringUnorderedMap<CharStringHashTrait, int>;
