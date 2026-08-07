// Rebuilt from df_auction_r (DWARF + Ghidra decompile), 2026-08-08
#include "UnicodeConvert.h"

#include <stdlib.h>

static int gCodePage;

static __thread char g_szMbcsBuffer[10][0x1000];
static __thread int g_nMbcsBufferIndex;
static __thread wchar_t g_szUnicodeBuffer[10][0x1000];
static __thread int g_nUnicodeBufferIndex;
static __thread char g_szTCharBuffer[10][0x1000];
static __thread int g_nTCharBufferIndex;

int getCodePage()
{
    return gCodePage;
}

int setCodePage(int nNewCodePage)
{
    int last_code_page = gCodePage;
    gCodePage = nNewCodePage;
    return last_code_page;
}

char* getMbcsBuffer()
{
    return g_szMbcsBuffer[g_nMbcsBufferIndex];
}

wchar_t* getUnicodeBuffer()
{
    return g_szUnicodeBuffer[g_nUnicodeBufferIndex];
}

char* getTCharBuffer()
{
    return g_szTCharBuffer[g_nTCharBufferIndex];
}

int getMbcsBufferSize()
{
    return 0x1000;
}

int getUnicodeBufferSize()
{
    return 0x4000;
}

int getTCharBufferSize()
{
    return 0x1000;
}

const char* toTString(const std::string& sSrc)
{
    return sSrc.c_str();
}

const char* toTString(const std::wstring& sSrc)
{
    int curIndex = g_nTCharBufferIndex;
    size_t nRet = wcstombs(g_szTCharBuffer[curIndex], sSrc.c_str(), 0x1000);
    g_szTCharBuffer[curIndex][nRet] = 0;
    g_nTCharBufferIndex = (g_nTCharBufferIndex + 1) % 10;
    return g_szTCharBuffer[curIndex];
}

const char* toTString(const char* pszSrc)
{
    return pszSrc;
}

const char* toTString(const wchar_t* pszSrc)
{
    int curIndex = g_nTCharBufferIndex;
    size_t nRet = wcstombs(g_szTCharBuffer[curIndex], pszSrc, 0x1000);
    g_szTCharBuffer[curIndex][nRet] = 0;
    g_nTCharBufferIndex = (g_nTCharBufferIndex + 1) % 10;
    return g_szTCharBuffer[curIndex];
}

void toTString(const std::string& sSrc, std::string& sDst)
{
    sDst = sSrc;
}

void toTString(const std::wstring& sSrc, std::string& sDst)
{
    int curIndex = g_nTCharBufferIndex;
    size_t nRet = wcstombs(g_szTCharBuffer[curIndex], sSrc.c_str(), 0x1000);
    g_szTCharBuffer[curIndex][nRet] = 0;
    sDst = g_szTCharBuffer[curIndex];
    g_nTCharBufferIndex = (g_nTCharBufferIndex + 1) % 10;
}

void toTString(const char* pszSrc, std::string& sDst)
{
    sDst = pszSrc;
}

void toTString(const wchar_t* pszSrc, std::string& sDst)
{
    int curIndex = g_nTCharBufferIndex;
    size_t nRet = wcstombs(g_szTCharBuffer[curIndex], pszSrc, 0x1000);
    g_szTCharBuffer[curIndex][nRet] = 0;
    sDst = g_szTCharBuffer[curIndex];
    g_nTCharBufferIndex = (g_nTCharBufferIndex + 1) % 10;
}

const char* toMbcs(const std::string& sSrc)
{
    return sSrc.c_str();
}

const char* toMbcs(const std::wstring& sSrc)
{
    int curIndex = g_nMbcsBufferIndex;
    size_t nRet = wcstombs(g_szMbcsBuffer[curIndex], sSrc.c_str(), 0x1000);
    g_szMbcsBuffer[curIndex][nRet] = 0;
    g_nMbcsBufferIndex = (g_nMbcsBufferIndex + 1) % 10;
    return g_szMbcsBuffer[curIndex];
}

const char* toMbcs(const char* pszSrc)
{
    return pszSrc;
}

const char* toMbcs(const wchar_t* pszSrc)
{
    int curIndex = g_nMbcsBufferIndex;
    size_t nRet = wcstombs(g_szMbcsBuffer[curIndex], pszSrc, 0x1000);
    g_szMbcsBuffer[curIndex][nRet] = 0;
    g_nMbcsBufferIndex = (g_nMbcsBufferIndex + 1) % 10;
    return g_szMbcsBuffer[curIndex];
}

