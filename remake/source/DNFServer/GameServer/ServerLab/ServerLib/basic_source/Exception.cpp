#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

#include "Exception.h"

namespace nsl {

Exception::Exception(int iErrorCode)
{
    iErrorCode_ = iErrorCode;
    nLine_ = 0;
    szFunctionNameA_ = NULL;
    szErrorMsg_[0] = '\0';
}

Exception::Exception(const char* szFunctionNameA, int nLine)
{
    iErrorCode_ = 0;
    nLine_ = nLine;
    szFunctionNameA_ = szFunctionNameA;
    szErrorMsg_[0] = '\0';
}

Exception::Exception(const char* szFunctionNameA, int nLine, int iErrorCode)
{
    iErrorCode_ = iErrorCode;
    nLine_ = nLine;
    szFunctionNameA_ = szFunctionNameA;
}

Exception::Exception(const char* szErrorMsg)
{
    iErrorCode_ = 0;
    nLine_ = 0;
    szFunctionNameA_ = NULL;
}

Exception::Exception(int iErrorCode, const char* szMessage, ...)
{
    iErrorCode_ = iErrorCode;
    nLine_ = 0;
    szFunctionNameA_ = NULL;

    va_list args;
    va_start(args, szMessage);
    vsprintf(szErrorMsg_, szMessage, args);
    va_end(args);
}

Exception::Exception(const char* szFunctionNameA, int nLine, const char* szMessage, ...)
{
    nLine_ = nLine;
    szFunctionNameA_ = szFunctionNameA;

    va_list args;
    va_start(args, szMessage);
    vsprintf(szErrorMsg_, szMessage, args);
    va_end(args);
}

} // namespace nsl
