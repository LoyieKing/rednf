#include <stdarg.h>

#include "Exception.h"

Exception::Exception(int iErrorCode)
{
    nLine_ = 0;
    iErrorCode_ = iErrorCode;
    szFunctionNameA_ = NULL;
    szErrorMsg_[0] = '\0';
}

Exception::Exception(const TCHAR* szErrorMsg)
{
    nLine_ = 0;
    iErrorCode_ = 0;
    szFunctionNameA_ = NULL;
}

Exception::Exception(const char* szFunctionNameA, int nLine)
{
    nLine_ = nLine;
    iErrorCode_ = 0;
    szFunctionNameA_ = szFunctionNameA;
    szErrorMsg_[0] = '\0';
}

Exception::Exception(const char* szFunctionNameA, int nLine, int iErrorCode)
{
    nLine_ = nLine;
    iErrorCode_ = iErrorCode;
    szFunctionNameA_ = szFunctionNameA;
}

Exception::Exception(int iErrorCode, const TCHAR* szMessage, ...)
{
    nLine_ = 0;
    iErrorCode_ = iErrorCode;
    szFunctionNameA_ = NULL;

    va_list args;
    va_start(args, szMessage);
    vsprintf(szErrorMsg_, szMessage, args);
    va_end(args);
}

Exception::Exception(const char* szFunctionNameA, int nLine, const TCHAR* szMessage, ...)
{
    nLine_ = nLine;
    szFunctionNameA_ = szFunctionNameA;

    va_list args;
    va_start(args, szMessage);
    vsprintf(szErrorMsg_, szMessage, args);
    va_end(args);
}

// --- Functions defined in this compilation unit ---

// Exception::Exception(char const*)
/* TODO: implement */

// Exception::Exception(char const*, int)
/* TODO: implement */

// Exception::Exception(char const*, int, char const*, ...)
/* TODO: implement */

// Exception::Exception(char const*, int, int)
/* TODO: implement */

// Exception::Exception(int)
/* TODO: implement */

// Exception::Exception(int, char const*, ...)
/* TODO: implement */

// Exception::Exception(char const*)
/* TODO: implement */

// Exception::Exception(char const*, int)
/* TODO: implement */

// Exception::Exception(char const*, int, char const*, ...)
/* TODO: implement */

// Exception::Exception(char const*, int, int)
/* TODO: implement */

// Exception::Exception(int)
/* TODO: implement */

// Exception::Exception(int, char const*, ...)
/* TODO: implement */