void toMbcs(const std::string& sSrc, std::string& sDst)
{
    sDst = sSrc;
}

void toMbcs(const std::wstring& sSrc, std::string& sDst)
{
    int curIndex = g_nMbcsBufferIndex;
    size_t nRet = wcstombs(g_szMbcsBuffer[curIndex], sSrc.c_str(), 0x1000);
    g_szMbcsBuffer[curIndex][nRet] = 0;
    sDst = g_szMbcsBuffer[curIndex];
    g_nMbcsBufferIndex = (g_nMbcsBufferIndex + 1) % 10;
}

void toMbcs(const char* pszSrc, std::string& sDst)
{
    sDst = pszSrc;
}

void toMbcs(const wchar_t* pszSrc, std::string& sDst)
{
    int curIndex = g_nMbcsBufferIndex;
    size_t nRet = wcstombs(g_szMbcsBuffer[curIndex], pszSrc, 0x1000);
    g_szMbcsBuffer[curIndex][nRet] = 0;
    sDst = g_szMbcsBuffer[curIndex];
    g_nMbcsBufferIndex = (g_nMbcsBufferIndex + 1) % 10;
}

const wchar_t* toUnicode(const std::string& sSrc)
{
    int curIndex = g_nUnicodeBufferIndex;
    size_t nRet = mbstowcs(g_szUnicodeBuffer[curIndex], sSrc.c_str(), 0x1000);
    g_szUnicodeBuffer[curIndex][nRet] = 0;
    g_nUnicodeBufferIndex = (g_nUnicodeBufferIndex + 1) % 10;
    return g_szUnicodeBuffer[curIndex];
}

const wchar_t* toUnicode(const std::wstring& sSrc)
{
    return sSrc.c_str();
}

const wchar_t* toUnicode(const char* pszSrc)
{
    int curIndex = g_nUnicodeBufferIndex;
    size_t nRet = mbstowcs(g_szUnicodeBuffer[curIndex], pszSrc, 0x1000);
    g_szUnicodeBuffer[curIndex][nRet] = 0;
    g_nUnicodeBufferIndex = (g_nUnicodeBufferIndex + 1) % 10;
    return g_szUnicodeBuffer[curIndex];
}

const wchar_t* toUnicode(const wchar_t* pszSrc)
{
    return pszSrc;
}

void toUnicode(const std::string& sSrc, std::wstring& sDst)
{
    int curIndex = g_nUnicodeBufferIndex;
    size_t nRet = mbstowcs(g_szUnicodeBuffer[curIndex], sSrc.c_str(), 0x1000);
    g_szUnicodeBuffer[curIndex][nRet] = 0;
    sDst = g_szUnicodeBuffer[curIndex];
    g_nUnicodeBufferIndex = (g_nUnicodeBufferIndex + 1) % 10;
}

void toUnicode(const std::wstring& sSrc, std::wstring& sDst)
{
    sDst = sSrc;
}

void toUnicode(const char* pszSrc, std::wstring& sDst)
{
    int curIndex = g_nUnicodeBufferIndex;
    size_t nRet = mbstowcs(g_szUnicodeBuffer[curIndex], pszSrc, 0x1000);
    g_szUnicodeBuffer[curIndex][nRet] = 0;
    sDst = g_szUnicodeBuffer[curIndex];
    g_nUnicodeBufferIndex = (g_nUnicodeBufferIndex + 1) % 10;
}

void toUnicode(const wchar_t* pszSrc, std::wstring& sDst)
{
    sDst = pszSrc;
}

WideString convertToWideString(const char* src)
{
    return convertToWideString(src, gCodePage);
}

WideString convertToWideString(const char* src, uint32 codePage)
{
    size_t size = mbstowcs((wchar_t*)0, src, 0);
    if (size == 0xffffffff)
    {
        return WideString();
    }
    int32 newLength = (int32)(size >> 2);
    if (newLength == 0)
    {
        return WideString();
    }
    WideStringData* newData = WideStringData::create(newLength);
    mbstowcs(newData->getBuffer(), src, (newLength + 1) * 4);
    return WideString(newData);
}

WideString convertToWideString(const CharString& src)
{
    return convertToWideString(src.c_str());
}

WideString convertToWideString(const CharString& src, uint32 codePage)
{
    return convertToWideString(src.c_str(), codePage);
}

WideString convertToWideString(const WideString& src)
{
    return WideString(src);
}

