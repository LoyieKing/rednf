// SecureStdio.cpp
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <wchar.h>

#include "SecureStdio.h"

int ss_sprintf(char* buffer, size_t count, const char* format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = ss_vsprintf(buffer, count, format, args);
    va_end(args);
    return ret;
}

// ---- ss_vsprintf -----------------------------------------------------
// 行号对齐（ORIG __assert_fail 实参 0x1a/0x1b/0x1c/0x22）
// 
// 
// 
int ss_vsprintf(char* buffer, size_t count, const char* format, char* ap)
{
    assert(buffer != 0);
    assert(count > 0);
    assert(format != 0);
    int ret = vsnprintf(buffer, count, format, (va_list)ap);
    if ((int)count <= ret)
    {
        *buffer = '\0';
        // 行号对齐：assert(false) 在 ORIG 位于第 0x22 行
        assert(false);
    }
    return ret;
}

// ss_vswprintf 行号对齐
int ss_swprintf(wchar_t* buffer, size_t count, const wchar_t* format, ...)
{
    va_list args;
    va_start(args, format);
    int ret = ss_vswprintf(buffer, count, format, (char*)args);
    va_end(args);
    return ret;
}

int ss_vswprintf(wchar_t* buffer, size_t count, const wchar_t* format, char* ap)
{
    assert(buffer != 0);
    assert(count > 0);
    assert(format != 0);
    int ret = vswprintf(buffer, count, format, (va_list)ap);
    if (ret < 0 || (int)count <= ret)
    {
        *buffer = L'\0';
        // 行号对齐：assert(false) 在 ORIG 位于第 0x3b 行
        assert(false);
    }
    return ret;
}

// ss_strcpy 行号对齐
int ss_strcpy(char* dest, size_t count, const char* src)
{
    assert(dest != 0);
    assert(count > 0);
    assert(src != 0);
    int len = strlen(src);
    if ((int)count <= len)
    {
        *dest = '\0';
        // 行号对齐：assert(false) 在 ORIG 位于第 0x4b 行
        assert(false);
    }
    memcpy(dest, src, len + 1);
    return 0;
}

// ss_wcscpy 行号对齐
// 
int ss_wcscpy(wchar_t* dest, size_t count, const wchar_t* src)
{
    assert(dest != 0);
    assert(count > 0);
    assert(src != 0);
    int len = wcslen(src);
    if ((int)count <= len)
    {
        *dest = L'\0';
        // 行号对齐：assert(false) 在 ORIG 位于第 0x5d 行
        assert(false);
    }
    memcpy(dest, src, (len + 1) * 4);
    return 0;
}

// ss_strcat 行号对齐
// 
int ss_strcat(char* dest, size_t count, const char* src)
{
    assert(dest != 0);
    assert(count > 0);
    assert(src != 0);
    int dlen = strlen(dest);
    int slen = strlen(src);
    if ((int)count <= dlen + slen)
    {
        // 行号对齐：assert(false) 在 ORIG 位于第 0x6f 行
        assert(false);
    }
    memcpy(dest + dlen, src, slen + 1);
    return 0;
}

// ss_wcscat 行号对齐
// 
int ss_wcscat(wchar_t* dest, size_t count, const wchar_t* src)
{
    assert(dest != 0);
    assert(count > 0);
    assert(src != 0);
    int dlen = wcslen(dest);
    int slen = wcslen(src);
    if ((int)count <= dlen + slen)
    {
        // 行号对齐：assert(false) 在 ORIG 位于第 0x81 行
        assert(false);
    }
    memcpy(dest + dlen, src, (slen + 1) * 4);
    return 0;
}
