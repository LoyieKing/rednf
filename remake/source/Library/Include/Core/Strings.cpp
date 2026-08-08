// Rebuilt from df_auction_r (DWARF + Ghidra decompile), 2026-08-08
#include "Strings.h"
#include "SecureStdio.h"

#include <assert.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <wctype.h>

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
    while (*buf != '\0')
    {
        if (isspace((int)*buf) == 0)
        {
            return false;
        }
        buf = buf + 1;
    }
    return true;
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
        value = (value << 6) + (value << 16) - value + (int)c;
    }
    if (value == 0)
    {
        value = 1;
    }
    return value;
}

char CharString::getAt(int32 idx) const
{
    if (idx >= 0)
    {
        if (idx <= length())
        {
            return buffer_[idx];
        }
    }
    __assert_fail("(idx >= 0) && (idx <= length())", "../../Include/Core/Strings.cpp", 0xe2,
                  "char CharString::getAt(int32) const");
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

bool CharString::startsWith(const char* pat, bool isIgnoreCase) const
{
    size_t patLength = strlen(pat);
    bool bVar4;
    if (length() < (int32)patLength)
    {
        bVar4 = false;
    }
    else if (isIgnoreCase)
    {
        const char* pcVar2 = c_str();
        bVar4 = strncasecmp(pcVar2, pat, patLength) == 0;
    }
    else
    {
        const char* pcVar2 = c_str();
        bVar4 = strncmp(pcVar2, pat, patLength) == 0;
    }
    return bVar4;
}

bool CharString::endsWith(const char* pat, bool isIgnoreCase) const
{
    size_t patLength = strlen(pat);
    bool bVar4;
    if (length() < (int32)patLength)
    {
        bVar4 = false;
    }
    else if (isIgnoreCase)
    {
        const char* pcVar2 = c_str();
        int32 iVar1 = length();
        bVar4 = strncasecmp(pcVar2 + (iVar1 - patLength), pat, patLength) == 0;
    }
    else
    {
        const char* pcVar2 = c_str();
        int32 iVar1 = length();
        bVar4 = strncmp(pcVar2 + (iVar1 - patLength), pat, patLength) == 0;
    }
    return bVar4;
}

char CharString::front() const
{
    return *buffer_;
}

char CharString::back() const
{
    return length() == 0 ? '\0' : buffer_[length() - 1];
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
        return;
    }
    replaceData(s_emptyCharStringData);
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

int32 CharString::find(const char* pat) const
{
    return find(0, pat, (int32)strlen(pat));
}

int32 CharString::find(int32 pos, const char* pat) const
{
    return find(pos, pat, (int32)strlen(pat));
}

int32 CharString::find(const char* pat, int32 patLength) const
{
    return find(0, pat, patLength);
}

int32 CharString::find(int32 pos, const char* pat, int32 patLength) const
{
    if (patLength == 0)
    {
        return -1;
    }
    if (pos < 0 || length() - pos < patLength)
    {
        return -1;
    }
    const char* strBuf = c_str();
    int32 endPos = length() - patLength;
    for (int32 thisPos = pos; thisPos <= endPos; ++thisPos)
    {
        if (memcmp(strBuf + thisPos, pat, patLength) == 0)
        {
            return thisPos;
        }
    }
    return -1;
}

int32 CharString::find(const CharString& pat) const
{
    return find(0, pat.c_str(), pat.length());
}

int32 CharString::find(int32 pos, const CharString& pat) const
{
    return find(pos, pat.c_str(), pat.length());
}

int32 CharString::find(char pat) const
{
    return find(0, pat);
}

int32 CharString::find(int32 pos, char pat) const
{
    if (pat == '\0')
    {
        return -1;
    }
    if (pos < 0 || length() <= pos)
    {
        return -1;
    }
    const char* strBuf = c_str();
    int32 endPos = length() - 1;
    for (int32 thisPos = pos; thisPos <= endPos; ++thisPos)
    {
        if (strBuf[thisPos] == pat)
        {
            return thisPos;
        }
    }
    return -1;
}

int32 CharString::rfind(const char* pat) const
{
    int32 patLength = (int32)strlen(pat);
    return rfind(length() - patLength, pat, patLength);
}

int32 CharString::rfind(int32 pos, const char* pat) const
{
    return rfind(pos, pat, (int32)strlen(pat));
}

int32 CharString::rfind(const char* pat, int32 patLength) const
{
    return rfind(length() - patLength, pat, patLength);
}

int32 CharString::rfind(int32 pos, const char* pat, int32 patLength) const
{
    if (patLength == 0)
    {
        return -1;
    }
    if (pos < 0 || length() - pos < patLength)
    {
        return -1;
    }
    const char* strBuf = c_str();
    for (int32 thisPos = pos; thisPos > -1; --thisPos)
    {
        if (memcmp(strBuf + thisPos, pat, patLength) == 0)
        {
            return thisPos;
        }
    }
    return -1;
}

int32 CharString::rfind(const CharString& pat) const
{
    int32 patLength = pat.length();
    return rfind(length() - patLength, pat.c_str(), patLength);
}

int32 CharString::rfind(int32 pos, const CharString& pat) const
{
    return rfind(pos, pat.c_str(), pat.length());
}

int32 CharString::rfind(char pat) const
{
    return rfind(length() - 1, pat);
}

int32 CharString::rfind(int32 pos, char pat) const
{
    if (pat == '\0')
    {
        return -1;
    }
    if (pos < 0 || length() <= pos)
    {
        return -1;
    }
    const char* strBuf = c_str();
    for (int32 thisPos = pos; thisPos > -1; --thisPos)
    {
        if (strBuf[thisPos] == pat)
        {
            return thisPos;
        }
    }
    return -1;
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
    return length() == 0 ? 0 : buffer_[length() - 1];
}

const wchar* WideString::begin() const
{
    return buffer_;
}

const wchar* WideString::end() const
{
    return buffer_ + length();
}

bool WideString::isWhiteSpace() const
{
    const wchar* buf = buffer_;
    while (*buf != 0)
    {
        if (!isuspace(*buf))
        {
            return false;
        }
        buf = buf + 1;
    }
    return true;
}

uint32 WideString::getHash() const
{
    if (isEmpty())
    {
        return 0;
    }
    uint32 value = 0;
    const wchar* str = buffer_;
    while (true)
    {
        wchar c = *str;
        str = str + 1;
        if (c == 0)
        {
            break;
        }
        value = (value << 6) + (value << 16) - value + (uint32)c;
    }
    if (value == 0)
    {
        value = 1;
    }
    return value;
}

wchar WideString::getAt(int32 idx) const
{
    if (idx >= 0)
    {
        if (idx <= length())
        {
            return buffer_[idx];
        }
    }
    __assert_fail("(idx >= 0) && (idx <= length())", "../../Include/Core/Strings.cpp", 0x5d1,
                  "wchar WideString::getAt(int32) const");
}

WideString WideString::setAt(int32 idx, wchar ch)
{
    if (idx >= 0 && idx < length())
    {
        if (ch == 0)
        {
            __assert_fail("ch != 0", "../../Include/Core/Strings.cpp", 0x5d8,
                          "WideString WideString::setAt(int32, wchar)");
        }
        if (buffer_[idx] == ch)
        {
            return WideString(*this);
        }
        WideStringData* thisData = getData();
        WideStringData* newData = WideStringData::create(thisData->getLength());
        memcpy(newData->getBuffer(), thisData->getBuffer(), (thisData->getLength() + 1) * 4);
        newData->getBuffer()[idx] = ch;
        return WideString(newData);
    }
    __assert_fail("(idx >= 0) && (idx < length())", "../../Include/Core/Strings.cpp", 0x5d7,
                  "WideString WideString::setAt(int32, wchar)");
}

bool WideString::startsWith(const wchar* pat, bool isIgnoreCase) const
{
    size_t patLength = wcslen(pat);
    bool bVar4;
    if (length() < (int32)patLength)
    {
        bVar4 = false;
    }
    else if (isIgnoreCase)
    {
        const wchar* pwVar2 = c_str();
        bVar4 = wcsncasecmp(pwVar2, pat, patLength) == 0;
    }
    else
    {
        const wchar* pwVar2 = c_str();
        bVar4 = wcsncmp(pwVar2, pat, patLength) == 0;
    }
    return bVar4;
}

bool WideString::endsWith(const wchar* pat, bool isIgnoreCase) const
{
    size_t patLength = wcslen(pat);
    bool bVar4;
    if (length() < (int32)patLength)
    {
        bVar4 = false;
    }
    else if (isIgnoreCase)
    {
        const wchar* pwVar2 = c_str();
        int32 iVar1 = length();
        bVar4 = wcsncasecmp(pwVar2 + (iVar1 - patLength), pat, patLength) == 0;
    }
    else
    {
        const wchar* pwVar2 = c_str();
        int32 iVar1 = length();
        bVar4 = wcsncmp(pwVar2 + (iVar1 - patLength), pat, patLength) == 0;
    }
    return bVar4;
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

void WideString::assign(const wchar* src)
{
    int32 srcLength = (int32)wcslen(src);
    assign(src, srcLength);
}

void WideString::assign(const wchar* src, int32 srcLength)
{
    if (srcLength == 0)
    {
        clear();
    }
    else
    {
        WideStringData* newData = WideStringData::createTerminated(srcLength);
        memcpy(newData->getBuffer(), src, srcLength * 4);
        replaceData(newData);
    }
}

void WideString::assign(const WideString& src)
{
    if (getData() != src.getData())
    {
        replaceData(src.getData());
    }
}

void WideString::assign(wchar src)
{
    if (src == 0)
    {
        clear();
    }
    else
    {
        WideStringData* newData = WideStringData::createTerminated(1);
        newData->getBuffer()[0] = src;
        replaceData(newData);
    }
}

void WideString::clear()
{
    if (isEmpty())
    {
        return;
    }
    replaceData(s_emptyWideStringData);
}

wchar WideString::operator[](int idx) const
{
    return getAt(idx);
}

WideString::operator const wchar*() const
{
    return buffer_;
}

WideString& WideString::operator=(const WideString& src)
{
    assign(src);
    return *this;
}

WideString& WideString::operator=(const wchar* src)
{
    assign(src);
    return *this;
}

WideString& WideString::operator=(const std::wstring& src)
{
    assign(src.c_str(), (int32)src.size());
    return *this;
}

WideString& WideString::operator=(wchar src)
{
    assign(src);
    return *this;
}

int32 WideString::find(const wchar* pat) const
{
    return find(0, pat, (int32)wcslen(pat));
}

int32 WideString::find(int32 pos, const wchar* pat) const
{
    return find(pos, pat, (int32)wcslen(pat));
}

int32 WideString::find(const wchar* pat, int32 patLength) const
{
    return find(0, pat, patLength);
}

int32 WideString::find(int32 pos, const wchar* pat, int32 patLength) const
{
    if (patLength == 0)
    {
        return -1;
    }
    if (pos < 0 || length() - pos < patLength)
    {
        return -1;
    }
    const wchar* strBuf = c_str();
    int32 endPos = length() - patLength;
    for (int32 thisPos = pos; thisPos <= endPos; ++thisPos)
    {
                if (memcmp(strBuf + thisPos, pat, (size_t)patLength * 4) == 0)
        {
            return thisPos;
        }
    }
    return -1;
}

int32 WideString::find(const WideString& pat) const
{
    return find(0, pat.c_str(), pat.length());
}

int32 WideString::find(int32 pos, const WideString& pat) const
{
    return find(pos, pat.c_str(), pat.length());
}

int32 WideString::find(wchar pat) const
{
    return find(0, pat);
}

int32 WideString::find(int32 pos, wchar pat) const
{
    if (pat == 0)
    {
        return -1;
    }
    if (pos < 0 || length() <= pos)
    {
        return -1;
    }
    const wchar* strBuf = c_str();
    int32 endPos = length() - 1;
    for (int32 thisPos = pos; thisPos <= endPos; ++thisPos)
    {
        if (strBuf[thisPos] == pat)
        {
            return thisPos;
        }
    }
    return -1;
}

int32 WideString::rfind(const wchar* pat) const
{
    int32 patLength = (int32)wcslen(pat);
    return rfind(length() - patLength, pat, patLength);
}

int32 WideString::rfind(int32 pos, const wchar* pat) const
{
    return rfind(pos, pat, (int32)wcslen(pat));
}

int32 WideString::rfind(const wchar* pat, int32 patLength) const
{
    return rfind(length() - patLength, pat, patLength);
}

int32 WideString::rfind(int32 pos, const wchar* pat, int32 patLength) const
{
    if (patLength == 0)
    {
        return -1;
    }
    if (pos < 0 || length() - pos < patLength)
    {
        return -1;
    }
    const wchar* strBuf = c_str();
    for (int32 thisPos = pos; thisPos > -1; --thisPos)
    {
                if (memcmp(strBuf + thisPos, pat, (size_t)patLength * 4) == 0)
        {
            return thisPos;
        }
    }
    return -1;
}

int32 WideString::rfind(const WideString& pat) const
{
    int32 patLength = pat.length();
    return rfind(length() - patLength, pat.c_str(), patLength);
}

int32 WideString::rfind(int32 pos, const WideString& pat) const
{
    return rfind(pos, pat.c_str(), pat.length());
}

int32 WideString::rfind(wchar pat) const
{
    return rfind(length() - 1, pat);
}

int32 WideString::rfind(int32 pos, wchar pat) const
{
    if (pat == 0)
    {
        return -1;
    }
    if (pos < 0 || length() <= pos)
    {
        return -1;
    }
    const wchar* strBuf = c_str();
    for (int32 thisPos = pos; thisPos > -1; --thisPos)
    {
        if (strBuf[thisPos] == pat)
        {
            return thisPos;
        }
    }
    return -1;
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
    else if (src2.length() == 0)
    {
        return CharString(src1, src1Len);
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
    if (ch == '\0')
    {
        return CharString(src1);
    }
    else if (src1.length() == 0)
    {
        return CharString(ch);
    }
    else
    {
        CharStringData* newData = CharStringData::createTerminated(src1.length() + 1);
        newData->getBuffer()[0] = ch;
        memcpy(newData->getBuffer() + 1, src1.c_str(), src1.length());
        return CharString(newData);
    }
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

CharString CharString::format(const char* format, ...)
{
    va_list argList;
    va_start(argList, format);
    int length = vsnprintf((char*)0, 0, format, argList);
    if (length == 0)
    {
        va_end(argList);
        return CharString();
    }
    CharStringData* newData = CharStringData::create(length);
    char* buffer = newData->getBuffer();
    ss_vsprintf(buffer, length + 1, format, argList);
    va_end(argList);
    return CharString(newData);
}

CharString CharString::vformat(const char* format, char* argList)
{
    int length = vsnprintf((char*)0, 0, format, (va_list)argList);
    if (length == 0)
    {
        return CharString();
    }
    CharStringData* newData = CharStringData::create(length);
    char* buffer = newData->getBuffer();
    ss_vsprintf(buffer, length + 1, format, argList);
    return CharString(newData);
}

int32 CharString::tokenize(const CharString& str, std::vector<CharString>* tokenVector)
{
    return tokenize(str, tokenVector, " \t\n\r", ".,", false);
}

int32 CharString::tokenize(const CharString& str, std::vector<CharString>* tokenVector,
                           const char* dropDelimiters, const char* returnDelimiters,
                           bool isKeepEmptyToken)
{
    const char* strBuf = str.c_str();
    int32 strLen = str.length();
    int32 nextTokenStartIdx = 0;
    int32 tokenCount = 0;
    for (int32 i = 0; i < strLen; i = i + 1)
    {
        char c = strBuf[i];
        const char* d0 = dropDelimiters;
        while (true)
        {
            if (*d0 == '\0' || *d0 == c)
            {
                break;
            }
            d0 = d0 + 1;
        }
        if (*d0 == '\0')
        {
            const char* d1 = returnDelimiters;
            while (true)
            {
                if (*d1 == '\0' || *d1 == c)
                {
                    break;
                }
                d1 = d1 + 1;
            }
            if (*d1 != '\0')
            {
                int32 newLen = i - nextTokenStartIdx;
                if (newLen == 0)
                {
                    if (isKeepEmptyToken)
                    {
                        tokenVector->push_back(CharString());
                        tokenCount = tokenCount + 1;
                    }
                }
                else
                {
                    tokenVector->push_back(mid(str, nextTokenStartIdx, newLen));
                    tokenCount = tokenCount + 1;
                }
                tokenVector->push_back(CharString(*d1));
                tokenCount = tokenCount + 1;
                nextTokenStartIdx = i + 1;
            }
        }
        else
        {
            int32 newLen = i - nextTokenStartIdx;
            if (newLen == 0)
            {
                if (isKeepEmptyToken)
                {
                    tokenVector->push_back(CharString());
                    tokenCount = tokenCount + 1;
                }
            }
            else
            {
                tokenVector->push_back(mid(str, nextTokenStartIdx, newLen));
                tokenCount = tokenCount + 1;
            }
            nextTokenStartIdx = i + 1;
        }
    }
    if (nextTokenStartIdx != strLen)
    {
        tokenVector->push_back(mid(str, nextTokenStartIdx, strLen - nextTokenStartIdx));
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}

int32 CharString::tokenizeNewLine(const CharString& str, std::vector<CharString>* tokenVector)
{
    const char* strBuf = str.c_str();
    int32 strLen = str.length();
    int32 nextStartIdx = 0;
    int32 lastNewLineIdx = -1;
    int32 tokenCount = 0;
    for (int32 i = 0; i < strLen; i = i + 1)
    {
        char c = strBuf[i];
        if (c == '\r')
        {
            tokenVector->push_back(mid(str, nextStartIdx, i - nextStartIdx));
            if (strBuf[i + 1] == '\n')
            {
                i = i + 1;
            }
            nextStartIdx = i + 1;
            lastNewLineIdx = i;
        }
        else if (c == '\n')
        {
            tokenVector->push_back(mid(str, nextStartIdx, i - nextStartIdx));
            nextStartIdx = i + 1;
            lastNewLineIdx = i;
        }
    }
    if (nextStartIdx == strLen)
    {
        if (strLen - 1 == lastNewLineIdx)
        {
            tokenVector->push_back(CharString());
            tokenCount = tokenCount + 1;
        }
    }
    else
    {
        tokenVector->push_back(mid(str, nextStartIdx, strLen - nextStartIdx));
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}

CharString CharString::join(const std::vector<CharString>& tokenVector,
                            const CharString& separatorStr)
{
    if (tokenVector.empty())
    {
        return CharString();
    }

    const char* separatorStrSrc = separatorStr.c_str();
    int32 separatorStrLen = separatorStr.length();
    int32 totalLength = (tokenVector.size() - 1) * separatorStrLen;

    bool token_runOnce = false;
    for (std::vector<CharString>::const_iterator token_iterator = tokenVector.begin();
         token_iterator != tokenVector.end() && (token_runOnce = !token_runOnce);
         token_iterator++)
    {
        const CharString& token = *token_iterator;
        while (token_runOnce)
        {
            totalLength = totalLength + token.length();
            token_runOnce = false;
        }
    }

    if (totalLength == 0)
    {
        return CharString();
    }

    CharStringData* newData = CharStringData::createTerminated(totalLength);
    char* newDataBuf = newData->getBuffer();

    token_runOnce = false;
    for (std::vector<CharString>::const_iterator token_iterator = tokenVector.begin();
         token_iterator != tokenVector.end() && (token_runOnce = !token_runOnce);
         token_iterator++)
    {
        const CharString& token = *token_iterator;
        while (token_runOnce)
        {
            if (token_iterator != tokenVector.begin() && separatorStrLen >= 1)
            {
                memcpy(newDataBuf, separatorStrSrc, separatorStrLen);
                newDataBuf = newDataBuf + separatorStrLen;
            }
            int32 tokenLength = token.length();
            memcpy(newDataBuf, token.c_str(), tokenLength);
            newDataBuf = newDataBuf + tokenLength;
            token_runOnce = false;
        }
    }

    return CharString(newData);
}

WideString WideString::concat(const wchar* src1, int32 src1Len, const wchar* src2, int32 src2Len)
{
    if (src1Len + src2Len == 0)
    {
        return WideString();
    }
    WideStringData* newData = WideStringData::createTerminated(src1Len + src2Len);
    wchar* newDataBuf = newData->getBuffer();
    memcpy(newDataBuf, src1, src1Len * 4);
    newDataBuf = newData->getBuffer();
    memcpy(newDataBuf + src1Len, src2, src2Len * 4);
    return WideString(newData);
}

WideString WideString::concat(const wchar* src1, const wchar* src2)
{
    size_t len2 = wcslen(src2);
    size_t len1 = wcslen(src1);
    return concat(src1, (int32)len1, src2, (int32)len2);
}

WideString WideString::concat(const WideString& src1, const wchar* src2)
{
    size_t len2 = wcslen(src2);
    return concat(src1, src2, (int32)len2);
}

WideString WideString::concat(const WideString& src1, const wchar* src2, int32 src2Len)
{
    if (src1.length() == 0)
    {
        return WideString(src2, src2Len);
    }
    else if (src2Len == 0)
    {
        return WideString(src1);
    }
    else
    {
        return concat(src1.c_str(), src1.length(), src2, src2Len);
    }
}

WideString WideString::concat(const wchar* src1, const WideString& src2)
{
    size_t len1 = wcslen(src1);
    return concat(src1, (int32)len1, src2);
}

WideString WideString::concat(const wchar* src1, int32 src1Len, const WideString& src2)
{
    if (src1Len == 0)
    {
        return WideString(src2);
    }
    else
    {
        if (src2.length() == 0)
        {
            return WideString(src1, src1Len);
        }
        else
        {
            return concat(src1, src1Len, src2.c_str(), src2.length());
        }
    }
}

WideString WideString::concat(const WideString& src1, const WideString& src2)
{
    if (src1.length() == 0)
    {
        return WideString(src2);
    }
    else
    {
        if (src2.length() == 0)
        {
            return WideString(src1);
        }
        else
        {
            return concat(src1.c_str(), src1.length(), src2.c_str(), src2.length());
        }
    }
}

WideString WideString::concat(const WideString& src1, wchar ch)
{
    if (src1.length() == 0)
    {
        return WideString(ch);
    }
    else if (ch == 0)
    {
        return WideString(src1);
    }
    else
    {
        WideStringData* newData = WideStringData::createTerminated(src1.length() + 1);
        memcpy(newData->getBuffer(), src1.c_str(), src1.length() * 4);
        newData->getBuffer()[src1.length()] = ch;
        return WideString(newData);
    }
}

WideString WideString::concat(wchar ch, const WideString& src1)
{
    if (ch == 0)
    {
        return WideString(src1);
    }
    else
    {
        if (src1.length() == 0)
        {
            return WideString(ch);
        }
        else
        {
            WideStringData* newData = WideStringData::createTerminated(src1.length() + 1);
            newData->getBuffer()[0] = ch;
            memcpy(newData->getBuffer() + 1, src1.c_str(), src1.length() * 4);
            return WideString(newData);
        }
    }
}

WideString WideString::pattern(const wchar* pattern, int32 count)
{
    size_t patternLen = wcslen(pattern);
    if (patternLen == 0 || count < 1)
    {
        return WideString();
    }
    WideStringData* newData = WideStringData::createTerminated((int32)(patternLen * count));
    wchar* buffer = newData->getBuffer();
    for (int32 i = 0; i < count; i = i + 1)
    {
        memcpy(buffer, pattern, patternLen * 4);
        buffer = buffer + patternLen;
    }
    return WideString(newData);
}

int32 WideString::compare(const WideString& src1, const wchar* src2)
{
    return wcscmp(src1.c_str(), src2);
}

int32 WideString::compare(const WideString& src1, const WideString& src2)
{
    return wcscmp(src1.c_str(), src2.c_str());
}

int32 WideString::compare(const wchar* src1, const WideString& src2)
{
    return wcscmp(src1, src2.c_str());
}

int32 WideString::compareNoCase(const WideString& src1, const wchar* src2)
{
    return wcscasecmp(src1.c_str(), src2);
}

int32 WideString::compareNoCase(const WideString& src1, const WideString& src2)
{
    return wcscasecmp(src1.c_str(), src2.c_str());
}

int32 WideString::compareNoCase(const wchar* src1, const WideString& src2)
{
    return wcscasecmp(src1, src2.c_str());
}

WideString WideString::format(const wchar* format, ...)
{
    va_list argList;
    va_start(argList, format);
    int length = vswprintf((wchar_t*)0, 0, format, argList);
    if (length == 0)
    {
        va_end(argList);
        return WideString();
    }
    WideStringData* newData = WideStringData::create(length);
    wchar_t* buffer = newData->getBuffer();
    ss_vswprintf(buffer, length + 1, format, argList);
    va_end(argList);
    return WideString(newData);
}

WideString WideString::vformat(const wchar* format, char* argList)
{
    int length = vswprintf((wchar_t*)0, 0, format, (va_list)argList);
    if (length == 0)
    {
        return WideString();
    }
    WideStringData* newData = WideStringData::create(length);
    wchar_t* buffer = newData->getBuffer();
    ss_vswprintf(buffer, length + 1, format, argList);
    return WideString(newData);
}

int32 WideString::tokenize(const WideString& str, std::vector<WideString>* tokenVector)
{
    return tokenize(str, tokenVector, L" \t\n\r", L".", false);
}

int32 WideString::tokenize(const WideString& str, std::vector<WideString>* tokenVector,
                           const wchar* dropDelimiters, const wchar* returnDelimiters,
                           bool isKeepEmptyToken)
{
    const wchar* strBuf = str.c_str();
    int32 strLen = str.length();
    int32 nextTokenStartIdx = 0;
    int32 tokenCount = 0;
    for (int32 i = 0; i < strLen; i = i + 1)
    {
        wchar c = strBuf[i];
        const wchar* d0 = dropDelimiters;
        while (true)
        {
            if (*d0 == 0 || *d0 == c)
            {
                break;
            }
            d0 = d0 + 1;
        }
        if (*d0 == 0)
        {
            const wchar* d1 = returnDelimiters;
            while (true)
            {
                if (*d1 == 0 || *d1 == c)
                {
                    break;
                }
                d1 = d1 + 1;
            }
            if (*d1 != 0)
            {
                int32 newLen = i - nextTokenStartIdx;
                if (newLen == 0)
                {
                    if (isKeepEmptyToken)
                    {
                        tokenVector->push_back(WideString());
                        tokenCount = tokenCount + 1;
                    }
                }
                else
                {
                    tokenVector->push_back(mid(str, nextTokenStartIdx, newLen));
                    tokenCount = tokenCount + 1;
                }
                tokenVector->push_back(WideString(*d1));
                tokenCount = tokenCount + 1;
                nextTokenStartIdx = i + 1;
            }
        }
        else
        {
            int32 newLen = i - nextTokenStartIdx;
            if (newLen == 0)
            {
                if (isKeepEmptyToken)
                {
                    tokenVector->push_back(WideString());
                    tokenCount = tokenCount + 1;
                }
            }
            else
            {
                tokenVector->push_back(mid(str, nextTokenStartIdx, newLen));
                tokenCount = tokenCount + 1;
            }
            nextTokenStartIdx = i + 1;
        }
    }
    if (nextTokenStartIdx != strLen)
    {
        tokenVector->push_back(mid(str, nextTokenStartIdx, strLen - nextTokenStartIdx));
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}

int32 WideString::tokenizeNewLine(const WideString& str, std::vector<WideString>* tokenVector)
{
    const wchar* strBuf = str.c_str();
    int32 strLen = str.length();
    int32 nextStartIdx = 0;
    int32 lastNewLineIdx = -1;
    int32 tokenCount = 0;
    for (int32 i = 0; i < strLen; i = i + 1)
    {
        wchar c = strBuf[i];
        if (c == 0xd)
        {
            tokenVector->push_back(mid(str, nextStartIdx, i - nextStartIdx));
            if (strBuf[i + 1] == 0xa)
            {
                i = i + 1;
            }
            nextStartIdx = i + 1;
            lastNewLineIdx = i;
        }
        else if (c == 0xa)
        {
            tokenVector->push_back(mid(str, nextStartIdx, i - nextStartIdx));
            nextStartIdx = i + 1;
            lastNewLineIdx = i;
        }
    }
    if (nextStartIdx == strLen)
    {
        if (strLen - 1 == lastNewLineIdx)
        {
            tokenVector->push_back(WideString());
            tokenCount = tokenCount + 1;
        }
    }
    else
    {
        tokenVector->push_back(mid(str, nextStartIdx, strLen - nextStartIdx));
        tokenCount = tokenCount + 1;
    }
    return tokenCount;
}

WideString WideString::join(const std::vector<WideString>& tokenVector,
                            const WideString& separatorStr)
{
    if (tokenVector.empty())
    {
        return WideString();
    }

    const wchar* separatorStrSrc = separatorStr.c_str();
    int32 separatorStrLen = separatorStr.length();
    int32 totalLength = (tokenVector.size() - 1) * separatorStrLen;

    bool token_runOnce = false;
    for (std::vector<WideString>::const_iterator token_iterator = tokenVector.begin();
         token_iterator != tokenVector.end() && (token_runOnce = !token_runOnce);
         token_iterator++)
    {
        const WideString& token = *token_iterator;
        while (token_runOnce)
        {
            totalLength = totalLength + token.length();
            token_runOnce = false;
        }
    }

    if (totalLength == 0)
    {
        return WideString();
    }

    WideStringData* newData = WideStringData::createTerminated(totalLength);
    wchar* newDataBuf = newData->getBuffer();

    token_runOnce = false;
    for (std::vector<WideString>::const_iterator token_iterator = tokenVector.begin();
         token_iterator != tokenVector.end() && (token_runOnce = !token_runOnce);
         token_iterator++)
    {
        const WideString& token = *token_iterator;
        while (token_runOnce)
        {
            if (token_iterator != tokenVector.begin() && separatorStrLen >= 1)
            {
                memcpy(newDataBuf, separatorStrSrc, separatorStrLen * 4);
                newDataBuf = newDataBuf + separatorStrLen;
            }
            int32 tokenLength = token.length();
            memcpy(newDataBuf, token.c_str(), tokenLength * 4);
            newDataBuf = newDataBuf + tokenLength;
            token_runOnce = false;
        }
    }

    return WideString(newData);
}

bool WideString::isuspace(wchar ch)
{
    bool bVar1;
    if ((((((ch == 9) || (ch == 10)) || (ch == 0xb)) ||
          (((ch == 0xc || (ch == 0xd)) || ((ch == 0x20 || ((ch == 0xa0 || (ch == 0x2000)))))))) ||
         ((ch == 0x2001 ||
          ((((((ch == 0x2002 || (ch == 0x2003)) || (ch == 0x2004)) ||
             ((ch == 0x2005 || (ch == 0x2006)))) ||
            ((ch == 0x2007 || ((ch == 0x2008 || (ch == 0x2009)))))) || (ch == 0x200a)))))) ||
        ((((ch == 0x200b || (ch == 0x202f)) || (ch == 0x205f)) || ((ch == 0x3000 || (ch == 0xfeff)))))
        )
    {
        bVar1 = true;
    }
    else
    {
        bVar1 = false;
    }
    return bVar1;
}

bool WideString::isupunct(wchar ch)
{
    bool bVar1;
    if ((((ch < 0x21 || 0x23 < ch) && (ch < 0x25 || 0x2a < ch)) &&
         ((ch < 0x2c || 0x2f < ch) && ch != 0x3a && ch != 0x3b && ch != 0x3f && ch != 0x40) &&
         ((ch < 0x5b || 0x5d < ch) && ch != 0x5f && ch != 0x7b && ch != 0x7d && ch != 0xa1) &&
         ch != 0xab && ch != 0xad && ch != 0xb7 && ch != 0xbb && ch != 0xbf &&
         ch != 0x37e && ch != 0x387 && (ch < 0x55a || 0x55f < ch) && ch != 0x589 &&
         ch != 0x58a && ch != 0x5be && ch != 0x5c0 &&
         ch != 0x5c3 && ch != 0x5c6 && ch != 0x5f3 &&
         ch != 0x5f4 && ch != 0x60c && ch != 0x60d && ch != 0x61b && ch != 0x61e && ch != 0x61f &&
         (ch < 0x66a || 0x66d < ch) && ch != 0x6d4 && (ch < 0x700 || 0x70d < ch) &&
         (ch < 0x7f7 || 0x7f9 < ch) &&
         ch != 0x964 && ch != 0x965 && ch != 0x970 && ch != 0xdf4 &&
         (ch < 0xe4f || 0xe5b < ch) && (ch < 0xf04 || 0xf12 < ch) &&
         (ch < 0xf3a || 0xf3d < ch) && ch != 0xf85 && ch != 0xfd0 && ch != 0xfd1 &&
         (ch < 0x104a || 0x104f < ch) && ch != 0x10fb &&
         (ch < 0x1361 || 0x1368 < ch) && ch != 0x166d && ch != 0x166e && ch != 0x169b &&
         ch != 0x169c &&
         (ch < 0x16eb || 0x16ed < ch) && ch != 0x1735 && ch != 0x1736 &&
         (ch < 0x17d4 || 0x17d6 < ch) && (ch < 0x17d8 || 0x17da < ch) &&
         (ch < 0x1800 || 0x180a < ch) && ch != 0x1944 && ch != 0x1945 && ch != 0x19de &&
         ch != 0x19df && ch != 0x1a1e && ch != 0x1a1f &&
         (ch < 0x1b5a || 0x1b60 < ch) &&
         (ch < 0x2010 || 0x2027 < ch) && (ch < 0x2030 || 0x2043 < ch) &&
         (ch < 0x2045 || 0x2051 < ch) && (ch < 0x2053 || 0x205e < ch) &&
         ch != 0x207d && ch != 0x207e && ch != 0x208d && ch != 0x208e &&
         ch != 0x2329 && ch != 0x232a && (ch < 0x2768 || 0x2775 < ch) &&
         (ch < 0x27c5 || 0x27c6 < ch) &&
         (ch < 0x27e6 || 0x27eb < ch) && (ch < 0x2983 || 0x2998 < ch) &&
         (ch < 0x29d8 || 0x29db < ch) && ch != 0x29fc && ch != 0x29fd &&
         (ch < 0x2cf9 || 0x2cfc < ch) && ch != 0x2cfe && ch != 0x2cff &&
         (ch < 0x2e00 || 0x2e17 < ch) && ch != 0x2e1c && ch != 0x2e1d &&
         (ch < 0x3001 || 0x3003 < ch) && (ch < 0x3008 || 0x3011 < ch) &&
         (ch < 0x3014 || 0x301f < ch) && ch != 0x3030 && ch != 0x303d && ch != 0x30a0 &&
         ch != 0x30fb &&
         (ch < 0xa874 || 0xa877 < ch) && ch != 0xfd3e && ch != 0xfd3f &&
         (ch < 0xfe10 || 0xfe19 < ch) && (ch < 0xfe30 || 0xfe52 < ch) &&
         (ch < 0xfe54 || 0xfe61 < ch) && ch != 0xfe63 && ch != 0xfe68 && ch != 0xfe6a &&
         ch != 0xfe6b &&
         (ch < 0xff01 || 0xff03 < ch) && (ch < 0xff05 || 0xff0a < ch) &&
         (ch < 0xff0c || 0xff0f < ch) && ch != 0xff1a && ch != 0xff1b && ch != 0xff1f &&
         ch != 0xff20 && (ch < 0xff3b || 0xff3d < ch) && ch != 0xff3f &&
         ch != 0xff5b && ch != 0xff5d && (ch < 0xff5f || 0xff65 < ch)))
    {
        bVar1 = false;
    }
    else
    {
        bVar1 = true;
    }
    return bVar1;
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
    for (int32 pos = 0; pos < (int32)str.length(); pos = pos + 1)
    {
        if (isspace((int)strBuf[pos]) == 0)
        {
            if (pos == 0)
            {
                return CharString(str);
            }
            CharStringData* newData = CharStringData::createTerminated(str.length() - pos);
            memcpy(newData->getBuffer(), strBuf + pos, str.length() - pos);
            return CharString(newData);
        }
    }
    return CharString();
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
    if (str.length() == 0)
    {
        return CharString(src);
    }
    else if (src.length() == 0)
    {
        return CharString(str);
    }
    else
    {
        return insert(str, pos, src.c_str(), src.length());
    }
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
    if (oldChar == '\0')
    {
        __assert_fail("pat != 0", "../../Include/Core/Strings.cpp", 0x267,
                      "CharString replace(const CharString&, char, char)");
    }
    if (newChar == '\0')
    {
        __assert_fail("target != 0", "../../Include/Core/Strings.cpp", 0x268,
                      "CharString replace(const CharString&, char, char)");
    }
    if (str.length() == 0)
    {
        return CharString();
    }
    CharStringData* newData = CharStringData::createTerminated(str.length());
    char* newBuffer = newData->getBuffer();
    const char* strBuffer = str.c_str();
    int32 strLength = str.length();
    for (int32 i = 0; i < strLength; i = i + 1)
    {
        if (strBuffer[i] == oldChar)
        {
            newBuffer[i] = newChar;
        }
        else
        {
            newBuffer[i] = strBuffer[i];
        }
    }
    return CharString(newData);
}

CharString replace(const CharString& str, const char* oldChars, const char* newChars)
{
    size_t newCharsLength = strlen(newChars);
    size_t oldCharsLength = strlen(oldChars);
    return replace(str, oldChars, (int32)oldCharsLength, newChars, (int32)newCharsLength);
}

CharString replace(const CharString& str, const char* oldChars, int32 oldCharsLength,
                   const char* newChars, int32 newCharsLength)
{
    if (oldCharsLength == 0)
    {
        return CharString(str);
    }
    const char* strBuffer = str.c_str();
    int32 strLength = str.length();
    int32 replaceCount = 0;
    int32 i = 0;
    while (i <= strLength - oldCharsLength)
    {
        if (memcmp(strBuffer + i, oldChars, oldCharsLength) == 0)
        {
            replaceCount = replaceCount + 1;
            i = i + oldCharsLength;
        }
        else
        {
            i = i + 1;
        }
    }
    if (replaceCount == 0)
    {
        return CharString(str);
    }
    int32 newLength = (newCharsLength - oldCharsLength) * replaceCount + strLength;
    if (newLength != 0)
    {
        CharStringData* newData = CharStringData::createTerminated(newLength);
        char* newBuffer = newData->getBuffer();
        int32 ni = 0;
        i = 0;
        while (i < strLength)
        {
            if (strLength - oldCharsLength < i ||
                memcmp(strBuffer + i, oldChars, oldCharsLength) != 0)
            {
                newBuffer[ni] = strBuffer[i];
                ni = ni + 1;
                i = i + 1;
            }
            else
            {
                memcpy(newBuffer + ni, newChars, newCharsLength);
                i = i + oldCharsLength;
                ni = ni + newCharsLength;
            }
        }
        return CharString(newData);
    }
    return CharString();
}

CharString replace(const CharString& str, const CharString& oldChars, const CharString& newChars)
{
    int32 newCharsLength = newChars.length();
    const char* newCharsSrc = newChars.c_str();
    int32 oldCharsLength = oldChars.length();
    const char* oldCharsSrc = oldChars.c_str();
    return replace(str, oldCharsSrc, oldCharsLength, newCharsSrc, newCharsLength);
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

WideString left(const WideString& str, int32 length)
{
    if (length < 0)
    {
        __assert_fail("0 <= length", "../../Include/Core/Strings.cpp", 0x7b3,
                      "WideString left(const WideString&, int32)");
    }
    if (length < str.length())
    {
        return WideString(str.c_str(), length);
    }
    return WideString(str);
}

WideString mid(const WideString& str, int32 start)
{
    if (start < 0)
    {
        __assert_fail("0 <= start", "../../Include/Core/Strings.cpp", 0x7bc,
                      "WideString mid(const WideString&, int32)");
    }
    if (start == 0)
    {
        return WideString(str);
    }
    if (start < str.length())
    {
        return WideString(str.c_str() + start, str.length() - start);
    }
    return WideString();
}

WideString mid(const WideString& str, int32 start, int32 length)
{
    if (start < 0)
    {
        __assert_fail("0 <= start", "../../Include/Core/Strings.cpp", 0x7c8,
                      "WideString mid(const WideString&, int32, int32)");
    }
    if (length < 0)
    {
        __assert_fail("0 <= length", "../../Include/Core/Strings.cpp", 0x7c9,
                      "WideString mid(const WideString&, int32, int32)");
    }
    if (start < str.length())
    {
        if (str.length() < start + length)
        {
            length = str.length() - start;
        }
        return WideString(str.c_str() + start, length);
    }
    return WideString();
}

WideString right(const WideString& str, int32 length)
{
    if (length < 0)
    {
        __assert_fail("0 <= length", "../../Include/Core/Strings.cpp", 0x7d6,
                      "WideString right(const WideString&, int32)");
    }
    if (length < str.length())
    {
        return WideString(str.c_str() + (str.length() - length), length);
    }
    return WideString(str);
}

WideString lower(const WideString& str)
{
    if (str.length() == 0)
    {
        return WideString();
    }
    WideStringData* newData = WideStringData::createTerminated(str.length());
    const wchar* strBuf = str.c_str();
    wchar* newBuf = newData->getBuffer();
    for (int32 i = 0; i < str.length(); i = i + 1)
    {
        newBuf[i] = towlower(strBuf[i]);
    }
    return WideString(newData);
}

WideString upper(const WideString& str)
{
    if (str.length() == 0)
    {
        return WideString();
    }
    WideStringData* newData = WideStringData::createTerminated(str.length());
    const wchar* strBuf = str.c_str();
    wchar* newBuf = newData->getBuffer();
    for (int32 i = 0; i < str.length(); i = i + 1)
    {
        newBuf[i] = towupper(strBuf[i]);
    }
    return WideString(newData);
}

WideString reverse(const WideString& str)
{
    if (str.length() == 0)
    {
        return WideString();
    }
    WideStringData* newData = WideStringData::createTerminated(str.length());
    int32 strLength = str.length();
    const wchar* strBuf = str.c_str();
    wchar* newBuf = newData->getBuffer();
    for (int32 i = 0; i < str.length(); i = i + 1)
    {
        newBuf[i] = strBuf[(strLength - i) - 1];
    }
    return WideString(newData);
}

WideString trimLeft(const WideString& str)
{
    if (str.length() == 0)
    {
        return WideString();
    }
    const wchar* strBuf = str.c_str();
    for (int32 pos = 0; pos <= str.length() - 1; pos = pos + 1)
    {
        if (!WideString::isuspace(strBuf[pos]))
        {
            if (pos == 0)
            {
                return WideString(str);
            }
            WideStringData* newData = WideStringData::createTerminated(str.length() - pos);
            memcpy(newData->getBuffer(), strBuf + pos, (str.length() - pos) * 4);
            return WideString(newData);
        }
    }
    return WideString();
}

WideString trimRight(const WideString& str)
{
    if (str.length() == 0)
    {
        return WideString();
    }
    const wchar* strBuf = str.c_str();
    int32 len = str.length();
    for (int32 pos = len - 1; pos > -1; pos = pos - 1)
    {
        if (!WideString::isuspace(strBuf[pos]))
        {
            if (pos == len - 1)
            {
                return WideString(str);
            }
            WideStringData* newData = WideStringData::createTerminated(pos + 1);
            memcpy(newData->getBuffer(), strBuf, (pos + 1) * 4);
            return WideString(newData);
        }
    }
    return WideString();
}

WideString trim(const WideString& str)
{
    WideString tmp = trimRight(str);
    return trimLeft(tmp);
}

WideString insert(const WideString& str, int32 pos, const wchar* src)
{
    int32 srcLength = (int32)wcslen(src);
    return insert(str, pos, src, srcLength);
}

WideString insert(const WideString& str, int32 pos, const wchar* src, int32 srcLength)
{
    if (pos >= 0 && pos <= str.length())
    {
        if (str.length() == 0)
        {
            return WideString(src, srcLength);
        }
        else if (srcLength == 0)
        {
            return WideString(str);
        }
        else
        {
            WideStringData* newData = WideStringData::createTerminated(str.length() + srcLength);
            memcpy(newData->getBuffer(), str.c_str(), pos * 4);
            memcpy(newData->getBuffer() + pos, src, srcLength * 4);
            memcpy(newData->getBuffer() + pos + srcLength, str.c_str() + pos,
                   (str.length() - pos) * 4);
            return WideString(newData);
        }
    }
    __assert_fail("0 <= pos && pos <= str.length()", "../../Include/Core/Strings.cpp", 0x70f,
                  "WideString insert(const WideString&, int32, const wchar*, int32)");
}

WideString insert(const WideString& str, int32 pos, const WideString& src)
{
    if (str.length() == 0)
    {
        return WideString(src);
    }
    if (src.length() == 0)
    {
        return WideString(str);
    }
    return insert(str, pos, src.c_str(), src.length());
}

WideString insert(const WideString& str, int32 pos, wchar src)
{
    if (pos >= 0 && pos <= str.length())
    {
        if (str.length() == 0)
        {
            return WideString(src);
        }
        else if (src == 0)
        {
            return WideString(str);
        }
        else
        {
            WideStringData* newData = WideStringData::createTerminated(str.length() + 1);
            memcpy(newData->getBuffer(), str.c_str(), pos * 4);
            newData->getBuffer()[pos] = src;
            memcpy(newData->getBuffer() + pos + 1, str.c_str() + pos,
                   (str.length() - pos) * 4);
            return WideString(newData);
        }
    }
    __assert_fail("0 <= pos && pos <= str.length()", "../../Include/Core/Strings.cpp", 0x72e,
                  "WideString insert(const WideString&, int32, wchar)");
}

WideString remove(const WideString& str, int32 pos, int32 removeLength)
{
    if (pos >= 0 && pos <= str.length())
    {
        if (removeLength >= 0)
        {
            if (pos + removeLength <= str.length())
            {
                if (removeLength == 0)
                {
                    return WideString(str);
                }
                if (str.length() == removeLength)
                {
                    return WideString();
                }
                WideStringData* newData = WideStringData::createTerminated(str.length() - removeLength);
                memcpy(newData->getBuffer(), str.c_str(), pos * 4);
                memcpy(newData->getBuffer() + pos, str.c_str() + pos + removeLength,
                       (str.length() - pos - removeLength) * 4);
                return WideString(newData);
            }
        }
        __assert_fail("0 <= removeLength && pos + removeLength <= str.length()",
                      "../../Include/Core/Strings.cpp", 0x743,
                      "WideString remove(const WideString&, int32, int32)");
    }
    __assert_fail("0 <= pos && pos <= str.length()", "../../Include/Core/Strings.cpp", 0x742,
                  "WideString remove(const WideString&, int32, int32)");
}

WideString replace(const WideString& str, wchar pat, wchar target)
{
    if (pat == 0)
    {
        __assert_fail("pat != 0", "../../Include/Core/Strings.cpp", 0x756,
                      "WideString replace(const WideString&, wchar, wchar)");
    }
    if (target == 0)
    {
        __assert_fail("target != 0", "../../Include/Core/Strings.cpp", 0x757,
                      "WideString replace(const WideString&, wchar, wchar)");
    }
    if (str.length() == 0)
    {
        return WideString();
    }
    WideStringData* newData = WideStringData::createTerminated(str.length());
    wchar* newBuffer = newData->getBuffer();
    const wchar* strBuffer = str.c_str();
    int32 strLength = str.length();
    for (int32 i = 0; i < strLength; i = i + 1)
    {
        if (strBuffer[i] == pat)
        {
            newBuffer[i] = target;
        }
        else
        {
            newBuffer[i] = strBuffer[i];
        }
    }
    return WideString(newData);
}

WideString replace(const WideString& str, const wchar* pat, const wchar* target)
{
    size_t targetLength = wcslen(target);
    size_t patLength = wcslen(pat);
    return replace(str, pat, (int32)patLength, target, (int32)targetLength);
}

WideString replace(const WideString& str, const wchar* pat, int32 patLength,
                   const wchar* target, int32 targetLength)
{
    if (patLength == 0)
    {
        return WideString(str);
    }
    const wchar* strBuffer = str.c_str();
    int32 strLength = str.length();
    int32 replaceCount = 0;
    int32 i = 0;
    while (i <= strLength - patLength)
    {
        if (memcmp(strBuffer + i, pat, patLength * 4) == 0)
        {
            replaceCount = replaceCount + 1;
            i = i + patLength;
        }
        else
        {
            i = i + 1;
        }
    }
    if (replaceCount == 0)
    {
        return WideString(str);
    }
    int32 newLength = (targetLength - patLength) * replaceCount + strLength;
    if (newLength != 0)
    {
        WideStringData* newData = WideStringData::createTerminated(newLength);
        wchar* newBuffer = newData->getBuffer();
        int32 ni = 0;
        i = 0;
        while (i < strLength)
        {
            if (strLength - patLength < i ||
                memcmp(strBuffer + i, pat, patLength * 4) != 0)
            {
                newBuffer[ni] = strBuffer[i];
                ni = ni + 1;
                i = i + 1;
            }
            else
            {
                memcpy(newBuffer + ni, target, targetLength * 4);
                i = i + patLength;
                ni = ni + targetLength;
            }
        }
        return WideString(newData);
    }
    return WideString();
}

WideString replace(const WideString& str, const WideString& pat, const WideString& target)
{
    int32 targetLength = target.length();
    const wchar* targetSrc = target.c_str();
    int32 patLength = pat.length();
    const wchar* patSrc = pat.c_str();
    return replace(str, patSrc, patLength, targetSrc, targetLength);
}

WideString operator+(const WideString& src1, const wchar* src2)
{
    return src1.concat(src1, src2);
}

WideString operator+(const WideString& src1, const WideString& src2)
{
    return src1.concat(src1, src2);
}

WideString operator+(const WideString& src1, wchar src2)
{
    return src1.concat(src1, src2);
}

WideString operator+(const wchar* src1, const WideString& src2)
{
    return src2.concat(src1, src2);
}

WideString operator+(wchar src1, const WideString& src2)
{
    return src2.concat(src1, src2);
}

bool operator==(const WideString& src1, const wchar* src2)
{
    if (src1[0] == *src2)
    {
        return src1.compare(src1, src2) == 0;
    }
    return false;
}

bool operator==(const WideString& src1, const WideString& src2)
{
    if (src1.length() == src2.length() && src1[0] == src2[0])
    {
        return src1.compare(src1, src2) == 0;
    }
    return false;
}

bool operator==(const wchar* src1, const WideString& src2)
{
    if (*src1 == src2[0])
    {
        return src2.compare(src1, src2) == 0;
    }
    return false;
}

bool operator!=(const WideString& src1, const wchar* src2)
{
    return src1.compare(src1, src2) != 0;
}

bool operator!=(const WideString& src1, const WideString& src2)
{
    return src1.compare(src1, src2) != 0;
}

bool operator!=(const wchar* src1, const WideString& src2)
{
    return src2.compare(src1, src2) != 0;
}

bool operator<(const WideString& src1, const wchar* src2)
{
    return src1.compare(src1, src2) < 0;
}

bool operator<(const WideString& src1, const WideString& src2)
{
    return src1.compare(src1, src2) < 0;
}

bool operator<(const wchar* src1, const WideString& src2)
{
    return src2.compare(src1, src2) < 0;
}

bool operator<=(const WideString& src1, const wchar* src2)
{
    return src1.compare(src1, src2) < 1;
}

bool operator<=(const WideString& src1, const WideString& src2)
{
    return src1.compare(src1, src2) < 1;
}

bool operator<=(const wchar* src1, const WideString& src2)
{
    return src2.compare(src1, src2) < 1;
}

bool operator>(const WideString& src1, const wchar* src2)
{
    return 0 < src1.compare(src1, src2);
}

bool operator>(const WideString& src1, const WideString& src2)
{
    return 0 < src1.compare(src1, src2);
}

bool operator>(const wchar* src1, const WideString& src2)
{
    return 0 < src2.compare(src1, src2);
}

bool operator>=(const WideString& src1, const wchar* src2)
{
    return src1.compare(src1, src2) >= 0;
}

bool operator>=(const WideString& src1, const WideString& src2)
{
    return src1.compare(src1, src2) >= 0;
}

bool operator>=(const wchar* src1, const WideString& src2)
{
    return src2.compare(src1, src2) >= 0;
}
