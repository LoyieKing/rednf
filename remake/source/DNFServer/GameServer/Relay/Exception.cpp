// df_relay_r — Exception（GCC 4.1.2, 无 DWARF — Ghidra 反汇编还原）
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include "Exception.h"

Exception::Exception(int iErrorCode)
{
    nLine_ = 0;
    iErrorCode_ = iErrorCode;
    szFunctionNameA_ = 0;
    szErrorMsg_[0] = 0;
}

Exception::Exception(const char* szFunctionNameA, int nLine)
{
    nLine_ = nLine;
    iErrorCode_ = 0;
    szFunctionNameA_ = szFunctionNameA;
    szErrorMsg_[0] = 0;
}

Exception::Exception(const char* szFunctionNameA, int nLine, int iErrorCode)
{
    nLine_ = nLine;
    iErrorCode_ = iErrorCode;
    szFunctionNameA_ = szFunctionNameA;
    szErrorMsg_[0] = 0;
}

Exception::Exception(const char* szErrorMsg)
{
    nLine_ = 0;
    iErrorCode_ = 0;
    szFunctionNameA_ = 0;
    strncpy(szErrorMsg_, szErrorMsg, 0xfff);
    szErrorMsg_[0xfff] = 0;
}

Exception::Exception(int iErrorCode, const char* szMessage, ...)
{
    nLine_ = 0;
    iErrorCode_ = iErrorCode;
    szFunctionNameA_ = 0;
    va_list ap;
    va_start(ap, szMessage);
    vsprintf(szErrorMsg_, szMessage, ap);
    va_end(ap);
}

Exception::Exception(const char* szFunctionNameA, int nLine, const char* szMessage, ...)
{
    nLine_ = nLine;
    iErrorCode_ = 0;
    szFunctionNameA_ = szFunctionNameA;
    va_list ap;
    va_start(ap, szMessage);
    vsprintf(szErrorMsg_, szMessage, ap);
    va_end(ap);
}