CharString convertToCharString(const wchar* src)
{
    return convertToCharString(src, gCodePage);
}

CharString convertToCharString(const wchar* src, uint32 codePage)
{
    size_t length = wcstombs((char*)0, src, 0);
    if (length == 0xffffffff)
    {
        return CharString();
    }
    int32 newLength = (int32)length;
    if (newLength == 0)
    {
        return CharString();
    }
    CharStringData* newData = CharStringData::create(newLength);
    wcstombs(newData->getBuffer(), src, newLength + 1);
    return CharString(newData);
}

CharString convertToCharString(const WideString& src)
{
    return convertToCharString(src.c_str());
}

CharString convertToCharString(const WideString& src, uint32 codePage)
{
    return convertToCharString(src.c_str(), codePage);
}

CharString convertToCharString(const CharString& src)
{
    return CharString(src);
}

bool convertToUtf8(const wchar* source, int32 sourceLen, CharString* dest)
{
    int32 newLength = 0;
    int32 i = 0;
    while (i < sourceLen)
    {
        int32 c = source[i];
        if (c <= 0x7f)
        {
            newLength = newLength + 1;
            i = i + 1;
        }
        else if (c <= 0x7ff)
        {
            newLength = newLength + 2;
            i = i + 1;
        }
        else
        {
            newLength = newLength + 3;
            i = i + 1;
        }
    }
    if (newLength == 0)
    {
        *dest = "";
        return true;
    }
    else
    {
        CharStringData* newData = CharStringData::createTerminated(newLength);
        char* newBuf = newData->getBuffer();
        for (int32 i2 = 0; i2 < sourceLen; i2 = i2 + 1)
        {
            int32 c = source[i2];
            if (c <= 0x7f)
            {
                *newBuf = (char)c;
                newBuf = newBuf + 1;
            }
            else if (c <= 0x7ff)
            {
                *newBuf = (char)((c >> 6) | 0xc0);
                newBuf = newBuf + 1;
                *newBuf = (char)((c & 0x3f) | 0x80);
                newBuf = newBuf + 1;
            }
            else
            {
                *newBuf = (char)((c >> 12) | 0xe0);
                newBuf = newBuf + 1;
                *newBuf = (char)(((c >> 6) & 0x3f) | 0x80);
                newBuf = newBuf + 1;
                *newBuf = (char)((c & 0x3f) | 0x80);
                newBuf = newBuf + 1;
            }
        }
        CharString tmp(newData);
        *dest = tmp;
    }
    return true;
}

bool convertFromUtf8(const char* source, int32 sourceLen, WideString* dest)
{
    int32 newLength = 0;
    int32 i = 0;
    while (i < sourceLen)
    {
        unsigned char c = (unsigned char)source[i];
        if ((char)c >= 0)
        {
            newLength = newLength + 1;
            i = i + 1;
        }
        else
        {
            if (c <= 0xdf && i + 1 < sourceLen)
            {
                newLength = newLength + 1;
                i = i + 2;
            }
            else if (c <= 0xef && i + 2 < sourceLen)
            {
                newLength = newLength + 1;
                i = i + 3;
            }
            else
            {
                return false;
            }
        }
    }
    if (newLength == 0)
    {
        *dest = L"";
        return true;
    }
    else
    {
        WideStringData* newData = WideStringData::createTerminated(newLength);
        wchar_t* newBuf = newData->getBuffer();
        int32 i2 = 0;
        while (i2 < sourceLen)
        {
            unsigned char c = (unsigned char)source[i2];
            if ((char)c >= 0)
            {
                *newBuf = c;
                newBuf = newBuf + 1;
                i2 = i2 + 1;
            }
            else
            {
                if (c <= 0xdf && i2 + 1 < sourceLen)
                {
                    *newBuf = (c & 0x1f) * 0x40 + ((int)source[i2 + 1] & 0x3f);
                    newBuf = newBuf + 1;
                    i2 = i2 + 2;
                }
                else if (c <= 0xef && i2 + 2 < sourceLen)
                {
                    *newBuf = (unsigned short)(c << 12) +
                              ((int)source[i2 + 1] & 0x3f) * 0x40 +
                              ((int)source[i2 + 2] & 0x3f);
                    newBuf = newBuf + 1;
                    i2 = i2 + 3;
                }
                else
                {
                    return false;
                }
            }
        }
        WideString tmp(newData);
        *dest = tmp;
    }
    return true;
}
