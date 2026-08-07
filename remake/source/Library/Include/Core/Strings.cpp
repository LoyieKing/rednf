// Rebuilt from df_auction_r (DWARF + Ghidra decompile), 2026-08-08
#include "Strings.h"

#include <assert.h>
#include <ctype.h>

static char s_emptyCharStringDataRaw[12] = {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0};
static CharStringData* s_emptyCharStringData = (CharStringData*)s_emptyCharStringDataRaw;
static char s_emptyWideStringDataRaw[16] = {1, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static WideStringData* s_emptyWideStringData = (WideStringData*)s_emptyWideStringDataRaw;

void StringData::incRef()
{
    __sync_fetch_and_add(&refCount_, 1);
}

void StringData::decRef()
{
    if (refCount_ <= 0)
    {
        __assert_fail("refCount_ > 0", "../../Include/Core/Strings.cpp", 0x23,
                      "void StringData::decRef()");
    }
    if (__sync_add_and_fetch(&refCount_, -1) <= 0)
    {
        if (size_ < 1)
        {
            __assert_fail("size_ > 0", "../../Include/Core/Strings.cpp", 0x27,
                          "void StringData::decRef()");
        }
        if (getBuffer()[size_ - 1] != 0)
        {
            __assert_fail("getBuffer()[size_-1] == 0", "../../Include/Core/Strings.cpp", 0x28,
                          "void StringData::decRef()");
        }
        ::free(this);
    }
}

bool StringData::isUnique() const
{
    return refCount_ == 1;
}

bool StringData::isShared() const
{
    return refCount_ > 1;
}

int32 StringData::getSize() const
{
    return size_;
}

unsigned char* StringData::getBuffer()
{
    return (unsigned char*)(this + 1);
}

StringData* StringData::create(int32 size)
{
    if (size < 1)
    {
        __assert_fail("size > 0", "../../Include/Core/Strings.cpp", 0x44,
                      "static StringData* StringData::create(int32)");
    }
    StringData* data = (StringData*)malloc(size + 8);
    if (data == NULL)
    {
        return NULL;
    }
    data->refCount_ = 0;
    data->size_ = size;
    return data;
}

int32 CharStringData::getLength() const
{
    return size_ - 1;
}

char* CharStringData::getBuffer()
{
    return (char*)(this + 1);
}

CharStringData* CharStringData::create(int32 length)
{
    if (length <= 0)
    {
        __assert_fail("length > 0", "../../Include/Core/Strings.cpp", 0x5e,
                      "static CharStringData* CharStringData::create(int32)");
    }
    StringData* data = StringData::create(length + 1);
    if (data == NULL)
    {
        return NULL;
    }
    return (CharStringData*)data;
}

CharStringData* CharStringData::createTerminated(int32 length)
{
    CharStringData* data = create(length);
    data->getBuffer()[length] = '\0';
    return data;
}

int32 WideStringData::getLength() const
{
    return size_ / 4 - 1;
}

wchar* WideStringData::getBuffer()
{
    return (wchar*)(this + 1);
}

WideStringData* WideStringData::create(int32 length)
{
    if (length <= 0)
    {
        __assert_fail("length > 0", "../../Include/Core/Strings.cpp", 0x54d,
                      "static WideStringData* WideStringData::create(int32)");
    }
    StringData* data = StringData::create((length + 1) * 4);
    if (data == NULL)
    {
        return NULL;
    }
    return (WideStringData*)data;
}

WideStringData* WideStringData::createTerminated(int32 length)
{
    WideStringData* data = create(length);
    data->getBuffer()[length] = 0;
    return data;
}

CharString::CharString()
{
    attachData(s_emptyCharStringData);
}

CharString::CharString(const CharString& src)
{
    attachData(src.getData());
}

CharString::CharString(const char* src)
{
    size_t length = strlen(src);
    if (length == 0)
    {
        attachData(s_emptyCharStringData);
    }
    else
    {
        CharStringData* data = CharStringData::create((int32)length);
        memcpy(data->getBuffer(), src, length + 1);
        attachData(data);
    }
}

CharString::CharString(const char* src, int32 length)
{
    if (length == 0)
    {
        attachData(s_emptyCharStringData);
    }
    else
    {
        CharStringData* data = CharStringData::createTerminated(length);
        memcpy(data->getBuffer(), src, length);
        attachData(data);
    }
}

CharString::CharString(const std::string& src)
{
    int32 length = (int32)src.size();
    if (length == 0)
    {
        attachData(s_emptyCharStringData);
    }
    else
    {
        CharStringData* data = CharStringData::create(length);
        memcpy(data->getBuffer(), src.c_str(), length + 1);
        attachData(data);
    }
}

CharString::CharString(char src)
{
    CharStringData* data = CharStringData::createTerminated(1);
    data->getBuffer()[0] = src;
    attachData(data);
}

CharString::CharString(CharStringData* newData)
{
    attachData(newData);
}

CharString::~CharString()
{
    getData()->decRef();
}

bool CharString::isWhiteSpace() const
{
    const char* buf = buffer_;
    while (true)
    {
        if (*buf == '\0')
        {
            return true;
        }
        if (isspace((int)*buf) == 0)
        {
            break;
        }
        buf = buf + 1;
    }
    return false;
}

uint32 CharString::getHash() const
{
    if (isEmpty())
    {
        return 0;
    }
    uint32 value = 0;
    const char* str = buffer_;
    while (true)
    {
        char c = *str;
        str = str + 1;
        if (c == '\0')
        {
            break;
        }
        value = value * 0x1003f + (int)c;
    }
    if (value == 0)
    {
        value = 1;
    }
    return value;
}

char CharString::getAt(int32 idx) const
{
    if (idx < 0 || idx > length())
    {
        __assert_fail("(idx >= 0) && (idx <= length())", "../../Include/Core/Strings.cpp", 0xe2,
                      "char CharString::getAt(int32) const");
    }
    return buffer_[idx];
}

CharString CharString::setAt(int32 idx, char ch)
{
    if (idx >= 0 && idx < length())
    {
        if (ch == '\0')
        {
            __assert_fail("ch != 0", "../../Include/Core/Strings.cpp", 0xe9,
                          "CharString CharString::setAt(int32, char)");
        }
        if (buffer_[idx] == ch)
        {
            return CharString(*this);
        }
        CharStringData* thisData = getData();
        CharStringData* newData = CharStringData::create(thisData->getLength());
        memcpy(newData->getBuffer(), thisData->getBuffer(), thisData->getLength() + 1);
        newData->getBuffer()[idx] = ch;
        return CharString(newData);
    }
    __assert_fail("(idx >= 0) && (idx < length())", "../../Include/Core/Strings.cpp", 0xe8,
                  "CharString CharString::setAt(int32, char)");
}

char CharString::front() const
{
    return *buffer_;
}

char CharString::back() const
{
    if (length() == 0)
    {
        return '\0';
    }
    return buffer_[length() - 1];
}

const char* CharString::begin() const
{
    return buffer_;
}

const char* CharString::end() const
{
    return buffer_ + length();
}

void CharString::assign(const CharString& src)
{
    if (getData() != src.getData())
    {
        replaceData(src.getData());
    }
}

void CharString::assign(const char* src)
{
    size_t srcLength = strlen(src);
    assign(src, (int32)srcLength);
}

void CharString::assign(const char* src, int32 srcLength)
{
    if (srcLength == 0)
    {
        clear();
    }
    else
    {
        CharStringData* newData = CharStringData::createTerminated(srcLength);
        memcpy(newData->getBuffer(), src, srcLength);
        replaceData(newData);
    }
}

void CharString::assign(char src)
{
    if (src == '\0')
    {
        clear();
    }
    else
    {
        CharStringData* newData = CharStringData::createTerminated(1);
        newData->getBuffer()[0] = src;
        replaceData(newData);
    }
}

void CharString::clear()
{
    if (isEmpty())
    {
    }
    else
    {
        replaceData(s_emptyCharStringData);
    }
}

char CharString::operator[](int idx) const
{
    return getAt(idx);
}

CharString::operator const char*() const
{
    return buffer_;
}

CharString& CharString::operator=(const CharString& src)
{
    assign(src);
    return *this;
}

CharString& CharString::operator=(const char* src)
{
    assign(src);
    return *this;
}

CharString& CharString::operator=(const std::string& src)
{
    assign(src.c_str(), (int32)src.size());
    return *this;
}

CharString& CharString::operator=(char src)
{
    assign(src);
    return *this;
}

void CharString::attachData(CharStringData* newData)
{
    newData->incRef();
    buffer_ = newData->getBuffer();
}

void CharString::replaceData(CharStringData* newData)
{
    CharStringData* oldData = getData();
    newData->incRef();
    buffer_ = newData->getBuffer();
    oldData->decRef();
}

CharStringData* CharString::getData() const
{
    return (CharStringData*)(buffer_ - 8);
}

WideString::WideString()
{
    attachData(s_emptyWideStringData);
}

WideString::WideString(const WideString& src)
{
    attachData(src.getData());
}

WideString::WideString(const wchar* src)
{
    size_t length = wcslen(src);
    if (length == 0)
    {
        attachData(s_emptyWideStringData);
    }
    else
    {
        WideStringData* data = WideStringData::create((int32)length);
        memcpy(data->getBuffer(), src, (length + 1) * 4);
        attachData(data);
    }
}

WideString::WideString(const wchar* src, int32 length)
{
    if (length == 0)
    {
        attachData(s_emptyWideStringData);
    }
    else
    {
        WideStringData* data = WideStringData::createTerminated(length);
        memcpy(data->getBuffer(), src, length * 4);
        attachData(data);
    }
}

WideString::WideString(const std::wstring& src)
{
    int32 length = (int32)src.size();
    if (length == 0)
    {
        attachData(s_emptyWideStringData);
    }
    else
    {
        WideStringData* data = WideStringData::create(length);
        memcpy(data->getBuffer(), src.c_str(), (length + 1) * 4);
        attachData(data);
    }
}

WideString::WideString(wchar src)
{
    WideStringData* data = WideStringData::createTerminated(1);
    data->getBuffer()[0] = src;
    attachData(data);
}

WideString::WideString(WideStringData* newData)
{
    attachData(newData);
}

WideString::~WideString()
{
    getData()->decRef();
}

wchar WideString::front() const
{
    return *buffer_;
}

wchar WideString::back() const
{
    if (length() == 0)
    {
        return 0;
    }
    return buffer_[length() - 1];
}

const wchar* WideString::begin() const
{
    return buffer_;
}

const wchar* WideString::end() const
{
    return buffer_ + length();
}

void WideString::attachData(WideStringData* newData)
{
    newData->incRef();
    buffer_ = newData->getBuffer();
}

void WideString::replaceData(WideStringData* newData)
{
    WideStringData* oldData = getData();
    newData->incRef();
    buffer_ = newData->getBuffer();
    oldData->decRef();
}

WideStringData* WideString::getData() const
{
    return (WideStringData*)(buffer_ - 2);
}

int32 CharString::compare(const CharString& src1, const char* src2)
{
    return strcmp(src1.c_str(), src2);
}

int32 CharString::compare(const CharString& src1, const CharString& src2)
{
    return strcmp(src1.c_str(), src2.c_str());
}

int32 CharString::compare(const char* src1, const CharString& src2)
{
    return strcmp(src1, src2.c_str());
}

int32 CharString::compareNoCase(const CharString& src1, const char* src2)
{
    return strcasecmp(src1.c_str(), src2);
}

int32 CharString::compareNoCase(const CharString& src1, const CharString& src2)
{
    return strcasecmp(src1.c_str(), src2.c_str());
}

int32 CharString::compareNoCase(const char* src1, const CharString& src2)
{
    return strcasecmp(src1, src2.c_str());
}

CharString CharString::concat(const char* src1, int32 src1Len, const char* src2, int32 src2Len)
{
    if (src1Len + src2Len == 0)
    {
        return CharString();
    }
    CharStringData* newData = CharStringData::createTerminated(src1Len + src2Len);
    char* newDataBuf = newData->getBuffer();
    memcpy(newDataBuf, src1, src1Len);
    newDataBuf = newData->getBuffer();
    memcpy(newDataBuf + src1Len, src2, src2Len);
    return CharString(newData);
}

CharString CharString::concat(const char* src1, const char* src2)
{
    size_t len2 = strlen(src2);
    size_t len1 = strlen(src1);
    return concat(src1, (int32)len1, src2, (int32)len2);
}

CharString CharString::concat(const CharString& src1, const char* src2)
{
    size_t len2 = strlen(src2);
    return concat(src1, src2, (int32)len2);
}

CharString CharString::concat(const CharString& src1, const char* src2, int32 src2Len)
{
    if (src1.length() == 0)
    {
        return CharString(src2, src2Len);
    }
    else if (src2Len == 0)
    {
        return CharString(src1);
    }
    else
    {
        return concat(src1.c_str(), src1.length(), src2, src2Len);
    }
}

CharString CharString::concat(const char* src1, const CharString& src2)
{
    size_t len1 = strlen(src1);
    return concat(src1, (int32)len1, src2);
}

CharString CharString::concat(const char* src1, int32 src1Len, const CharString& src2)
{
    if (src1Len == 0)
    {
        return CharString(src2);
    }
    else
    {
        return concat(src1, src1Len, src2.c_str(), src2.length());
    }
}

CharString CharString::concat(const CharString& src1, const CharString& src2)
{
    if (src1.length() == 0)
    {
        return CharString(src2);
    }
    else
    {
        if (src2.length() == 0)
        {
            return CharString(src1);
        }
        else
        {
            return concat(src1.c_str(), src1.length(), src2.c_str(), src2.length());
        }
    }
}

CharString CharString::concat(const CharString& src1, char ch)
{
    if (src1.length() == 0)
    {
        return CharString(ch);
    }
    else if (ch == '\0')
    {
        return CharString(src1);
    }
    else
    {
        CharStringData* newData = CharStringData::createTerminated(src1.length() + 1);
        size_t n = src1.length();
        memcpy(newData->getBuffer(), src1.c_str(), n);
        newData->getBuffer()[src1.length()] = ch;
        return CharString(newData);
    }
}

CharString CharString::concat(char ch, const CharString& src1)
{
    CharString tmp(ch);
    return tmp.concat(tmp, src1);
}

CharString CharString::pattern(const char* pattern, int32 count)
{
    size_t patternLen = strlen(pattern);
    if (patternLen == 0 || count < 1)
    {
        return CharString();
    }
    CharStringData* newData = CharStringData::createTerminated((int32)(patternLen * count));
    char* buffer = newData->getBuffer();
    for (int32 i = 0; i < count; i = i + 1)
    {
        memcpy(buffer, pattern, patternLen);
        buffer = buffer + patternLen;
    }
    return CharString(newData);
}

CharString left(const CharString& str, int32 length)
{
    if (length < 0)
    {
        __assert_fail("0 <= length", "../../Include/Core/Strings.cpp", 0x2c4,
                      "CharString left(const CharString&, int32)");
    }
    if (length < str.length())
    {
        return CharString(str.c_str(), length);
    }
    return CharString(str);
}

CharString mid(const CharString& str, int32 start)
{
    if (start < 0)
    {
        __assert_fail("0 <= start", "../../Include/Core/Strings.cpp", 0x2cd,
                      "CharString mid(const CharString&, int32)");
    }
    if (start == 0)
    {
        return CharString(str);
    }
    if (start < str.length())
    {
        return CharString(str.c_str() + start, str.length() - start);
    }
    return CharString();
}

CharString mid(const CharString& str, int32 start, int32 length)
{
    if (start < 0)
    {
        __assert_fail("0 <= start", "../../Include/Core/Strings.cpp", 0x2d9,
                      "CharString mid(const CharString&, int32, int32)");
    }
    if (length < 0)
    {
        __assert_fail("0 <= length", "../../Include/Core/Strings.cpp", 0x2da,
                      "CharString mid(const CharString&, int32, int32)");
    }
    if (start < str.length())
    {
        if (str.length() < start + length)
        {
            length = str.length() - start;
        }
        return CharString(str.c_str() + start, length);
    }
    return CharString();
}

CharString right(const CharString& str, int32 length)
{
    if (length < 0)
    {
        __assert_fail("0 <= length", "../../Include/Core/Strings.cpp", 0x2ed,
                      "CharString right(const CharString&, int32)");
    }
    if (length < str.length())
    {
        return CharString(str.c_str() + (str.length() - length), length);
    }
    return CharString(str);
}

CharString lower(const CharString& str)
{
    if (str.length() == 0)
    {
        return CharString();
    }
    CharStringData* newData = CharStringData::createTerminated(str.length());
    const char* strBuf = str.c_str();
    char* newBuf = newData->getBuffer();
    for (int32 i = 0; i < str.length(); i = i + 1)
    {
        newBuf[i] = (char)tolower((int)strBuf[i]);
    }
    return CharString(newData);
}

CharString upper(const CharString& str)
{
    if (str.length() == 0)
    {
        return CharString();
    }
    CharStringData* newData = CharStringData::createTerminated(str.length());
    const char* strBuf = str.c_str();
    char* newBuf = newData->getBuffer();
    for (int32 i = 0; i < str.length(); i = i + 1)
    {
        newBuf[i] = (char)toupper((int)strBuf[i]);
    }
    return CharString(newData);
}

CharString reverse(const CharString& str)
{
    if (str.length() == 0)
    {
        return CharString();
    }
    CharStringData* newData = CharStringData::createTerminated(str.length());
    const char* strBuf = str.c_str();
    char* newBuf = newData->getBuffer();
    int32 strLength = str.length();
    for (int32 i = 0; i < str.length(); i = i + 1)
    {
        newBuf[i] = strBuf[(strLength - i) - 1];
    }
    return CharString(newData);
}

CharString trimLeft(const CharString& str)
{
    if (str.length() == 0)
    {
        return CharString();
    }
    const char* strBuf = str.c_str();
    int32 pos = 0;
    while (pos < str.length())
    {
        if (isspace((int)strBuf[pos]) == 0)
        {
            break;
        }
        pos = pos + 1;
    }
    if (pos == 0)
    {
        return CharString(str);
    }
    CharStringData* newData = CharStringData::createTerminated(str.length() - pos);
    memcpy(newData->getBuffer(), strBuf + pos, str.length() - pos);
    return CharString(newData);
}

CharString trimRight(const CharString& str)
{
    if (str.length() == 0)
    {
        return CharString();
    }
    const char* strBuf = str.c_str();
    int32 strLength = str.length();
    for (int32 pos = strLength - 1; pos > -1; pos = pos - 1)
    {
        if (isspace((int)strBuf[pos]) == 0)
        {
            if (pos == strLength - 1)
            {
                return CharString(str);
            }
            CharStringData* newData = CharStringData::createTerminated(pos + 1);
            memcpy(newData->getBuffer(), strBuf, pos + 1);
            return CharString(newData);
        }
    }
    return CharString();
}

CharString trim(const CharString& str)
{
    return trimLeft(trimRight(str));
}

CharString insert(const CharString& str, int32 pos, const char* src)
{
    size_t srcLength = strlen(src);
    return insert(str, pos, src, (int32)srcLength);
}

CharString insert(const CharString& str, int32 pos, const char* src, int32 srcLength)
{
    if (pos < 0 || pos > str.length())
    {
        __assert_fail("0 <= pos && pos <= str.length()", "../../Include/Core/Strings.cpp", 0x244,
                      "CharString insert(const CharString&, int32, const char*, int32)");
    }
    if (str.length() == 0)
    {
        return CharString(src, srcLength);
    }
    else if (srcLength == 0)
    {
        return CharString(str);
    }
    else
    {
        CharStringData* newData = CharStringData::createTerminated(str.length() + srcLength);
        memcpy(newData->getBuffer(), str.c_str(), pos);
        memcpy(newData->getBuffer() + pos, src, srcLength);
        memcpy(newData->getBuffer() + pos + srcLength, str.c_str() + pos, str.length() - pos);
        return CharString(newData);
    }
}

CharString insert(const CharString& str, int32 pos, const CharString& src)
{
    return insert(str, pos, src.c_str(), src.length());
}

CharString insert(const CharString& str, int32 pos, char src)
{
    if (pos < 0 || pos > str.length())
    {
        __assert_fail("0 <= pos && pos <= str.length()", "../../Include/Core/Strings.cpp", 0x23f,
                      "CharString insert(const CharString&, int32, char)");
    }
    if (str.length() == 0)
    {
        return CharString(src);
    }
    else if (src == '\0')
    {
        return CharString(str);
    }
    else
    {
        CharStringData* newData = CharStringData::createTerminated(str.length() + 1);
        memcpy(newData->getBuffer(), str.c_str(), pos);
        newData->getBuffer()[pos] = src;
        memcpy(newData->getBuffer() + pos + 1, str.c_str() + pos, str.length() - pos);
        return CharString(newData);
    }
}

CharString remove(const CharString& str, int32 pos, int32 removeLength)
{
    if (pos < 0 || pos > str.length())
    {
        __assert_fail("0 <= pos && pos <= str.length()", "../../Include/Core/Strings.cpp", 0x253,
                      "CharString remove(const CharString&, int32, int32)");
    }
    if (removeLength < 0 || pos + removeLength > str.length())
    {
        __assert_fail("0 <= removeLength && pos + removeLength <= str.length()",
                      "../../Include/Core/Strings.cpp", 0x254,
                      "CharString remove(const CharString&, int32, int32)");
    }
    if (removeLength == 0)
    {
        return CharString(str);
    }
    else if (str.length() == removeLength)
    {
        return CharString();
    }
    else
    {
        CharStringData* newData = CharStringData::createTerminated(str.length() - removeLength);
        memcpy(newData->getBuffer(), str.c_str(), pos);
        memcpy(newData->getBuffer() + pos, str.c_str() + pos + removeLength,
               (str.length() - pos) - removeLength);
        return CharString(newData);
    }
}

CharString replace(const CharString& str, char oldChar, char newChar)
{
    CharStringData* newData = CharStringData::createTerminated(str.length());
    memcpy(newData->getBuffer(), str.c_str(), str.length() + 1);
    for (int32 i = 0; i < str.length(); i = i + 1)
    {
        if (str.c_str()[i] == oldChar)
        {
            newData->getBuffer()[i] = newChar;
        }
    }
    return CharString(newData);
}

CharString operator+(const CharString& src1, const char* src2)
{
    return src1.concat(src1, src2);
}

CharString operator+(const CharString& src1, const CharString& src2)
{
    return src1.concat(src1, src2);
}

CharString operator+(const CharString& src1, char src2)
{
    return src1.concat(src1, src2);
}

CharString operator+(const char* src1, const CharString& src2)
{
    return src2.concat(src1, src2);
}

CharString operator+(char src1, const CharString& src2)
{
    return src2.concat(src1, src2);
}

bool operator==(const CharString& src1, const char* src2)
{
    if (src1[0] == *src2)
    {
        return src1.compare(src1, src2) == 0;
    }
    return false;
}

bool operator==(const CharString& src1, const CharString& src2)
{
    if (src1.length() == src2.length() && src1[0] == src2[0])
    {
        return src1.compare(src1, src2) == 0;
    }
    return false;
}

bool operator==(const char* src1, const CharString& src2)
{
    if (*src1 == src2[0])
    {
        return src2.compare(src1, src2) == 0;
    }
    return false;
}

bool operator!=(const CharString& src1, const char* src2)
{
    return src1.compare(src1, src2) != 0;
}

bool operator!=(const CharString& src1, const CharString& src2)
{
    return src1.compare(src1, src2) != 0;
}

bool operator!=(const char* src1, const CharString& src2)
{
    return src2.compare(src1, src2) != 0;
}

bool operator<(const CharString& src1, const char* src2)
{
    return src1.compare(src1, src2) < 0;
}

bool operator<(const CharString& src1, const CharString& src2)
{
    return src1.compare(src1, src2) < 0;
}

bool operator<(const char* src1, const CharString& src2)
{
    return src2.compare(src1, src2) < 0;
}

bool operator<=(const CharString& src1, const char* src2)
{
    return src1.compare(src1, src2) < 1;
}

bool operator<=(const CharString& src1, const CharString& src2)
{
    return src1.compare(src1, src2) < 1;
}

bool operator<=(const char* src1, const CharString& src2)
{
    return src2.compare(src1, src2) < 1;
}

bool operator>(const CharString& src1, const char* src2)
{
    return 0 < src1.compare(src1, src2);
}

bool operator>(const CharString& src1, const CharString& src2)
{
    return 0 < src1.compare(src1, src2);
}

bool operator>(const char* src1, const CharString& src2)
{
    return 0 < src2.compare(src1, src2);
}

bool operator>=(const CharString& src1, const char* src2)
{
    return src1.compare(src1, src2) >= 0;
}

bool operator>=(const CharString& src1, const CharString& src2)
{
    return src1.compare(src1, src2) >= 0;
}

bool operator>=(const char* src1, const CharString& src2)
{
    return src2.compare(src1, src2) >= 0;
}
